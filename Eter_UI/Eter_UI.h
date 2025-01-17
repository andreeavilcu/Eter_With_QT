#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Eter_UI.h"
#include "qpushbutton.h"
#include "qpointer.h"
#include <QLabel>
#include <QPixmap>
#include <qdir.h>
#include <QLayout>
#include "../models/GameMechanics/Board.h"

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
    QPointer<QPushButton> buttonTournament;
    QPointer<QPushButton> buttonSpeed;
    bool isStartPage;

    Board* gameBoard;  // Obiect pentru logica tablei
    QGridLayout* boardLayout; // Layout pentru tabla
    QVector<QLabel*> boardCells; // Celulele tablei

    void createBoard();// Funcție pentru inițializarea tablei
    void createCards(QPushButton* clickedButton);
    ///slot= event handler;
    ///teoretic iti spune ce se intampla cand faci o actieune pe un obiect (de ex apesi un buton)
private slots: 
    void OnButtonClick();
};
