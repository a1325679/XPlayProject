#pragma once
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
#include<mutex>
using ll = long long;
class XDecoder
{
public:
	bool isAudio;
	virtual bool Open(AVCodecParameters* para);
	virtual AVFrame* Recv();
	virtual bool Send(AVPacket *pkt);
	virtual void Close();
	virtual void Clear();
	XDecoder();
	virtual ~XDecoder();

	ll pts;
	AVCodecContext* codec = 0;
	std::mutex mux;
};

