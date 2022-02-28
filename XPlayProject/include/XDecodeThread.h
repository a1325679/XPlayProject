#pragma once
struct AVPacket;
class XDecoder;
#include <list>
#include <mutex>
#include <QThread>
class XDecodeThread:public QThread
{
public:
	XDecodeThread();
	~XDecodeThread();
	virtual void Push(AVPacket *pkt);
	virtual AVPacket* Pop();
	virtual void Clear();
	virtual void Close();

	int maxList = 100;
	bool isExit = false;
	XDecoder* decode = 0;
protected:
	std::list <AVPacket*> packs;
	std::mutex mux;
};

