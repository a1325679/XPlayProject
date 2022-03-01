#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>
#include "XVideoWidget.h"

using namespace std;

void XDemuxThread::setPause(bool isPause) {
	mux.lock();
	this->isPause = isPause;
	if (at)	at->setPause(isPause);
	if (vt)vt->setPause(isPause);
	mux.unlock();
}
bool XDemuxThread::Open(const char* url, XVideoWidget* call) {
	if (url == 0 || url[0] == '\0')
		return false;
	mux.lock();
	if (!demux) demux = new XDemux();
	if (!vt) vt = new XVideoThread();
	if (!at) at = new XAudioThread();
	//打开解封装
	bool ret = demux->Open(url);
	if (!ret)
	{
		mux.unlock();
		cout << "demux->Open(url) failed!" << endl;
		return false;
	}
	//打开视频解码器和处理线程
	if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
	{
		ret = false;
		cout << "vt->Open failed!" << endl;
	}
	//打开音频解码器和处理线程
	if (!at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels))
	{
		ret = false;
		cout << "at->Open failed!" << endl;
	}
	mux.unlock();
	totalMs = demux->totalMs;
	cout << "XDemuxThread::Open " << ret << endl;
	return ret;
}
//启动所有线程
void XDemuxThread::Start() {
	mux.lock();
	if (!demux) demux = new XDemux();
	if (!vt) vt = new XVideoThread();
	if (!at) at = new XAudioThread();
	//启动当前线程
	QThread::start();
	if (vt)vt->start();
	if (at)at->start();
	mux.unlock();

}

void XDemuxThread::run() {
	while (!isExit)
	{
		mux.lock();
		if (isPause) {
			mux.unlock();
			msleep(5);
			continue;
		}
		if (!demux)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		if (vt && at) {
			pts = at->pts;
			vt->synpts = at->pts;
		}
		AVPacket* pkt = demux->Read();
		if (!pkt)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		//判断数据是音频
		if (demux->IsAudio(pkt))
		{
			if (at)at->Push(pkt);
		}
		else //视频
		{
			if (vt)vt->Push(pkt);
		}
		mux.unlock();
		msleep(1);
	}
}

//关闭线程清理资源
void XDemuxThread::Close()
{
	isExit = true;
	wait();
	if (vt) vt->Close();
	if (at) at->Close();
	mux.lock();
	delete vt;
	delete at;
	vt = NULL;
	at = NULL;
	mux.unlock();
}



XDemuxThread::XDemuxThread()
{

}


XDemuxThread::~XDemuxThread()
{
	isExit = true;
	wait();
}

void XDemuxThread::Clear()
{
	mux.lock();
	if (demux)demux->Clear();
	if (vt) vt->Clear();
	if (at) at->Clear();
	mux.unlock();
}
void XDemuxThread::Seek(double pos)
{
	//清理缓存
	Clear();

	mux.lock();
	bool status = this->isPause;
	mux.unlock();
	//暂停
	setPause(true);

	mux.lock();
	if (demux)
		demux->Seek(pos);
	//实际要显示的位置pts
	long long seekPts = pos * demux->totalMs;
	while (!isExit)
	{
		AVPacket* pkt = demux->ReadVideo();
		if (!pkt) break;
		//如果解码到seekPts
		if (vt->RepaintPts(pkt, seekPts))
		{
			this->pts = seekPts;
			break;
		}
		//bool re = vt->decode->Send(pkt);
		//if (!re) break;
		//AVFrame *frame = vt->decode->Recv();
		//if (!frame) continue;
		////到达位置
		//if (frame->pts >= seekPts)
		//{
		//	this->pts = frame->pts;
		//	vt->call->Repaint(frame);
		//	break;
		//}
		//av_frame_free(&frame);
	}

	mux.unlock();

	//seek是非暂停状态
	if (!status)
		setPause(false);
}