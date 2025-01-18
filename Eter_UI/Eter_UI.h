#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Eter_UI.h"
#include <QPushButton>
#include <QPointer>
#include <QLabel>
#include <QPixmap>
#include <QDir>
#include <QLayout>
#include <QVector>
#include "../models/GameMechanics/Board.h"
#include "CardLabel.h"
#include "../models/GameMechanics/Game.h"
#include <QMessageBox>

class Eter_UI : public QMainWindow
{
    Q_OBJECT

public:
    Eter_UI(QWidget* parent = nullptr);
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

    void createButton(QPointer<QPushButton>& button, const QString& text, int x, int y, int width, int height, const QFont& font, void (Eter_UI::* slot)());
    void initializeButtons();
    Card::Value charToCardValue(char value);
    void createBoard(QPushButton* clickedButton);
    void createCards(QPushButton* clickedButton);
    void updateTurnLabel();
    void checkWinCondition();
    void showWinMessage(Card::Color winner);

private slots:
    void OnButtonClick();
    void onWizardPowersClicked();
    void drawTournamentMenu();
    void drawSpeedMenu();
    void removeCard(CardLabel* card);
};
