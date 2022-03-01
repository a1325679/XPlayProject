#pragma once
#include <QThread>
#include <mutex>
#include <list>
#include"XDecoder.h"
#include"XDecodeThread.h"
struct AVCodecParameters;
class XDecoder;
class XAudioPlay;
class XResample;
class AVPacket;
class XAudioThread :public XDecodeThread
{
public:
	virtual void Close();
	bool Open(AVCodecParameters* para, int sampleRate, int channels);
	void run();
	XAudioThread();
	void setPause(bool isPause);
	virtual ~XAudioThread();

	//最大队列
	int maxList = 100;

	//当前音频播放的pts
	long long pts = 0;

	bool isExit = false;
	bool isPause = false;
	XAudioPlay* ap = 0;
	XResample* res = 0;
	std::mutex amux;
};

