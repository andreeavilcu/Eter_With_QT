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
#include "../models/GameMechanics/Match.h"
#include <QMessageBox>
#include <QInputDialog>
#include "BoardCell.h"
#include "qrandom.h"

class Eter_UI : public QMainWindow {
    Q_OBJECT

public:
    Eter_UI(QWidget* parent = nullptr);
    ~Eter_UI();

    Board* getGameBoard() { return gameBoard; }

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
    QPointer<QPushButton> shiftUpButton;
    QPointer<QPushButton> shiftDownButton;
    QPointer<QPushButton> shiftLeftButton;
    QPointer<QPushButton> shiftRightButton;
    QPushButton* illusionButton;

    QVector<CardLabel*> cards;
    QVector<QLabel*> boardCells;
   
    QGridLayout* boardLayout;
    QLabel* powerCardLabel;
    QLabel* turnLabel;
    QLabel* scoreLabel;

    Board* gameBoard;
    Match* m_match = nullptr;
    std::unique_ptr<Game> m_game;


    bool isStartPage;
    bool isRedTurn;
    bool firstCardPlaced = false;
    int m_redScore;
    int m_blueScore;
    bool redIllusionPlayed = false;
    bool blueIllusionPlayed = false;

    void createShiftButtons();
    void onShiftUp();
    void onShiftDown();
    void onShiftLeft();
    void onShiftRight();
    void updateShiftButtons();
    void updateBoardDisplay();
    void createGame(Game::GameType gameType);       
    void processGameTurn(CardLabel* selectedCard, BoardCell* targetCell); 
    void endGame(const GameEndInfo& info);           
    bool isValidMove(size_t row, size_t col, Card::Value cardValue);
    void createButton(QPointer<QPushButton>& button, const QString& text, int x, int y, int width, int height, const QFont& font, void (Eter_UI::* slot)());
    void initializeButtons();
    void initializePowerCardArea();
    Card::Value charToCardValue(char value);
    void createBoard(QPushButton* clickedButton);
    void createCards(QPushButton* clickedButton);
    void updateTurnLabel();
    void checkWinCondition();
    void showWinMessage(Card::Color winner);
    void processCardPlacement(Player& player, int row, int col, Card::Value cardValue);
    void updateCardStacks();
    void cleanCardStack();
    void displayPowerCard(const QString& powerName); 
    void updateBoardFrom();             
    void initializeMatch(Game::GameType gameType); 
    void startNewTurn();
    void updateScoreLabel(); 
private slots:
    void drawTournamentMenu();
    void drawSpeedMenu();

    void onIllusionButtonClicked();

    void OnButtonClick();
    void onWizardPowersClicked();
    void activateWizardPower(size_t powerIndex, Player& player, Game& game);

    void removeCard(CardLabel* card);
    void onCardPlaced(QDropEvent* event, BoardCell* cell); 
};
