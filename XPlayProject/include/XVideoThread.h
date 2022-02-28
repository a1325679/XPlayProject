#pragma once
struct AVPacket;
struct AVCodecParameters;
#include"XDecoder.h"
#include <list>
#include <mutex>
#include <QThread>
#include"XVideoWidget.h"
#include"XDecodeThread.h"
class XVideoThread:public XDecodeThread
{
	bool Open(AVCodecParameters* ac, XVideoWidget* xv, int widthj, int height);
	void run();
	void Push(AVPacket* pkt);
	void setPause(bool isPause);
	bool RepaintPts(AVPacket* pkt, ll seekPts);

	XVideoThread();
	~XVideoThread();

	int maxList = 100;
	bool isExit = false;
	long long synpts = 0;
	XVideoWidget* video;
	std::mutex vmux;
	bool isPause = false;
};

