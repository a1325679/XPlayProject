#pragma once
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
#include<mutex>
using ll = long long;

extern void XFreePacket(AVPacket** pkt);
extern void XFreeFrame(AVFrame** frame);

class XDecoder
{
public:
	bool isAudio = false;
	virtual bool Open(AVCodecParameters* para);
	virtual AVFrame* Recv();
	virtual bool Send(AVPacket *pkt);
	virtual void Close();
	virtual void Clear();
	XDecoder();
	virtual ~XDecoder();

	ll pts = 0;
	AVCodecContext* codec = 0;
	std::mutex mux;
};

