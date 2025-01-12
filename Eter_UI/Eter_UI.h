#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Eter_UI.h"

class Eter_UI : public QMainWindow
{
    Q_OBJECT

public:
    Eter_UI(QWidget *parent = nullptr);
    ~Eter_UI();

private:
    Ui::Eter_UIClass ui;
};
