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
    boardLayout(nullptr),
    powerCardLabel(nullptr) // Inițializarea QLabel pentru puteri
{
    // Configurează interfața UI
    ui.setupUi(this);

    // Setează dimensiunea ferestrei la dimensiunea ecranului principal
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        this->setGeometry(screenGeometry);
    }

    // Inițializează butoanele jocului
    initializeButtons();

    // Creează și configurează QLabel pentru afișarea puterii
    powerCardLabel = new QLabel(this);
    powerCardLabel->setFixedSize(100, 150);
    powerCardLabel->setGeometry(width() - 150, height() / 2 - 75, 100, 150);
    powerCardLabel->setStyleSheet("border: 2px solid black; background-color: white;");
    powerCardLabel->hide(); // Ascunde până la activarea unei puteri

    // Inițializează QLabel pentru indicarea rândului
    turnLabel = new QLabel(this);
    turnLabel->setFont(QFont("Arial", 14, QFont::Bold));
    turnLabel->setStyleSheet("color: white; background-color: rgba(0, 0, 0, 128); padding: 5px;");
    turnLabel->setAlignment(Qt::AlignCenter);
    turnLabel->hide(); // Ascunde până când începe jocul
}

Eter_UI::~Eter_UI() {
    // Curăță resursele alocate
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
    if (clickedButton == buttonWizard || clickedButton == buttonPowers || clickedButton == buttonWizardPowers) {
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
    QStringList wizardNames = {
        "eliminateCard",
        "eliminateRow",
        "coverCard",
        "sinkHole",
        "moveStackOwn",
        "extraEter",
        "moveStackOpponent",
        "moveEdge"
    };

    bool includeWizards = true;

    if (clickedButton == buttonTraining) {
        blueCards = { "B1","B1","B2","B2","B3","B3","B4" };
        redCards = { "R1","R1","R2","R2","R3","R3","R4" };
        includeWizards = false; // Nu adăugăm vrăjitori în Training Mode
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

    // Selectăm vrăjitori
    int blueWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
    int redWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
    QString blueWizard = wizardNames[blueWizardIndex];
    QString redWizard = wizardNames[redWizardIndex];

    // Lambda pentru a crea o carte
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

    // Lambda pentru a adăuga vrăjitori
    auto addWizardCard = [&](const QString& wizardName, int startX, int startY) {
        if (!includeWizards) return;

        QString wizardImagePath = cardsPath + wizardName + ".png";
        if (QFile::exists(wizardImagePath)) {
            QLabel* wizardLabel = new QLabel(this);
            QPixmap pixmap(wizardImagePath);
            wizardLabel->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
            wizardLabel->setGeometry(startX, startY, 100, 150);
            wizardLabel->show();
        }
        else {
            qDebug() << "Error: Wizard image not found: " << wizardImagePath;
        }
        };

    // Creăm cărți albastre și poziționăm vrăjitorul înainte
    int startXBlue = width() / 2 - ((blueCards.size() / 2) * 110);
    int startYBlue = height() / 2 + 230;
    addWizardCard(blueWizard, startXBlue - 120, startYBlue); // Poziționăm vrăjitorul la începutul liniei albastre
    for (const QString& cardName : blueCards) {
        createCard(cardName, startXBlue, startYBlue);
    }

    // Creăm cărți roșii și poziționăm vrăjitorul înainte
    int startXRed = width() / 2 - ((redCards.size() / 2) * 110);
    int startYRed = height() / 2 - 340;
    addWizardCard(redWizard, startXRed - 120, startYRed); // Poziționăm vrăjitorul la începutul liniei roșii
    for (const QString& cardName : redCards) {
        createCard(cardName, startXRed, startYRed);
    }

    updateCardStacks();
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

    // Ascunde toate componentele existente
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
    bool includeWizards = true; // Flag pentru a decide dacă se adaugă vrăjitori

    // Determină tipul de joc
    if (clickedButton == buttonTraining) {
        gameType = Game::GameType::Training;
        includeWizards = false; // Fără vrăjitori pentru modul Training
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
    else {
        return;
    }

    // Inițializează Match
    Match::MatchType matchType = Match::MatchType::Normal;
    Match::TimerDuration timerDuration = Match::TimerDuration::Untimed;

    m_match = std::make_unique<Match>(matchType, timerDuration, gameType, false, false);

    // Curăță și inițializează tabla
    createBoard(clickedButton);
    createCards(clickedButton);

    // Actualizează eticheta pentru rânduri
    turnLabel->setGeometry(width() / 2 - 100, 50, 200, 40);
    turnLabel->show();
    updateTurnLabel();

    QString cardsPath = QCoreApplication::applicationDirPath() + "/cards/";

    // Adaugă vrăjitori dacă nu suntem în modul Training
    if (includeWizards && (clickedButton == buttonWizard || clickedButton == buttonWizardPowers)) {
        QStringList wizardNames = {
            "eliminateCard",
            "eliminateRow",
            "coverCard",
            "sinkHole",
            "moveStackOwn",
            "extraEter",
            "moveStackOpponent",
            "moveEdge"
        };

        int blueWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
        int redWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());

        QString blueWizard = wizardNames[blueWizardIndex];
        QString redWizard = wizardNames[redWizardIndex];

        auto addWizardCard = [&](const QString& wizardName, int startX, int startY) {
            QString wizardImagePath = cardsPath + wizardName + ".png";
            if (QFile::exists(wizardImagePath)) {
                QLabel* wizardLabel = new QLabel(this);
                QPixmap pixmap(wizardImagePath);
                wizardLabel->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
                wizardLabel->setGeometry(startX, startY, 100, 150);
                wizardLabel->show();
            }
            else {
                qDebug() << "Error: Wizard image not found: " << wizardImagePath;
            }
            };

        int startXBlue = width() / 2 + ((10 / 2) * 110); // În dreapta cărților albastre
        int startYBlue = height() / 2 + 230;
        addWizardCard(blueWizard, startXBlue, startYBlue);

        int startXRed = width() / 2 + ((10 / 2) * 110); // În dreapta cărților roșii
        int startYRed = height() / 2 - 340;
        addWizardCard(redWizard, startXRed, startYRed);
    }

    // Adaugă puteri dacă modul necesită acest lucru
    if (clickedButton == buttonPowers || clickedButton == buttonWizardPowers) {
        QStringList powerNames = {
            "controlledExplosion",
            "destruction",
            "flame",
            "lava",
            "ash",
            "spark",
            "squall",
            "gale",
            "hurricane",
            "gust",
            "mirage",
            "storm",
            "tide",
            "mist",
            "wave",
            "whirlpool",
            "tsunami",
            "waterfall",
            "support",
            "earthquake",
            "crumble",
            "border",
            "avalanche",
            "rock"
        };

        auto addPowerCard = [&](const QString& powerName, int startX, int startY) {
            QString powerImagePath = cardsPath + powerName + ".png";
            if (QFile::exists(powerImagePath)) {
                QLabel* powerLabel = new QLabel(this);
                QPixmap pixmap(powerImagePath);
                powerLabel->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
                powerLabel->setGeometry(startX, startY, 100, 150);
                powerLabel->show();
            }
            else {
                qDebug() << "Error: Power image not found: " << powerImagePath;
            }
            };

        int bluePower1Index = QRandomGenerator::global()->bounded(powerNames.size());
        int bluePower2Index = QRandomGenerator::global()->bounded(powerNames.size());
        while (bluePower2Index == bluePower1Index) {
            bluePower2Index = QRandomGenerator::global()->bounded(powerNames.size());
        }

        int redPower1Index = QRandomGenerator::global()->bounded(powerNames.size());
        int redPower2Index = QRandomGenerator::global()->bounded(powerNames.size());
        while (redPower2Index == redPower1Index) {
            redPower2Index = QRandomGenerator::global()->bounded(powerNames.size());
        }

        QString bluePower1 = powerNames[bluePower1Index];
        QString bluePower2 = powerNames[bluePower2Index];
        QString redPower1 = powerNames[redPower1Index];
        QString redPower2 = powerNames[redPower2Index];

        int startXBlue = width() / 2 + ((10 / 2) * 110); // În dreapta cărților albastre
        int startYBlue = height() / 2 + 230;
        addPowerCard(bluePower1, startXBlue, startYBlue);
        addPowerCard(bluePower2, startXBlue + 110, startYBlue);

        int startXRed = width() / 2 + ((10 / 2) * 110); // În dreapta cărților roșii
        int startYRed = height() / 2 - 340;
        addPowerCard(redPower1, startXRed, startYRed);
        addPowerCard(redPower2, startXRed + 110, startYRed);
    }

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

        // Disable special cards if it's not the player's turn
        bool isSpecialCard = cardName.contains("E") || cardName.contains("P");

        if (isRedTurn) {
            card->setEnabled(isRedCard || (isSpecialCard && isRedCard));
            qDebug() << "Enabling red card or special:" << cardName;
        }
        else {
            card->setEnabled(isBlueCard || (isSpecialCard && isBlueCard));
            qDebug() << "Enabling blue card or special:" << cardName;
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
void Eter_UI::displayPowerCard(const QString& powerName) {
    QString imagePath = QCoreApplication::applicationDirPath() + "/cards/" + powerName + ".png";
    if (QFile::exists(imagePath)) {
        QPixmap pixmap(imagePath);
        powerCardLabel->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
        powerCardLabel->show();
    }
    else {
        qDebug() << "Eroare: Imaginea pentru puterea" << powerName << "nu a fost găsită!";
    }
}
void Eter_UI::activateWizardPower(size_t powerIndex, Player& player, Game& game) {
    Wizard& wizard = Wizard::getInstance();
    if (wizard.play(powerIndex, player, game, /*check=*/false)) {
        QString powerName = QString::fromStdString(wizard.getWizardName(powerIndex));
        displayPowerCard(powerName);
    }
    else {
        QMessageBox::warning(this, "Activare eșuată", "Puterea nu a putut fi activată.");
    }
}
void Eter_UI::createWizards() {
    QString cardsPath = QCoreApplication::applicationDirPath() + "/cards/";

    QStringList wizardNames = {
        "eliminateCard",
        "eliminateRow",
        "coverCard",
        "sinkHole",
        "moveStackOwn",
        "extraEter",
        "moveStackOpponent",
        "moveEdge"
    };

    int startX = width() - 200; // Poziția în partea dreaptă
    int startY = 100;

    for (const QString& wizardName : wizardNames) {
        QString imagePath = cardsPath + wizardName + ".png";

        if (!QFile::exists(imagePath)) {
            qDebug() << "Imaginea vrăjitorului lipsă: " << imagePath;
            continue;
        }

        QLabel* wizardLabel = new QLabel(this);
        QPixmap pixmap(imagePath);
        wizardLabel->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
        wizardLabel->setGeometry(startX, startY, 100, 150);
        wizardLabel->show();

        startY += 160; // Spațiere între imagini
    }
}

