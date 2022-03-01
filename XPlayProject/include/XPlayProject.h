#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlayProject.h"

class XPlayProject : public QWidget
{
    Q_OBJECT

public:
    XPlayProject(QWidget *parent = Q_NULLPTR);
    ~XPlayProject();
    void SetPause(bool isPause);
    //定时器 滑动条显示
    void timerEvent(QTimerEvent* e);

    //窗口尺寸变化
    void resizeEvent(QResizeEvent* e);
    bool isSliderPress = false;
    Ui::XPlayProjectClass ui;
public slots:
    void OpenFile(); 
    void PlayOrPause();
    void SliderPress();
    void SliderRelease();
};
