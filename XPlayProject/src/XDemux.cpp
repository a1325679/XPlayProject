#include "XDemux.h"
#include<iostream>

#include<mutex>
using namespace std;

extern "C" {
#include<libavformat/avformat.h>
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")

static double r2d(AVRational r) {
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}
XDemux::XDemux() {
	px = NULL;
}
AVPacket* XDemux::Read() {
	mux.lock();
	if (!px) {
		mux.unlock();
		return NULL;
	}
	AVPacket* pkt = av_packet_alloc();
	int ret = av_read_frame(px,pkt);
	if (ret != 0) {
		mux.unlock();
		av_packet_free(&pkt);
		return 0;
	}
	//pts转换为秒
	pkt->pts = pkt->pts * (1000 * r2d(px->streams[pkt->stream_index]->time_base));
	pkt->dts = pkt->dts * (1000 *r2d(px->streams[pkt->stream_index]->time_base));
	mux.unlock();
	cout << pkt->pts << " " << flush;
	return pkt;
}
AVPacket* XDemux::ReadVideo() {
	mux.lock();
	if (!px) {
		return NULL;
	}
	mux.unlock();
	AVPacket* pkt = NULL;
	for (int i = 0; i < 20; i++) {
		pkt = Read();
		if (!pkt) {
			break;
		}
		if (pkt->stream_index == videoStream) {
			break;
		}
		av_packet_free(&pkt);
	}
	return pkt;
}
void XDemux::Close()
{
	if (!px)
	{
		return;
	}
	avformat_close_input(&px);
}

bool XDemux::Open(const char* path) {
	Close();
	mux.lock();
	int ret = avformat_open_input(&px,path,0,0);
	if (ret != 0)
	{
		mux.unlock();
		return false;
	}
	ret = avformat_find_stream_info(px, 0);
	//打印视频流详细信息
	av_dump_format(px, 0, path, 0);
	//获得视频流
	videoStream = av_find_best_stream(px, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	AVStream* vs = px->streams[videoStream];
	width = vs->codecpar->width;
	height = vs->codecpar->height;
	this->totalMs = px->duration / (AV_TIME_BASE / 1000);
	//获取音频流

	audioStream = av_find_best_stream(px, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	AVStream* as= px->streams[audioStream];

	sampleRate = as->codecpar->sample_rate;
	channels = as->codecpar->channels;
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "sample_rate = " << as->codecpar->sample_rate << endl;
	//AVSampleFormat;
	cout << "channels = " << as->codecpar->channels << endl;
	//一帧数据？？ 单通道样本数 
	cout << "frame_size = " << as->codecpar->frame_size << endl;
	//1024 * 2 * 2 = 4096  fps = sample_rate/frame_size
	mux.unlock();
	return true;
}

AVCodecParameters* XDemux::CopyVPara()
{
	mux.lock();
	if (!px)
	{
		mux.unlock();
		return NULL;
	}
	AVCodecParameters* pa = avcodec_parameters_alloc();
	avcodec_parameters_copy(pa, px->streams[videoStream]->codecpar);
	mux.unlock();

	return pa;
}
AVCodecParameters* XDemux::CopyAPara()
{
	mux.lock();
	if (!px)
	{
		mux.unlock();
		return NULL;
	}
	AVCodecParameters* pa = avcodec_parameters_alloc();
	avcodec_parameters_copy(pa, px->streams[audioStream]->codecpar);
	mux.unlock();
	return pa;
}
bool XDemux::IsAudio(AVPacket* pkt)
{
	if (!pkt) return false;
	if (pkt->stream_index == videoStream)
		return false;
	return true;
}
//清空读取缓存
void XDemux::Clear()
{
	if (!px)
	{
		return;
	}
	//清理读取缓冲
	avformat_flush(px);
}
bool XDemux::Seek(double pos) {
	return true;
}
