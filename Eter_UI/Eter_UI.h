#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Eter_UI.h"
#include "qpushbutton.h"
#include "qpointer.h"
#include <QLabel>
#include <QPixmap>
#include <qdir.h>
#include <QLayout>
#include<qvector.h>
#include "../models/GameMechanics/Board.h"
#include "CardLabel.h"
#include "../models/GameMechanics/Game.h"
#include <QMessageBox>


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
    QPointer<QPushButton> buttonTraining;
    QPointer<QPushButton> buttonPowers;
    QPointer<QPushButton> buttonWizard;
    QPointer<QPushButton> buttonTournament;
    QPointer<QPushButton> buttonSpeed;
    QPointer<QPushButton> buttonWizardPowers;
    QVector<CardLabel*> cards; 
    bool isStartPage;
    bool isRedTurn;
    Game* m_game;

    QLabel* turnLabel;
    Board* gameBoard;  
    QGridLayout* boardLayout; 
    QVector<QLabel*> boardCells; 

    Card::Value charToCardValue(char value);
    void createBoard();
    void createCards(QPushButton* clickedButton);
    void updateTurnLabel();
    void checkWinCondition();
    void showWinMessage(Card::Color winner);
    
    ///slot= event handler;
    ///teoretic iti spune ce se intampla cand faci o actieune pe un obiect (de ex apesi un buton)
    
private slots: 
    void OnButtonClick();
    void drawTournamentMenu();
    void drawSpeedMenu();
    void removeCard(CardLabel* card); 
    
};
