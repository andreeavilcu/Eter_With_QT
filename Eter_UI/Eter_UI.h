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
    QVector<CardLabel*> cards;
    QLabel* powerCardLabel; // Eticheta pentru afisarea cartii puterii
    bool isStartPage;
    bool isRedTurn;

    QLabel* turnLabel;
    Board* gameBoard;
    QGridLayout* boardLayout;
    QVector<QLabel*> boardCells;

    std::unique_ptr<Game> m_game; // Instanță a clasei Game
    std::unique_ptr<Match> m_match; // Instanță a clasei Match

    void createGame(Game::GameType gameType);         // Creează o nouă instanță Game
    void processGameTurn(CardLabel* selectedCard, BoardCell* targetCell); // Procesează mutarea
    void endGame(const GameEndInfo& info);            // Termină jocul și afișează mesajul

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

    void displayPowerCard(const QString& powerName); // Afișează cartea puterii

    // Funcții noi
    void updateBoardFromMatch();               // Actualizează tabla conform stării din Match
    void initializeMatch(Game::GameType gameType); // Inițializează o instanță Match
    void createWizards();
private slots:
    void OnButtonClick();
    void onWizardPowersClicked();
    void drawTournamentMenu();
    void drawSpeedMenu();
    void removeCard(CardLabel* card);

    void onCardPlaced(QDropEvent* event, BoardCell* cell); // Slot pentru plasarea cărților
    void activateWizardPower(size_t powerIndex, Player& player, Game& game); // Activează o putere
};
