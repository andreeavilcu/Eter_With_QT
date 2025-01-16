#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Eter_UI.h"
#include "qpushbutton.h"
#include "qpointer.h"
#include <QLabel>
#include <QPixmap>
#include <qdir.h>
#include <QLayout>


class Eter_UI : public QMainWindow
{
    Q_OBJECT

public:
    Eter_UI(QWidget *parent = nullptr);
    ~Eter_UI();
protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::Eter_UIClass ui;
    QPointer<QPushButton> buttonTraning;
    QPointer<QPushButton> buttonPowers;
    QPointer<QPushButton> buttonWizard;
    QPointer<QPushButton> buttonTurnament;
    QPointer<QPushButton> buttonTimed;
    bool isStartPage;
    ///slot= event handler;
    ///teoretic iti spune ce se intampla cand faci o actieune pe un obiect (de ex apesi un buton)
private slots: 
    void OnButtonClick();
};
