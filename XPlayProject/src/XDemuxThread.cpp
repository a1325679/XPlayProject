#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include <iostream>
#include "XVideoWidget.h"


using namespace std;

void XDemuxThread::setPause(bool isPause) {
	mux.lock();
	this->isPause = isPause;

	if (vt)vt->setPause(isPause);
	mux.unlock();
}
bool XDemuxThread::Open(const char* url, XVideoWidget* call) {
	if (url == 0 || url[0] == '\0')
		return false;
	mux.lock();
	if (!demux) demux = new XDemux();
	if (!vt) vt = new XVideoThread();
	//�򿪽��װ
	bool ret = demux->Open(url);
	if (!ret)
	{
		mux.unlock();
		cout << "demux->Open(url) failed!" << endl;
		return false;
	}
	//����Ƶ�������ʹ����߳�
	if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
	{
		ret = false;
		cout << "vt->Open failed!" << endl;
	}
	mux.unlock();
	totalMs = demux->totalMs;
	cout << "XDemuxThread::Open " << ret << endl;
	return ret;
}
//���������߳�
void XDemuxThread::Start() {
	mux.lock();
	if (!demux) demux = new XDemux();
	if (!vt) vt = new XVideoThread();
	//������ǰ�߳�
	QThread::start();
	if (vt)vt->start();
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
		AVPacket* pkt = demux->Read();
		//if (vt ){
		//	pts = vt->decode->pts;
		//	vt->synpts = pts;
		//}
		if (!pkt)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		//�ж���������Ƶ
		if (demux->IsAudio(pkt))
		{
			//if (at)at->Push(pkt);
		}
		else //��Ƶ
		{
			if (vt)vt->Push(pkt);
		}
		mux.unlock();
		msleep(1);
	}
}

//�ر��߳�������Դ
void XDemuxThread::Close()
{
	isExit = true;
	wait();
	if (vt) vt->Close();
	mux.lock();
	delete vt;
	vt = NULL;
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
	mux.unlock();
}
