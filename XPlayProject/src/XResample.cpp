#include "XResample.h"
#include<iostream>
using namespace std;
extern "C" {
#include<libswresample/swresample.h>
#include<libavcodec/avcodec.h>
}
#pragma comment(lib,"swresample.lib")

void XResample::Close() {
	if (actx) {
		swr_free(&actx);
	}
}
bool XResample::Open(AVCodecParameters* para){
	if (!para) {
		return false;
	}
	actx = swr_alloc_set_opts(actx,av_get_default_channel_layout(2),
		(AVSampleFormat)outFormat,para->sample_rate, av_get_default_channel_layout(para->channels),//ÊäÈë¸ñÊ½
		(AVSampleFormat)para->format,
		para->sample_rate,
		0, 0
		);
	int ret = swr_init(actx);
	if (ret != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "swr_init  failed! :" << buf << endl;
		return false;
	}
	return true;
}
int XResample::Resample(AVFrame* indata, unsigned char* d) {
	if (!indata)return 0;
	if (!d) {
		av_frame_free(&indata);
		return 0;
	}
	uint8_t* data[2] = { 0 };
	data[0] = d;
	int ret = swr_convert(actx, data, indata->nb_samples, (const uint8_t**)indata, indata->nb_samples);
	if (ret <= 0)return ret; int outSize = ret * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
	return outSize;
}

XResample::XResample()
{
}


XResample::~XResample()
{
}