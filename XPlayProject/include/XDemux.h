#pragma once
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
#include <mutex>
class XDemux
{
public:
	XDemux();
	AVPacket* ReadVideo();
	bool Open(const char* path);
	AVPacket* Read();

	//seek Œª÷√ pos 0.0 ~1.0
	virtual bool Seek(double pos);

	void Clear();
	void Close();
	AVCodecParameters* CopyVPara();

	AVCodecParameters* CopyAPara();

	bool IsAudio(AVPacket* pkt);


	AVFormatContext* px = NULL;
	int videoStream;
	int audioStream;

	int totalMs = 0;


	int channels;

	int sampleRate;
	int width;
	int height;
	std::mutex mux;
};

