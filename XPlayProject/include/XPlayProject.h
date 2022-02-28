#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlayProject.h"

class XPlayProject : public QWidget
{
    Q_OBJECT

public:
    XPlayProject(QWidget *parent = Q_NULLPTR);

private:
    Ui::XPlayProjectClass ui;
};
