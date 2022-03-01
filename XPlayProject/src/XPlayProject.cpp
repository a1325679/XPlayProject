#include "XPlayProject.h"
#include <QFileDialog>
#include <QDebug>
#include "XDemuxThread.h"
#include <QMessageBox>
#include<iostream>
static XDemuxThread dt;
XPlayProject::XPlayProject(QWidget *parent)
    : QWidget(parent)
{
    dt.Start();
    ui.setupUi(this);
	startTimer(40);
}
XPlayProject::~XPlayProject()
{
    dt.Close();
}
void XPlayProject::SetPause(bool isPause)
{
	if (isPause)
		ui.playOrpause->setText(QString::fromLocal8Bit("�� ��"));
	else
		ui.playOrpause->setText(QString::fromLocal8Bit("�� ͣ"));
}
void XPlayProject::PlayOrPause()
{
	bool isPause = !dt.isPause;
	SetPause(isPause);
	dt.setPause(isPause);

}
void XPlayProject::OpenFile()
{
	//ѡ���ļ�
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����Ƶ�ļ�"));
	if (name.isEmpty())return;
	this->setWindowTitle(name);
	if (!dt.Open(name.toLocal8Bit(), ui.video))
	{
		QMessageBox::information(0, "error", "open file failed!");
		return;
	}
}
//��ʱ�� ��������ʾ
void XPlayProject::timerEvent(QTimerEvent* e)
{
	if (isSliderPress)return;
	long long total = dt.totalMs;
	if (total > 0)
	{
		double pos = (double)dt.pts / (double)total;
		int v = ui.playPos->maximum() * pos;
		ui.playPos->setValue(v);
	}
}
//���ڳߴ�仯
void XPlayProject::resizeEvent(QResizeEvent* e)
{
	ui.playPos->move(50, this->height() - 100);
	ui.playPos->resize(this->width() - 100, ui.playPos->height());
	ui.openFile->move(100, this->height() - 150);
	ui.playOrpause->move(ui.openFile->x() + ui.openFile->width() + 10, ui.openFile->y());
	ui.video->resize(this->size());
}
void XPlayProject::SliderPress()
{
	isSliderPress = true;
}
void XPlayProject::SliderRelease()
{
	isSliderPress = false;
	double pos = 0.0;
	pos = (double)ui.playPos->value() / (double)ui.playPos->maximum();
	dt.Seek(pos);
}