#include "XVideoThread.h"
#include "XDecoder.h"
#include<iostream>
bool XVideoThread::Open(AVCodecParameters* ac, XVideoWidget* xv, int width, int height) {
	//ac是否为空
	
	if (ac == NULL) {
		return false;
	}
	//清理list中的数据
	Clear();
	vmux.lock();
	//同步PTS设为0
	synpts = 0;
	//为vedio初始化
	video = xv;
	if (video) {
		video->Init(width,height);
	}

	vmux.unlock();
	//打开解码器
	if(!decode)
		decode = new XDecoder();
	int ret = true;

	if (!decode->Open(ac))
	{
		std::cout << "audio XDecode open failed!" << endl;
		ret = false;
	}
	std::cout << "XVedioThread::Open :" << ret << endl;
	return ret;
}
void XVideoThread::Push(AVPacket* pkt) {
	if (!pkt)return;
	while (!isExit)
	{
		if (packs.size() < maxList)
		{
			packs.push_back(pkt);
			break;
		}
		msleep(1);
	}
}
void XVideoThread::setPause(bool isPause) {
	vmux.lock();
	this->isPause = isPause;
	vmux.unlock();
}
void XVideoThread::run() {
	while (!isExit) {
		vmux.lock();
		if (this->isPause) {
			vmux.unlock();
			msleep(5);
			continue;
		}
		if (synpts > 0 && decode->pts > synpts) {
			vmux.unlock();
			msleep(1);
			continue;
		}
		AVPacket* pkt = Pop();
		bool ret = decode->Send(pkt);
		if (!ret)
		{
			vmux.unlock();
			msleep(1);
			continue;
		}
		while (!isExit) {
			AVFrame* frame = decode->Recv();
			if (!frame)break;
			if (video) {
				video->Repaint(frame);

			}
		}
		vmux.unlock();
	}
}
XVideoThread::XVideoThread()
{
}
XVideoThread::~XVideoThread()
{
	//等待线程退出
	isExit = true;
	wait();
}

