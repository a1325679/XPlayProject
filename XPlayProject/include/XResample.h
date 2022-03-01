#pragma once
struct AVCodecParameters;
struct AVFrame;
struct SwrContext;
class XResample
{
public:
	virtual bool Open(AVCodecParameters* para);
	virtual void Close();

	//返回重采样后大小,不管成功与否都释放indata空间
	virtual int Resample(AVFrame* indata, unsigned char* data);
	XResample();
	~XResample();
	int outFormat = 1;

	SwrContext* actx = 0;
};

