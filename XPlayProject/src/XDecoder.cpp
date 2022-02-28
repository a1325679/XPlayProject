#include "XDecoder.h"
#include<iostream>
using namespace std;
extern "C"
{
#include<libavcodec/avcodec.h>
}
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")


void XFreePacket(AVPacket** pkt)
{
	if (!pkt || !(*pkt))return;
	av_packet_free(pkt);
}
void XFreeFrame(AVFrame** frame)
{
	if (!frame || !(*frame))return;
	av_frame_free(frame);
}

void XDecoder::Close() {
	mux.lock();
	if (codec) {
		avcodec_close(codec);
		avcodec_free_context(&codec);
	}
	pts = 0;
	mux.unlock();
}
void XDecoder::Clear()
{
	mux.lock();
	//������뻺��
	if (codec)
		avcodec_flush_buffers(codec);
	mux.unlock();
}

bool XDecoder::Open(AVCodecParameters* para) {

	if (para == NULL) {
		cout << "AVCodecParameters is Null!" << endl;
		return false;
	}

	Close();
	mux.lock();
	//�ҵ������� avcodec_find_decoder

	AVCodec* code = avcodec_find_decoder(para->codec_id);
	if(code==NULL){
		avcodec_parameters_free(&para);
		mux.unlock();
		cout << "Can't find codec_id " <<para->codec_id << endl;
		return false;
	}

	//Ϊ�����������ڴ�ռ� avcodec_alloc_context3
	codec = avcodec_alloc_context3(code);

	//���ý����������Ĳ��� avcodec_parameters_to_context(Ȼ������ͷ�para)
	int ret = avcodec_parameters_to_context(codec, para);

	avcodec_parameters_free(&para);
	//���ý������߳���
	codec->thread_count = 8;
	//�򿪽����������� avcodec_open2
	ret = avcodec_open2(codec, 0, 0);
	if (ret != 0) {
		Close();
		mux.unlock();
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "avcodec_open2  failed! :" << buf << endl;
		return false;
	}	
	mux.unlock();
	return true;
}
bool XDecoder::Send(AVPacket* pkt) {
	//�ж�pkt�Ƿ��������
	if (pkt == NULL || pkt->data == NULL || pkt->size <= 0) {
		return false;
	}
	mux.lock();
	//codec�Ƿ��ʼ��
	if (codec == NULL) {
		mux.unlock();
		return false;
	}
	//����pkt��codec�� avcodec_send_packet()
	int ret = avcodec_send_packet(codec,pkt);
	//�ͷ�pkt;
	mux.unlock();
	av_packet_free(&pkt);
	if (ret != 0) {
		return false;
	}
	return true;
}
AVFrame* XDecoder::Recv() {
	mux.lock();
	//�ж�codec�Ƿ��ʼ��
	if (codec == NULL) {
		mux.unlock();
		return NULL;
	}
	//����AVFrame��Ϊ������ڴ�ռ�av_frame_alloc();
	AVFrame* frame = av_frame_alloc();
	//����codec�е�����
	int ret = avcodec_receive_frame(codec, frame);
	mux.unlock();
	if (ret != 0) {
		av_frame_free(&frame);
		return NULL;
	}
	pts = frame->pts;
	return frame;
}
XDecoder::XDecoder() {

}
XDecoder::~XDecoder() {

}