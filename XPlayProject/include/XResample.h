#pragma once
struct AVCodecParameters;
struct AVFrame;
struct SwrContext;
class XResample
{
public:
	virtual bool Open(AVCodecParameters* para);
	virtual void Close();

	//�����ز������С,���ܳɹ�����ͷ�indata�ռ�
	virtual int Resample(AVFrame* indata, unsigned char* data);
	XResample();
	~XResample();
	int outFormat = 1;

	SwrContext* actx = 0;
};

