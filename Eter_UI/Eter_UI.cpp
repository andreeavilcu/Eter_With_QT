#include "Eter_UI.h"
#include <QPainter>
#include <QLinearGradient>
#include <QScreen>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QGridLayout>
#include "BoardCell.h"
#include "CardLabel.h"


void Eter_UI::createButton(QPointer<QPushButton>& button,
    const QString& text,
    int x,
    int y,
    int width,
    int height,
    const QFont& font,
    void (Eter_UI::* slot)())
{
    button = new QPushButton(text, this);
    button->setFont(font);
    button->setGeometry(x, y, width, height);
    button->show();
    if (slot) {
        connect(button, &QPushButton::clicked, this, slot);
    }
}

void Eter_UI::initializeButtons() {
    QFont buttonFont;
    buttonFont.setPointSize(16);
    buttonFont.setBold(true);

    const int buttonWidth = 230;
    const int buttonHeight = 90;
    const int spacing = 10;
    const int yOffset = 60;

    int centerX = (this->width() - buttonWidth) / 2;
    int centerY = (this->height() - buttonHeight) / 2;

    createButton(buttonTraining, "Training mode",
        centerX, centerY - yOffset,
        buttonWidth, buttonHeight, buttonFont,
        &Eter_UI::OnButtonClick);

    createButton(buttonWizard, "Wizards duel",
        centerX, centerY + spacing + buttonHeight - yOffset,
        buttonWidth, buttonHeight, buttonFont,
        &Eter_UI::OnButtonClick);

    createButton(buttonPowers, "Powers duel",
        centerX, centerY + (spacing + buttonHeight) * 2 - yOffset,
        buttonWidth, buttonHeight, buttonFont,
        &Eter_UI::OnButtonClick);

    createButton(buttonTournament, "Tournament game",
        centerX, centerY + (spacing + buttonHeight) * 3 - yOffset,
        buttonWidth, buttonHeight, buttonFont,
        &Eter_UI::drawTournamentMenu);

    createButton(buttonSpeed, "Speed game",
        centerX, centerY + (spacing + buttonHeight) * 4 - yOffset,
        buttonWidth, buttonHeight, buttonFont,
        &Eter_UI::drawSpeedMenu);

          
}

Eter_UI::Eter_UI(QWidget* parent)
    : QMainWindow(parent),
    isStartPage(true),
    isRedTurn(false),
    m_game(nullptr),
    gameBoard(nullptr),
    boardLayout(nullptr)
{
    ui.setupUi(this);

    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        this->setGeometry(screenGeometry);
    }

    initializeButtons();

    turnLabel = new QLabel(this);
    turnLabel->setFont(QFont("Arial", 14, QFont::Bold));
    turnLabel->setStyleSheet("color: white; background-color: rgba(0, 0, 0, 128); padding: 5px;");
    turnLabel->setAlignment(Qt::AlignCenter);
    turnLabel->hide();
}

Eter_UI::~Eter_UI() {
    if (gameBoard) {
        delete gameBoard;
        gameBoard = nullptr;
    }
}

void Eter_UI::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);

    QLinearGradient gradient(0, height(), width(), 0);
    gradient.setColorAt(0.0, QColor(128, 0, 0));
    gradient.setColorAt(0.5, QColor(64, 0, 64));
    gradient.setColorAt(1.0, QColor(0, 0, 64));

    painter.fillRect(rect(), gradient);

    // Attempt to load a logo
    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.cdUp();  // Adjust if your actual path differs
    QString logoPath = appDir.absoluteFilePath("logo.png");

    QPixmap logo(logoPath);
    if (!logo.isNull()) {
        QRect logoRect;
        if (isStartPage) {
            // Large centered
            logoRect = QRect(width() / 2 - logo.width() / 2,
                height() / 4 - 150,
                logo.width(),
                logo.height());
        }
        else {
            // Smaller corner
            logoRect = QRect(10, 10, 150, 75);
        }
        painter.drawPixmap(logoRect, logo);
    }
    else {
        qDebug() << "Error: Failed to load image:" << logoPath;
    }
}

void Eter_UI::createBoard(QPushButton* clickedButton) {
    // Cleanup old board if any
    if (gameBoard) {
        delete gameBoard;
        gameBoard = nullptr;
    }

    int boardSize = 3;
    if (clickedButton == buttonWizard || clickedButton == buttonPowers) {
        boardSize = 4;
    }

    gameBoard = new Board(boardSize);

    QWidget* boardWidget = new QWidget(this);

    if (boardLayout) {
        delete boardLayout;
        boardLayout = nullptr;
    }
    boardLayout = new QGridLayout(boardWidget);
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0, 0, 0, 0);

    const int cellSize = 100;

    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            BoardCell* cell = new BoardCell(this);
            cell->setAcceptDrops(true);
            cell->setFixedSize(cellSize, cellSize);
            cell->setGridPosition(i, j);

            connect(cell, &BoardCell::cardPlaced,
                this, &Eter_UI::onCardPlaced);

            boardLayout->addWidget(cell, i, j);
            boardCells.append(cell);
        }
    }

    int widgetSize = boardSize * cellSize;
    boardWidget->setLayout(boardLayout);
    boardWidget->setGeometry(width() / 2 - widgetSize / 2,
        height() / 2 - widgetSize / 2,
        widgetSize,
        widgetSize);
    boardWidget->show();
}

void Eter_UI::createCards(QPushButton* clickedButton) {
    QString cardsPath = QCoreApplication::applicationDirPath() + "/cards/";
    if (!QDir(cardsPath).exists()) {
        qDebug() << "Error: 'cards' folder does not exist at:" << cardsPath;
        return;
    }

    QStringList blueCards, redCards;

    if (clickedButton == buttonTraining) {
        blueCards = { "B1","B1","B2","B2","B3","B3","B4" };
        redCards = { "R1","R1","R2","R2","R3","R3","R4" };
    }
    else if (clickedButton == buttonWizard) {
        blueCards = { "B1","B1","B2","B2","B2","B3","B3","B3","B4","BE" };
        redCards = { "R1","R1","R2","R2","R2","R3","R3","R3","R4","RE" };
    }
    else if (clickedButton == buttonPowers || clickedButton == buttonWizardPowers) {
        blueCards = { "B1","B2","B2","B2","B3","B3","B3","B4","BE" };
        redCards = { "R1","R2","R2","R2","R3","R3","R3","R4","RE" };
    }
    else {
        return;
    }

    auto createCard = [&](const QString& cardName, int& startX, int startY) {
        QString imagePath = cardsPath + cardName + ".png";
        if (!QFile::exists(imagePath)) {
            qDebug() << "Error: Image not found:" << imagePath;
            return;
        }

        CardLabel* card = new CardLabel(imagePath,
            charToCardValue(cardName[cardName.size() - 1].toLatin1()),
            this);

        card->setGeometry(startX, startY, 100, 150);

        cards.append(card);
        card->show();
        startX += 110; 
        };

    int startXBlue = width() / 2 - ((blueCards.size() / 2) * 110);
    int startYBlue = height() / 2 + 230;
    for (const QString& cardName : blueCards) {
        createCard(cardName, startXBlue, startYBlue);
    }

    int startXRed = width() / 2 - ((redCards.size() / 2) * 110);
    int startYRed = height() / 2 - 340;
    for (const QString& cardName : redCards) {
        createCard(cardName, startXRed, startYRed);
    }
    updateCardStacks();
    //for (CardLabel* card : rosu) {  // Exemplu pentru cărțile roșii
    //    cards.append(card);
    //}
    //for (CardLabel* card : albastru) {  // Exemplu pentru cărțile albastre
    //    cards.append(card);
    //}

}

Card::Value Eter_UI::charToCardValue(char value) {
    switch (value) {
    case '1': return Card::Value::One;
    case '2': return Card::Value::Two;
    case '3': return Card::Value::Three;
    case '4': return Card::Value::Four;
    case 'E': return Card::Value::Eter;
    ///default:  return Card::Value::Undefined;
    }
}

void Eter_UI::createGame(Game::GameType gameType) {
    if (m_game) {
    }

    std::pair<size_t, size_t> wizardIndices = { 0, 1 };
    bool illusionsAllowed = false;
    bool explosionAllowed = false;
    bool tournament = false;
   
    m_game = std::make_unique<Game>(gameType, wizardIndices, illusionsAllowed, explosionAllowed, tournament);

}

void Eter_UI::OnButtonClick() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    isStartPage = false;
    for (QObject* child : children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this && widget != turnLabel) {
                widget->hide();
                widget->deleteLater();
            }
        }
    }
    Game::GameType gameType;
    if (clickedButton == buttonTraining) {
        gameType = Game::GameType::Training;
    }
    else if (clickedButton == buttonWizard) {
        gameType = Game::GameType::WizardDuel;
    }
    else if (clickedButton == buttonPowers) {
        gameType = Game::GameType::PowerDuel;
    }
    else if (clickedButton == buttonWizardPowers) {
        gameType = Game::GameType::WizardAndPowerDuel;
    }

    // Inițializează Match
    Match::MatchType matchType = Match::MatchType::Normal;
    Match::TimerDuration timerDuration = Match::TimerDuration::Untimed;

    m_match = std::make_unique<Match>(matchType, timerDuration, gameType, false, false);

    // Curăță și inițializează tabla
    createBoard(clickedButton);
    createCards(clickedButton);

    turnLabel->setGeometry(width() / 2 - 100, 50, 200, 40);
    turnLabel->show();
    updateTurnLabel();
    update();
}


void Eter_UI::updateTurnLabel() {
    if (!turnLabel) return;

    if (isRedTurn) {
        turnLabel->setText("Rândul jucătorului roșu");
        turnLabel->setStyleSheet("color: white; background-color: rgba(255, 0, 0, 128); padding: 5px;");
    }
    else {
        turnLabel->setText("Rândul jucătorului albastru");
        turnLabel->setStyleSheet("color: white; background-color: rgba(0, 0, 255, 128); padding: 5px;");
    }

    turnLabel->adjustSize();
    turnLabel->move(width() / 2 - turnLabel->width() / 2, 20);
}

void Eter_UI::onWizardPowersClicked() {
    OnButtonClick();
}

void Eter_UI::checkWinCondition() {
    if (gameBoard) {
        Card::Color winner = gameBoard->checkWin();
        if (winner != Card::Color::Undefined) {
            showWinMessage(winner);
        }
    }
}

void Eter_UI::removeCard(CardLabel* card) {
    cards.removeOne(card);
    card->deleteLater();

    isRedTurn = !isRedTurn;
    updateTurnLabel();

    checkWinCondition();
}

void Eter_UI::showWinMessage(Card::Color winner) {
    QString winnerText = (winner == Card::Color::Red) ? "Red Player" : "Blue Player";
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Game Over");
    msgBox.setText(QString("%1 has won the game!").arg(winnerText));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void Eter_UI::processCardPlacement(Player& player, int row, int col, Card::Value cardValue)
{
    // Verificăm dacă mutarea este validă
    if (!m_game->getBoard().checkPartial(row, col, static_cast<size_t>(cardValue))) {
        return;
    }

    auto playedCard = player.useCard(cardValue);
    if (!playedCard) {
        return;
    }

    auto& board = m_game->getBoard();
    if (!board.checkIllusion(row, col, Card::Color::Undefined) &&
        board.checkIllusionValue(row, col, static_cast<size_t>(cardValue))) {
        board.placeCard(row, col, std::move(*playedCard));
        player.setLastPlacedCard(board.getBoard()[row][col].back());
    }
    else {
        board.getBoard()[row][col].back().resetIllusion();
        m_game->m_eliminatedCards.push_back(std::move(*playedCard));
    }

    board.setFirstCardPlayed();
}

void Eter_UI::drawTournamentMenu() {
    isStartPage = false;

    for (QObject* child : children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this && widget != turnLabel) {
                widget->hide();
                widget->deleteLater();
            }
        }
    }

    QFont buttonFont;
    buttonFont.setPointSize(16);
    buttonFont.setBold(true);

    const int buttonWidth = 200;
    const int buttonHeight = 100;
    const int spacing = 10;

    int centerX = (this->width() - buttonWidth) / 2;
    int centerY = (this->height() - buttonHeight * 4 - spacing * 3) / 2;

    createButton(buttonTraining, "Training Mode",
        centerX, centerY,
        buttonWidth, buttonHeight,
        buttonFont, &Eter_UI::OnButtonClick);

    createButton(buttonWizard, "Wizard Mode",
        centerX, centerY + buttonHeight + spacing,
        buttonWidth, buttonHeight,
        buttonFont, &Eter_UI::OnButtonClick);

    createButton(buttonPowers, "Powers Mode",
        centerX, centerY + (buttonHeight + spacing) * 2,
        buttonWidth, buttonHeight,
        buttonFont, &Eter_UI::OnButtonClick);

    createButton(buttonWizardPowers, "Wizard and Powers",
        centerX, centerY + (buttonHeight + spacing) * 3,
        buttonWidth, buttonHeight,
        buttonFont, &Eter_UI::onWizardPowersClicked);
}

void Eter_UI::drawSpeedMenu() {
    isStartPage = false;

    for (QObject* child : children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this && widget != turnLabel) {
                widget->hide();
                widget->deleteLater();
            }
        }
    }

    QFont buttonFont;
    buttonFont.setPointSize(16);
    buttonFont.setBold(true);

    const int buttonWidth = 200;
    const int buttonHeight = 100;
    const int spacing = 10;

    int centerX = (this->width() - buttonWidth) / 2;
    int centerY = (this->height() - buttonHeight * 5 - spacing * 4) / 2;

    createButton(buttonTraining, "Training Mode",
        centerX, centerY,
        buttonWidth, buttonHeight,
        buttonFont, &Eter_UI::OnButtonClick);

    createButton(buttonWizard, "Wizard Mode",
        centerX, centerY + buttonHeight + spacing,
        buttonWidth, buttonHeight,
        buttonFont, &Eter_UI::OnButtonClick);

    createButton(buttonPowers, "Powers Mode",
        centerX, centerY + (buttonHeight + spacing) * 2,
        buttonWidth, buttonHeight,
        buttonFont, &Eter_UI::OnButtonClick);

    createButton(buttonWizardPowers, "Wizard and Powers",
        centerX, centerY + (buttonHeight + spacing) * 3,
        buttonWidth, buttonHeight,
        buttonFont, &Eter_UI::onWizardPowersClicked);

}
void Eter_UI::updateCardStacks() {
    for (CardLabel* card : cards) {
        if (!card) {
            qDebug() << "Invalid card pointer (nullptr) detected!";
            continue;
        }

        if (card->isHidden()) {
            qDebug() << "Card is hidden, skipping:" << card;
            continue;
        }

        QString cardName = card->property("cardName").toString();
        bool isRedCard = cardName.contains("R", Qt::CaseInsensitive);
        bool isBlueCard = cardName.contains("B", Qt::CaseInsensitive);

        if (isRedTurn) {
            card->setEnabled(isRedCard); 
            qDebug() << "Enabling red card:" << cardName;
        }
        else {
            card->setEnabled(isBlueCard);  
            qDebug() << "Enabling blue card:" << cardName;
        }
    }
}

void Eter_UI::cleanCardStack() {
    cards.erase(
        std::remove_if(cards.begin(), cards.end(),
            [](CardLabel* card) {
                return !card || card->isHidden();
            }),
        cards.end());
}


void Eter_UI::onCardPlaced(QDropEvent* event, BoardCell* cell) {
    CardLabel* card = qobject_cast<CardLabel*>(event->source());
    if (!card || !m_match) {
        return;
    }

    QString cardName = card->property("cardName").toString();
    bool isRedCard = cardName.contains("R", Qt::CaseInsensitive);
    bool isBlueCard = cardName.contains("B", Qt::CaseInsensitive);

    if ((isRedTurn && !isRedCard) || (!isRedTurn && !isBlueCard)) {
        QMessageBox::warning(this, "Mutare interzisă", "Este rândul celuilalt jucător!");
        return;
    }

    int row = cell->getRow();
    int col = cell->getCol();

    qDebug() << "Attempting to place card at:" << row << "," << col;
    qDebug() << "First card played:" << gameBoard->m_firstCardPlayed;
    qDebug() << "checkNeighbours result:" << gameBoard->checkNeighbours(row, col);

    if (gameBoard->m_firstCardPlayed && !gameBoard->checkNeighbours(row, col)) {
        qDebug() << "Placement rejected - not adjacent";
        QMessageBox::warning(this, "Mutare interzisă", "Cartea trebuie plasată adiacent unei cărți existente!");
        //cell->clear();
        return;
    }

    Card::Value cardValue = static_cast<Card::Value>(card->property("cardValue").toInt());

    // Update internal board state first
    Card newCard(cardValue, isRedTurn ? Card::Color::Red : Card::Color::Blue);
    gameBoard->m_board[row][col].push_back(newCard);

    cell->setPixmap(card->pixmap(Qt::ReturnByValue).scaled(100, 150, Qt::KeepAspectRatio));

    // Updatează logica Match
    GameEndInfo gameInfo;
    gameInfo.x = row; 
    gameInfo.y = col; 
    gameInfo.winner = isRedTurn ? Card::Color::Red : Card::Color::Blue;

    m_match->runArenaLogic(gameInfo);

    if (!gameBoard->m_firstCardPlayed) {
        gameBoard->m_firstCardPlayed = true;
    }

    card->hide();
    isRedTurn = !isRedTurn;
    updateCardStacks();
    updateTurnLabel();

    if (m_match && m_match->checkArenaWin()) { 
        showWinMessage(isRedTurn ? Card::Color::Blue : Card::Color::Red);
    }
}

void Eter_UI::processGameTurn(CardLabel* selectedCard, BoardCell* targetCell) {
    if (!m_game) {
        qWarning() << "No valid Game object to process turn.";
        return;
    }

    targetCell->setPixmap(selectedCard->pixmap(Qt::ReturnByValue));

    auto info = m_game->run(isRedTurn, /*timed=*/false, /*duration=*/0);
    removeCard(selectedCard);

    if (info.winner != Card::Color::Undefined) {
        endGame(info);
    }
}
void Eter_UI::endGame(const GameEndInfo& info) {
    showWinMessage(info.winner);
}
void Eter_UI::updateBoardFromMatch() {
    if (!m_match) return;

    const auto& arena = m_match->getArena();
    for (size_t i = 0; i < arena.size(); ++i) {
        for (size_t j = 0; j < arena[i].size(); ++j) {
            BoardCell* cell = qobject_cast<BoardCell*>(boardCells.at(i * arena.size() + j));

            const auto& pieces = arena[i][j];

            if (!pieces.empty()) {
                const auto& lastPiece = pieces.back();
                QString color = (lastPiece.getColor() == Card::Color::Red) ? "R" : "B";
                QString imagePath = QString(":/images/%1.png").arg(color);
                QPixmap pixmap(imagePath);
                cell->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
            }
            else {
                cell->clear();
            }
        }
    }
}
