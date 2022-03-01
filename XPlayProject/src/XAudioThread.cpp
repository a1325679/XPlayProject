#include "XAudioThread.h"
#include "XDecoder.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include <iostream>
using namespace std;


void XAudioThread::Close()
{
	XDecodeThread::Close();
	if (res)
	{
		res->Close();
		amux.lock();
		delete res;
		res = NULL;
		amux.unlock();
	}
	if (ap)
	{
		ap->Close();
		amux.lock();
		ap = NULL;
		amux.unlock();
	}
}


void XAudioThread::setPause(bool isPause) {
	//amux.lock();
	this->isPause = isPause;
	if (ap) {
		ap->setPause(isPause);
	}
	//amux.unlock();
}

bool XAudioThread::Open(AVCodecParameters* para, int sampleRate, int channels) {
	if (!para)return false;
	Clear();
	amux.lock();
	pts = 0;


	bool ret = true;
	if (!res->Open(para))
	{
		cout << "XResample open failed!" << endl;
		ret = false;
	}
	ap->sampleRate = sampleRate;
	ap->channels = channels;
	if (!ap->Open())
	{
		ret = false;
		cout << "XAudioPlay open failed!" << endl;
	}
	if (!decode->Open(para))
	{
		cout << "audio XDecode open failed!" << endl;
		ret = false;
	}
	amux.unlock();
	cout << "XAudioThread::Open :" << ret << endl;
	return ret;

}
void XAudioThread::run()
{
	unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
	while (!isExit)
	{
		amux.lock();

		if (isPause) {
			amux.unlock();
			msleep(5);
			continue;
		}

		//没有数据
		//if (packs.empty() || !decode || !res || !ap)
		//{
		//	amux.unlock();
		//	msleep(1);
		//	continue;
		//}
		AVPacket* pkt = Pop();
		bool re = decode->Send(pkt);
		if (!re)
		{
			amux.unlock();
			msleep(1);
			continue;
		}
		//一次send 多次recv
		while (!isExit)
		{
			AVFrame* frame = decode->Recv();
			if (!frame) break;

			pts = decode->pts - ap->GetNoPlayMs();

			//重采样 
			int size = res->Resample(frame, pcm);
			//播放音频
			while (!isExit)
			{
				if (size <= 0)break;
				//缓冲未播完，空间不够
				if (ap->GetFree() < size || isPause)
				{
					msleep(1);
					continue;
				}
				ap->Write(pcm, size);
				break;
			}
		}
		amux.unlock();
	}
	delete pcm;
}

XAudioThread::XAudioThread()
{
	if (!res) res = new XResample();
	if (!ap) ap = XAudioPlay::Get();
}


XAudioThread::~XAudioThread()
{
	//等待线程退出
	isExit = true;
	wait();
}