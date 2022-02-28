#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlayProject.h"

class XPlayProject : public QWidget
{
    Q_OBJECT

public:
    XPlayProject(QWidget *parent = Q_NULLPTR);
    ~XPlayProject();

    Ui::XPlayProjectClass ui;
public slots:
    void OpenFile();
};
