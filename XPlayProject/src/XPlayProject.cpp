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
}
XPlayProject::~XPlayProject()
{
    dt.Close();
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