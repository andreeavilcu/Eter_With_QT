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

void Eter_UI::createButton(
    QPointer<QPushButton>& button,
    const QString& text,
    int x, int y,
    int width, int height,
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
    m_match(nullptr),
    boardLayout(nullptr),
    powerCardLabel(nullptr),
    m_redScore(0),
    m_blueScore(0)
{
    ui.setupUi(this);

    // Setează fereastra la dimensiunea ecranului primar
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        this->setGeometry(screenGeometry);
    }

    // Inițializează butoanele de pe ecranul de start
    initializeButtons();

    // Label pentru (eventuale) puteri afișate
    powerCardLabel = new QLabel(this);
    powerCardLabel->setFixedSize(100, 150);
    powerCardLabel->setGeometry(width() - 150, height() / 2 - 75, 100, 150);
    powerCardLabel->setStyleSheet("border: 2px solid black; background-color: white;");
    powerCardLabel->hide();

    // Label pentru afișarea rândului curent
    turnLabel = new QLabel(this);
    turnLabel->setFont(QFont("Arial", 14, QFont::Bold));
    turnLabel->setStyleSheet("color: white; background-color: rgba(0, 0, 0, 128); padding: 5px;");
    turnLabel->setAlignment(Qt::AlignCenter);
    turnLabel->hide();

    // Label pentru scor (best of 3) - inițial ascuns
    scoreLabel = new QLabel(this);
    scoreLabel->setFont(QFont("Arial", 14, QFont::Bold));
    scoreLabel->setStyleSheet("color: white; background-color: rgba(0,0,0,128); padding: 5px;");
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setText("Score: Blue 0 - 0 Red");
    scoreLabel->adjustSize();
    // Poți ajusta poziția după preferințe; exemplu: colț dreapta-sus:
    scoreLabel->move(width() - scoreLabel->width() - 20, 20);
    scoreLabel->hide();  // Va fi afișat doar dacă se joacă Training
}

Eter_UI::~Eter_UI() {
}
void Eter_UI::OnButtonClick() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    isStartPage = false;
    for (QObject* child : children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this && widget != turnLabel && widget != scoreLabel) {
                widget->hide();
                widget->deleteLater();
            }
        }
    }

    Game::GameType gameType;
    bool includeWizards = false;
    bool includePowers = false;

    if (clickedButton == buttonTraining) {
        gameType = Game::GameType::Training;
    }
    else if (clickedButton == buttonWizard) {
        gameType = Game::GameType::WizardDuel;
        includeWizards = true;
    }
    else if (clickedButton == buttonPowers) {
        gameType = Game::GameType::PowerDuel;
        includePowers = true;
    }
    else if (clickedButton == buttonWizardPowers) {
        gameType = Game::GameType::WizardAndPowerDuel;
        includeWizards = true;
        includePowers = true;
    }
    else {
        return;
    }

    try {
        m_game = std::make_unique<Game>(
            gameType,
            std::make_pair<size_t, size_t>(0, 1),  // exemplu: indices wizards
            false, // illusionsAllowed
            false, // explosionAllowed
            false  // tournament
        );
    }
    catch (const std::exception& e) {
        qDebug() << "Error creating game instance:" << e.what();
        QMessageBox::critical(this, "Error", "Failed to create game instance.");
        return;
    }

    createBoard(clickedButton);
    createCards(clickedButton);
    createShiftButtons();

    // Afișăm turnLabel
    turnLabel->setGeometry(width() / 2 - 100, 50, 200, 40);
    turnLabel->show();
    updateTurnLabel();

    // Afișăm și scoreLabel **doar** dacă suntem în modul Training
    if (m_game && m_game->getGameType() == Game::GameType::Training) {
        scoreLabel->show();
    }

    update();
}
void Eter_UI::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);

    QLinearGradient gradient(0, height(), width(), 0);
    gradient.setColorAt(0.0, QColor(128, 0, 0));
    gradient.setColorAt(0.5, QColor(64, 0, 64));
    gradient.setColorAt(1.0, QColor(0, 0, 64));

    painter.fillRect(rect(), gradient);

    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.cdUp();
    QString logoPath = appDir.absoluteFilePath("logo.png");

    QPixmap logo(logoPath);
    if (!logo.isNull()) {
        QRect logoRect;
        if (isStartPage) {
            logoRect = QRect(width() / 2 - logo.width() / 2,
                height() / 4 - 150,
                logo.width(),
                logo.height());
        }
        else {
            logoRect = QRect(10, 10, 150, 75);
        }
        painter.drawPixmap(logoRect, logo);
    }
    else {
        qDebug() << "Error: Failed to load image:" << logoPath;
    }
}

void Eter_UI::createBoard(QPushButton* clickedButton) {
    if (boardLayout) {
        delete boardLayout;
        boardLayout = nullptr;
    }

    int boardSize = 3; 
    if (clickedButton == buttonWizard ||
        clickedButton == buttonPowers ||
        clickedButton == buttonWizardPowers)
    {
        boardSize = 4;
    }

    QWidget* boardWidget = new QWidget(this);

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

    int boardX = width() / 2 - widgetSize / 2;
    int boardY = height() / 2 - widgetSize / 2;
    boardWidget->setLayout(boardLayout);
    boardWidget->setGeometry(width() / 2 - widgetSize / 2,
        height() / 2 - widgetSize / 2,
        widgetSize,
        widgetSize);
    boardWidget->show();

    illusionButton = new QPushButton("Iluzie", this);
    illusionButton->setFont(QFont("Arial", 12, QFont::Bold));
    illusionButton->setStyleSheet(R"(
        QPushButton {
            background-color: rgba(255, 255, 255, 180);
            border: 2px solid black;
            border-radius: 5px;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 220);
        }
    )");

    // Poziționează butonul în stânga tablei
    int buttonWidth = 100;
    int buttonHeight = 40;
    illusionButton->setGeometry(boardX - buttonWidth - 20, boardY + widgetSize / 2 - buttonHeight / 2,
        buttonWidth, buttonHeight);
    illusionButton->show();

    // Conectează semnalul de click
    connect(illusionButton, &QPushButton::clicked, this, &Eter_UI::onIllusionButtonClicked);
}


void Eter_UI::onIllusionButtonClicked() {
    if (!m_game) return;

    // Verifică tura curentă și oferă doar opțiunea corespunzătoare
    QString cardColor;
    if (isRedTurn) {
        cardColor = "Carte Roșie";
    }
    else {
        cardColor = "Carte Albastră";
    }

    // Dialog pentru confirmarea alegerii
    bool ok;
    QString selected = QInputDialog::getItem(this, "Alegere Carte Iluzie",
        "Confirmați crearea cărții de iluzie:", QStringList() << cardColor, 0, false, &ok);

    if (!ok || selected.isEmpty())
        return;

    // Dialog pentru alegerea valorii
    QStringList valueOptions;
    valueOptions << "1" << "2" << "3" << "4";

    QString selectedValue = QInputDialog::getItem(this, "Alegere Valoare",
        "Alegeți valoarea cărții:", valueOptions, 0, false, &ok);

    if (!ok || selectedValue.isEmpty())
        return;

    // Construiește numele pentru proprietatea cardName (pentru logica jocului)
    QString cardName = (isRedTurn ? "R" : "B") + selectedValue;

    // Construiește calea către imaginea de iluzie
    QString imagePath = QCoreApplication::applicationDirPath() + "/cards/" +
        (isRedTurn ? "RI" : "BI") + ".png";

    // Convertește valoarea selectată în Card::Value
    Card::Value cardValue;
    switch (selectedValue.toInt()) {
    case 1: cardValue = Card::Value::One; break;
    case 2: cardValue = Card::Value::Two; break;
    case 3: cardValue = Card::Value::Three; break;
    case 4: cardValue = Card::Value::Four; break;
    default: cardValue = Card::Value::One; break;
    }

    // Creează CardLabel cu imaginea de iluzie dar valoarea reală
    CardLabel* illusionCard = new CardLabel(imagePath, cardValue, this);
    illusionCard->setProperty("cardName", cardName);

    // Poziționează cartea în stânga tablei
    int boardX = width() / 2 - (m_game->getBoard().getSize() * 100) / 2;
    illusionCard->setGeometry(boardX - 120, height() / 2 + 50, 100, 150);

    cards.append(illusionCard);
    illusionCard->show();
    updateCardStacks();
}


void Eter_UI::createCards(QPushButton* clickedButton) {
    QString cardsPath = QCoreApplication::applicationDirPath() + "/cards/";
    if (!QDir(cardsPath).exists()) {
        qDebug() << "Error: 'cards' folder does not exist at:" << cardsPath;
        return;
    }

    QStringList blueCards, redCards;
    QStringList wizardNames = {
        "eliminateCard", "eliminateRow", "coverCard", "sinkHole",
        "moveStackOwn", "extraEter", "moveStackOpponent", "moveEdge"
    };
    QStringList powerNames = {
        "controlledExplosion", "destruction", "flame", "lava",
        "ash", "spark", "squall", "gale", "hurricane", "gust",
        "mirage", "storm", "tide", "mist", "wave", "whirlpool",
        "tsunami", "waterfall", "support", "earthquake", "crumble",
        "border", "avalanche", "rock"
    };

    bool includeWizards = true; 

    if (clickedButton == buttonTraining) {
        blueCards = { "B1","B1","B2","B2","B3","B3","B4" };
        redCards = { "R1","R1","R2","R2","R3","R3","R4" };
        includeWizards = false;
    }
    else if (clickedButton == buttonWizard) {
        blueCards = { "B1","B1","B2","B2","B2","B3","B3","B3","B4","BE" };
        redCards = { "R1","R1","R2","R2","R2","R3","R3","R3","R4","RE" };
    }
    else if (clickedButton == buttonPowers || clickedButton == buttonWizardPowers) {
        blueCards = { "B1","B2","B2","B2","B3","B3","B3","B4","BE" };
        redCards = { "R1","R2","R2","R2","R3","R3","R3","R4","RE" };
        includeWizards = (clickedButton == buttonWizardPowers);

        auto addRandomPowers = [&](const QStringList& pnames, int startX, int startY) {
            for (int i = 0; i < 2; ++i) {
                int idx = QRandomGenerator::global()->bounded(pnames.size());
                QString powerName = pnames[idx];
                QString powerImagePath = cardsPath + powerName + ".png";
                if (QFile::exists(powerImagePath)) {
                    QLabel* powerLabel = new QLabel(this);
                    QPixmap pixmap(powerImagePath);
                    powerLabel->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
                    powerLabel->setGeometry(startX, startY, 100, 150);
                    powerLabel->show();
                    startX += 110;
                }
                else {
                    qDebug() << "Power image not found:" << powerImagePath;
                }
            }
            };
        int startXBlue = width() / 2 + ((blueCards.size() / 2) * 110) + 120;
        int startYBlue = height() / 2 + 230;
        addRandomPowers(powerNames, startXBlue, startYBlue);

        int startXRed = width() / 2 + ((redCards.size() / 2) * 110) + 120;
        int startYRed = height() / 2 - 340;
        addRandomPowers(powerNames, startXRed, startYRed);
    }

    int blueWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
    int redWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
    QString blueWizard = wizardNames[blueWizardIndex];
    QString redWizard = wizardNames[redWizardIndex];

    auto createCard = [&](const QString& cardName, int& startX, int startY) {
        QString imagePath = cardsPath + cardName + ".png";
        if (!QFile::exists(imagePath)) {
            qDebug() << "Image not found:" << imagePath;
            return;
        }
        char valChar = cardName.at(cardName.size() - 1).toLatin1();
        Card::Value val = charToCardValue(valChar);

        CardLabel* card = new CardLabel(imagePath, val, this);
        card->setProperty("cardName", cardName);
        card->setGeometry(startX, startY, 100, 150);
        cards.append(card);
        card->show();
        startX += 110;
        };

    auto addWizardCard = [&](const QString& wizName, int startX, int startY) {
        if (!includeWizards) return;
        QString wizPath = cardsPath + wizName + ".png";
        if (QFile::exists(wizPath)) {
            QLabel* wizardLabel = new QLabel(this);
            QPixmap pixmap(wizPath);
            wizardLabel->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
            wizardLabel->setGeometry(startX, startY, 100, 150);
            wizardLabel->show();
        }
        else {
            qDebug() << "Wizard image not found:" << wizPath;
        }
        };

    int startXBlue = width() / 2 - ((blueCards.size() / 2) * 110);
    int startYBlue = height() / 2 + 230;
    addWizardCard(blueWizard, startXBlue - 120, startYBlue);
    for (const QString& cardName : blueCards) {
        createCard(cardName, startXBlue, startYBlue);
    }

    int startXRed = width() / 2 - ((redCards.size() / 2) * 110);
    int startYRed = height() / 2 - 340;
    addWizardCard(redWizard, startXRed - 120, startYRed);
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
   /// default:  return Card::Value::Undefined;
    }
}

void Eter_UI::createShiftButtons() {
    const int buttonSize = 50;
    const int spacing = 10;

    int boardPixels = 300;
    int startX = width() / 2 + boardPixels / 2 + spacing * 2;
    int startY = height() / 2 - (buttonSize * 2 + spacing);

    QString buttonStyle = R"(
       QPushButton {
          background-color: rgba(255, 255, 255, 180);
          border: 2px solid black;
          border-radius: 5px;
       }
       QPushButton:hover {
          background-color: rgba(255, 255, 255, 220);
       }
       QPushButton:disabled {
          background-color: rgba(128, 128, 128, 180);
       }
    )";

    shiftUpButton = new QPushButton("↑", this);
    shiftDownButton = new QPushButton("↓", this);
    shiftLeftButton = new QPushButton("←", this);
    shiftRightButton = new QPushButton("→", this);

    if (shiftUpButton) {
        shiftUpButton->setFixedSize(buttonSize, buttonSize);
        shiftUpButton->setStyleSheet(buttonStyle);
        shiftUpButton->setFont(QFont("Arial", 20, QFont::Bold));
        shiftUpButton->move(startX, startY);
        shiftUpButton->show();
        connect(shiftUpButton, &QPushButton::clicked, this, &Eter_UI::onShiftUp);
    }
    if (shiftLeftButton) {
        shiftLeftButton->setFixedSize(buttonSize, buttonSize);
        shiftLeftButton->setStyleSheet(buttonStyle);
        shiftLeftButton->setFont(QFont("Arial", 20, QFont::Bold));
        shiftLeftButton->move(startX, startY + buttonSize + spacing);
        shiftLeftButton->show();
        connect(shiftLeftButton, &QPushButton::clicked, this, &Eter_UI::onShiftLeft);
    }
    if (shiftRightButton) {
        shiftRightButton->setFixedSize(buttonSize, buttonSize);
        shiftRightButton->setStyleSheet(buttonStyle);
        shiftRightButton->setFont(QFont("Arial", 20, QFont::Bold));
        shiftRightButton->move(startX, startY + (buttonSize + spacing) * 2);
        shiftRightButton->show();
        connect(shiftRightButton, &QPushButton::clicked, this, &Eter_UI::onShiftRight);
    }
    if (shiftDownButton) {
        shiftDownButton->setFixedSize(buttonSize, buttonSize);
        shiftDownButton->setStyleSheet(buttonStyle);
        shiftDownButton->setFont(QFont("Arial", 20, QFont::Bold));
        shiftDownButton->move(startX, startY + (buttonSize + spacing) * 3);
        shiftDownButton->show();
        connect(shiftDownButton, &QPushButton::clicked, this, &Eter_UI::onShiftDown);
    }

    updateShiftButtons();
}

void Eter_UI::onShiftUp() {
    if (!m_game) return;

    Board& board = m_game->getBoard();
    if (board.circularShiftUp()) {
        updateBoardDisplay();
        checkWinCondition();
    }
    else {
        qDebug() << "circularShiftUp did not execute properly";
    }
}

void Eter_UI::onShiftDown() {
    if (!m_game) return;

    Board& board = m_game->getBoard();
    if (board.circularShiftDown()) {
        updateBoardDisplay();
        checkWinCondition();
    }
    else {
        qDebug() << "circularShiftDown did not execute properly";
    }
}

void Eter_UI::onShiftLeft() {
    if (!m_game) return;

    Board& board = m_game->getBoard();
    if (board.circularShiftLeft()) {
        updateBoardDisplay();
        checkWinCondition();
    }
    else {
        qDebug() << "circularShiftLeft did not execute properly";
    }
}

void Eter_UI::onShiftRight() {
    if (!m_game) return;

    Board& board = m_game->getBoard();
    if (board.circularShiftRight()) {
        updateBoardDisplay();
        checkWinCondition();
    }
    else {
        qDebug() << "circularShiftRight did not execute properly";
    }
}
void Eter_UI::onCardPlaced(QDropEvent* event, BoardCell* cell) {
    if (!m_game) return;

    CardLabel* card = qobject_cast<CardLabel*>(event->source());
    if (!card) {
        QMessageBox::warning(this, "Eroare", "Nu s-a putut plasa cartea.");
        return;
    }

    int row = cell->getRow();
    int col = cell->getCol();

    QString cardName = card->property("cardName").toString();
    Card::Color cardColor = isRedTurn ? Card::Color::Red : Card::Color::Blue;

    if (isRedTurn && !cardName.contains("R")) {
        QMessageBox::warning(this, "Mutare interzisă", "Nu este rândul jucătorului roșu!");
        return;
    }
    if (!isRedTurn && !cardName.contains("B")) {
        QMessageBox::warning(this, "Mutare interzisă", "Nu este rândul jucătorului albastru!");
        return;
    }

    Board& board = m_game->getBoard();

    char valChar = cardName.at(cardName.size() - 1).toLatin1();
    Card::Value val = charToCardValue(valChar);

    if (firstCardPlaced) {
        if (!board.checkNeighbours(row, col)) {
            QMessageBox::warning(this, "Mutare interzisă",
                "Cartea trebuie plasată lângă (adiacent) unei cărți deja existente!");
            return;
        }
    }

    if (!board.checkValue(row, col, val)) {
        QMessageBox::warning(this, "Mutare interzisă", "Cartea nu poate fi plasata in acest loc!");
        return;
    }

    Card newCard(val, cardColor);
    board.getBoard()[row][col].push_back(newCard);

    cell->setPixmap(card->pixmap(Qt::ReturnByValue).scaled(100, 150, Qt::KeepAspectRatio));
    card->hide();

    if (!firstCardPlaced) {
        firstCardPlaced = true;
    }

    isRedTurn = !isRedTurn;
    updateTurnLabel();
    updateCardStacks();

    Card::Color winner = board.checkWin();
    if (winner != Card::Color::Undefined) {
        showWinMessage(winner);
    }
}


void Eter_UI::updateShiftButtons() {
    if (!m_game) return;

    Board& board = m_game->getBoard();

    bool canShiftUp = board.circularShiftUp(true);   
    bool canShiftDown = board.circularShiftDown(true);
    bool canShiftLeft = board.circularShiftLeft(true);
    bool canShiftRight = board.circularShiftRight(true);

    if (shiftUpButton)    shiftUpButton->setEnabled(canShiftUp);
    if (shiftDownButton)  shiftDownButton->setEnabled(canShiftDown);
    if (shiftLeftButton)  shiftLeftButton->setEnabled(canShiftLeft);
    if (shiftRightButton) shiftRightButton->setEnabled(canShiftRight);
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
    if (!m_game) return;

    Card::Color winner = m_game->getBoard().checkWin();
    if (winner != Card::Color::Undefined) {
        showWinMessage(winner);
    }
}

void Eter_UI::removeCard(CardLabel* card) {
    cards.removeOne(card);
    card->deleteLater();

    isRedTurn = !isRedTurn;
    updateTurnLabel();
    checkWinCondition();
}



void Eter_UI::processCardPlacement(Player& player, int row, int col, Card::Value cardValue) {
    // Ex. dacă ai nevoie de logică la plasare
    // TOTUL însă să meargă prin m_game->getBoard()
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
    for (CardLabel* c : cards) {
        if (!c) continue;
        if (c->isHidden()) continue;

        QString name = c->property("cardName").toString();
        bool isRedCard = name.contains("R", Qt::CaseInsensitive);
        bool isBlueCard = name.contains("B", Qt::CaseInsensitive);

        if (isRedTurn) {
            c->setEnabled(isRedCard);
        }
        else {
            c->setEnabled(isBlueCard);
        }
    }
}

void Eter_UI::cleanCardStack() {
    cards.erase(
        std::remove_if(cards.begin(), cards.end(), [](CardLabel* c) {
            return !c || c->isHidden();
            }),
        cards.end()
    );
}


void Eter_UI::updateBoardDisplay() {
    const auto& boardData = m_game->getBoard().getBoard();
    for (int row = 0; row < (int)boardData.size(); ++row) {
        for (int col = 0; col < (int)boardData[row].size(); ++col) {
            BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(row, col)->widget());
            if (!cell) continue;

            if (boardData[row][col].empty()) {
                cell->clear(); 
            }
            else {
                const Card& topCard = boardData[row][col].back();
                QString colorPrefix = (topCard.getColor() == Card::Color::Red) ? "R" : "B";
                QString valSuffix = QString::number(static_cast<int>(topCard.getValue()));
                QString imagePath = QCoreApplication::applicationDirPath() + "/cards/" + colorPrefix + valSuffix + ".png";
                QPixmap pixmap(imagePath);
                cell->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
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
        qDebug() << "Eroare: Nu am găsit imaginea pt puterea" << powerName;
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
void Eter_UI::startNewTurn() {
  
    if (!m_game) return;

    Matrix<Card>& boardData = m_game->getBoard().getBoard();
    for (auto& row : boardData) {
        for (auto& cell : row) {
            cell.clear(); 
        }
    }
    m_game->getBoard().m_firstCardPlayed = false;

    
    cleanCardStack();       
    createCards(nullptr);   

    isRedTurn = true;
    firstCardPlaced = false;  

    updateTurnLabel();

    updateBoardDisplay();
    updateCardStacks();

    QMessageBox::information(this, "New Turn", "A new turn has started!");
}
void Eter_UI::showWinMessage(Card::Color winner) {
    QString winnerText = (winner == Card::Color::Red) ? "Red Player" : "Blue Player";

    // Dacă modul este Training, incrementăm scorul și verificăm dacă cineva a ajuns la 2.
    if (m_game && m_game->getGameType() == Game::GameType::Training) {
        if (winner == Card::Color::Blue) {
            m_blueScore++;
        }
        else if (winner == Card::Color::Red) {
            m_redScore++;
        }

        // Actualizăm textul Label-ului de scor
        updateScoreLabel();  // Vezi implementarea mai jos

        // Verificăm dacă unul dintre jucători a obținut 2 victorii (best of 3)
        if (m_blueScore == 2) {
            QMessageBox::information(
                this,
                "Game Over",
                QString("Blue Player wins the Best of 3 in Training Mode!")
            );
            // Resetăm scorul sau revenim la meniu, depinde de logică:
            m_blueScore = 0;
            m_redScore = 0;
            updateScoreLabel();
            // Poți alege să oprești jocul sau să faci un startNewTurn, după preferințe.
            return;
        }
        else if (m_redScore == 2) {
            QMessageBox::information(
                this,
                "Game Over",
                QString("Red Player wins the Best of 3 in Training Mode!")
            );
            // Reset scor
            m_blueScore = 0;
            m_redScore = 0;
            updateScoreLabel();
            return;
        }

        // Dacă nimeni nu a atins 2, reluăm jocul (startNewTurn)
        QMessageBox::information(
            this,
            "Match Over",
            QString("%1 has won this match! Next match starts...").arg(winnerText)
        );
        startNewTurn();
    }
    else {
        // Pentru alte moduri de joc (Wizard, Powers etc.):
        QMessageBox::information(
            this,
            "Game Over",
            QString("%1 has won the game! Starting a new turn...").arg(winnerText)
        );
        startNewTurn();
    }
}
void Eter_UI::updateScoreLabel() {
    if (!scoreLabel) return;
    // Afișăm ceva de forma "Score: Blue 1 - 0 Red"
    scoreLabel->setText(QString("Score: Blue %1 - %2 Red")
        .arg(m_blueScore)
        .arg(m_redScore)
    );
    scoreLabel->adjustSize();
    // Repoziționăm dacă vrei să fie chiar în colț (opțional):
    scoreLabel->move(width() - scoreLabel->width() - 20, 20);
}