#include "XDecodeThread.h"
#include "XDecoder.h"

void XDecodeThread::Clear() {
	mux.lock();
	decode->Clear();
	while (!packs.empty()) {
		AVPacket* pkt = packs.front();
		XFreePacket(&pkt);
		packs.pop_front();
	}
	mux.unlock();
}
void XDecodeThread::Close() {
	Clear();
	isExit = true;
	wait();
	mux.lock();
	decode->Close();
	delete decode;
	decode = NULL;
	mux.unlock();
}
void XDecodeThread::Push(AVPacket *pkt){
	if (pkt == NULL) {
		return;
	}
	while (!isExit) {
		mux.lock();
		if (packs.size() < maxList) {
			packs.push_back(pkt);
			mux.unlock();
			break;
		}
		mux.unlock();
		msleep(1);
	}
}
AVPacket* XDecodeThread::Pop() {
	if (packs.empty()) {
		return NULL;
	}
	mux.lock();
	AVPacket* pkt = packs.front();
	packs.pop_front();
	mux.unlock();
	return pkt;
}

XDecodeThread::XDecodeThread()
{
	//打开解码器
	if (!decode) decode = new XDecoder();
}

XDecodeThread::~XDecodeThread()
{	//等待线程退出
	isExit = true;
	wait();
}