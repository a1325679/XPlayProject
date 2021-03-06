#pragma once
#include <QThread>
#include <mutex>
class XDemux;
class XVideoThread;
class XVideoWidget;
class XAudioThread;
class XDemuxThread:public QThread
{
public:
	bool Open(const char* url, XVideoWidget* call);

	void Seek(double pos);
	void Start();
	void Close();
	virtual void Clear();
	void run();

	void setPause(bool isPause);
	XDemuxThread();
	~XDemuxThread();

	bool isExit = false;

	long long pts = 0;
	long long totalMs = 0;

	std::mutex mux;
	XDemux* demux = NULL;
	XVideoThread* vt = NULL;
	XAudioThread* at = NULL;
	bool isPause = false;
};

