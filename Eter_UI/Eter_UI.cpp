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

//--------------------------------------------------
// Helper care creează și atașează un buton la fereastră
//--------------------------------------------------
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

//--------------------------------------------------
// Inițializează butoane la ecranul de start
//--------------------------------------------------
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

//--------------------------------------------------
// Constructor
//--------------------------------------------------
Eter_UI::Eter_UI(QWidget* parent)
    : QMainWindow(parent),
    isStartPage(true),
    isRedTurn(false),
    m_game(nullptr),
    m_match(nullptr),
    boardLayout(nullptr),
    powerCardLabel(nullptr)
{
    // Setează UI din .ui (dacă ai un .ui generat)
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
    powerCardLabel->hide(); // Ascuns până la activarea unei puteri

    // Inițializează QLabel pentru indicarea rândului
    turnLabel = new QLabel(this);
    turnLabel->setFont(QFont("Arial", 14, QFont::Bold));
    turnLabel->setStyleSheet("color: white; background-color: rgba(0, 0, 0, 128); padding: 5px;");
    turnLabel->setAlignment(Qt::AlignCenter);
    turnLabel->hide(); // Ascunde până începe jocul
}

//--------------------------------------------------
// Destructor
//--------------------------------------------------
Eter_UI::~Eter_UI() {
    // Nu mai ștergem gameBoard (nu îl mai avem aici),
    // m_game se ocupă de curățare deoarece e std::unique_ptr
}

//--------------------------------------------------
// Buton principal: decide tipul de joc și pornește jocul
//--------------------------------------------------
void Eter_UI::OnButtonClick() {
    // Identifică butonul apăsat
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    // Ascunde tot (inclusiv butoanele) și curăță interfața
    isStartPage = false;
    for (QObject* child : children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this && widget != turnLabel) {
                widget->hide();
                widget->deleteLater();
            }
        }
    }

    // Ce tip de joc avem?
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
        return; // Nimic
    }

    // Creează instanța jocului (dimensiunea tablei, etc.)
    // Ajustează parametrii cum ai nevoie
    try {
        m_game = std::make_unique<Game>(
            gameType,
            std::make_pair<size_t, size_t>(0, 1),  // Indici vrăjitori, dacă e relevant
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

    // Creează partea vizuală a tablei (grila de BoardCell)
    createBoard(clickedButton);

    // Creează și afișează cărțile (CardLabel)
    createCards(clickedButton);

    // Creează butoanele de shift
    createShiftButtons();

    // Afișează eticheta cu rândul curent
    turnLabel->setGeometry(width() / 2 - 100, 50, 200, 40);
    turnLabel->show();
    updateTurnLabel();

    // Refresh
    update();
}

//--------------------------------------------------
// Pictăm fundal + logo
//--------------------------------------------------
void Eter_UI::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);

    QLinearGradient gradient(0, height(), width(), 0);
    gradient.setColorAt(0.0, QColor(128, 0, 0));
    gradient.setColorAt(0.5, QColor(64, 0, 64));
    gradient.setColorAt(1.0, QColor(0, 0, 64));

    painter.fillRect(rect(), gradient);

    // Încearcă încărcarea unui logo
    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.cdUp();
    QString logoPath = appDir.absoluteFilePath("logo.png");

    QPixmap logo(logoPath);
    if (!logo.isNull()) {
        QRect logoRect;
        if (isStartPage) {
            // Mare și centrat
            logoRect = QRect(width() / 2 - logo.width() / 2,
                height() / 4 - 150,
                logo.width(),
                logo.height());
        }
        else {
            // Mic, colț stânga
            logoRect = QRect(10, 10, 150, 75);
        }
        painter.drawPixmap(logoRect, logo);
    }
    else {
        qDebug() << "Error: Failed to load image:" << logoPath;
    }
}

//--------------------------------------------------
// Creăm tabla (strict UI): un QGridLayout cu BoardCell
//--------------------------------------------------
void Eter_UI::createBoard(QPushButton* clickedButton) {
    // Ștergem layoutul vechi (dacă există)
    if (boardLayout) {
        delete boardLayout;
        boardLayout = nullptr;
    }

    int boardSize = 3; // default
    if (clickedButton == buttonWizard ||
        clickedButton == buttonPowers ||
        clickedButton == buttonWizardPowers)
    {
        boardSize = 4;
    }

    // Eventual ai setat deja boardSize intern în Game, 
    // dar dacă mai vrei să-l stabilești explicit:
    // m_game->getBoard().setSize(boardSize); 
    // (depinde de implementarea ta)

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
    boardWidget->setLayout(boardLayout);
    boardWidget->setGeometry(width() / 2 - widgetSize / 2,
        height() / 2 - widgetSize / 2,
        widgetSize,
        widgetSize);
    boardWidget->show();
}

//--------------------------------------------------
// Creează și așază cărțile la marginea ferestrei
//--------------------------------------------------
void Eter_UI::createCards(QPushButton* clickedButton) {
    QString cardsPath = QCoreApplication::applicationDirPath() + "/cards/";
    if (!QDir(cardsPath).exists()) {
        qDebug() << "Error: 'cards' folder does not exist at:" << cardsPath;
        return;
    }

    // Liste de cărți
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

    bool includeWizards = true; // ajustăm mai jos

    // Stabilim cărțile după modul de joc
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

        // Exemplu: adăugăm random niște puteri (dacă vrei)
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

    // Vrăjitori random
    int blueWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
    int redWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
    QString blueWizard = wizardNames[blueWizardIndex];
    QString redWizard = wizardNames[redWizardIndex];

    // Helper: creează un CardLabel
    auto createCard = [&](const QString& cardName, int& startX, int startY) {
        QString imagePath = cardsPath + cardName + ".png";
        if (!QFile::exists(imagePath)) {
            qDebug() << "Image not found:" << imagePath;
            return;
        }
        // Ultimul caracter e 1..4 sau E
        char valChar = cardName.at(cardName.size() - 1).toLatin1();
        Card::Value val = charToCardValue(valChar);

        CardLabel* card = new CardLabel(imagePath, val, this);
        card->setProperty("cardName", cardName);
        card->setGeometry(startX, startY, 100, 150);
        cards.append(card);
        card->show();
        startX += 110;
        };

    // Helper: afișează un wizard
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

    // Creează cărțile albastre
    int startXBlue = width() / 2 - ((blueCards.size() / 2) * 110);
    int startYBlue = height() / 2 + 230;
    addWizardCard(blueWizard, startXBlue - 120, startYBlue);
    for (const QString& cardName : blueCards) {
        createCard(cardName, startXBlue, startYBlue);
    }

    // Creează cărțile roșii
    int startXRed = width() / 2 - ((redCards.size() / 2) * 110);
    int startYRed = height() / 2 - 340;
    addWizardCard(redWizard, startXRed - 120, startYRed);
    for (const QString& cardName : redCards) {
        createCard(cardName, startXRed, startYRed);
    }

    // Activează/dezactivează stivele
    updateCardStacks();
}

//--------------------------------------------------
// Convertește un caracter ('1','2','3','4','E') în Card::Value
//--------------------------------------------------
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

//--------------------------------------------------
// Creează butoanele de shift și le conectează
//--------------------------------------------------
void Eter_UI::createShiftButtons() {
    const int buttonSize = 50;
    const int spacing = 10;

    // Poziționare la dreapta tablei
    int boardPixels = 300; // Să fie puțin offset; modifică după nevoi
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

    // Inițial, update
    updateShiftButtons();
}

//--------------------------------------------------
// Handler pt shift UP
//--------------------------------------------------
void Eter_UI::onShiftUp() {
    if (!m_game) return;

    Board& board = m_game->getBoard();
    if (board.circularShiftUp()) {
        // Logica a mutat, acum reafișăm
        updateBoardDisplay();
        checkWinCondition();
    }
    else {
        qDebug() << "circularShiftUp did not execute properly";
    }
}

//--------------------------------------------------
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

//--------------------------------------------------
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

//--------------------------------------------------
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

//--------------------------------------------------
// Când plasăm o carte
//--------------------------------------------------
void Eter_UI::onCardPlaced(QDropEvent* event, BoardCell* cell)
{
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

    // Verificăm dacă jucătorul are dreptul să plaseze cartea (culoarea)
    if (isRedTurn && !cardName.contains("R")) {
        QMessageBox::warning(this, "Mutare interzisă", "Nu este rândul jucătorului roșu!");
        return;
    }
    if (!isRedTurn && !cardName.contains("B")) {
        QMessageBox::warning(this, "Mutare interzisă", "Nu este rândul jucătorului albastru!");
        return;
    }

    // Referință la Board
    Board& board = m_game->getBoard();

    // Dacă s-a mai plasat o carte înainte, impunem plasarea lângă una existentă
    if (firstCardPlaced) {
        if (!board.checkNeighbours(row, col)) {
            QMessageBox::warning(this, "Mutare interzisă",
                "Cartea trebuie plasată lângă (adiacent) unei cărți deja existente!");
            return;
        }
    }

    // Construcția cărții logice
    char valChar = cardName.at(cardName.size() - 1).toLatin1();
    Card::Value val = charToCardValue(valChar);
    Card newCard(val, cardColor);

    // Plasare în Board (logica jocului)
    board.getBoard()[row][col].push_back(newCard);

    // Actualizare UI: punem imaginea în celulă și ascundem cartea din mână
    cell->setPixmap(card->pixmap(Qt::ReturnByValue).scaled(100, 150, Qt::KeepAspectRatio));
    card->hide();

    // Dacă este prima carte plasată, marcăm acest lucru
    if (!firstCardPlaced) {
        firstCardPlaced = true;
    }

    // Schimbăm jucătorul curent și actualizăm interfața
    isRedTurn = !isRedTurn;
    updateTurnLabel();
    updateCardStacks();

    // Verificăm dacă cineva a câștigat
    Card::Color winner = board.checkWin();
    if (winner != Card::Color::Undefined) {
        showWinMessage(winner);
    }
}


//--------------------------------------------------
// Actualizează butoanele de shift pe baza logicii
//--------------------------------------------------
void Eter_UI::updateShiftButtons() {
    if (!m_game) return;

    Board& board = m_game->getBoard();

    bool canShiftUp = board.circularShiftUp(true);    // mod "simulate only"
    bool canShiftDown = board.circularShiftDown(true);
    bool canShiftLeft = board.circularShiftLeft(true);
    bool canShiftRight = board.circularShiftRight(true);

    if (shiftUpButton)    shiftUpButton->setEnabled(canShiftUp);
    if (shiftDownButton)  shiftDownButton->setEnabled(canShiftDown);
    if (shiftLeftButton)  shiftLeftButton->setEnabled(canShiftLeft);
    if (shiftRightButton) shiftRightButton->setEnabled(canShiftRight);
}

//--------------------------------------------------
// Actualizează label-ul cu rândul curent
//--------------------------------------------------
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

//--------------------------------------------------
void Eter_UI::onWizardPowersClicked() {
    OnButtonClick();
}

//--------------------------------------------------
void Eter_UI::checkWinCondition() {
    if (!m_game) return;

    Card::Color winner = m_game->getBoard().checkWin();
    if (winner != Card::Color::Undefined) {
        showWinMessage(winner);
    }
}

//--------------------------------------------------
void Eter_UI::removeCard(CardLabel* card) {
    cards.removeOne(card);
    card->deleteLater();

    isRedTurn = !isRedTurn;
    updateTurnLabel();
    checkWinCondition();
}

//--------------------------------------------------
void Eter_UI::showWinMessage(Card::Color winner) {
    QString winnerText = (winner == Card::Color::Red) ? "Red Player" : "Blue Player";
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Game Over");
    msgBox.setText(QString("%1 has won the game!").arg(winnerText));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

//--------------------------------------------------
void Eter_UI::processCardPlacement(Player& player, int row, int col, Card::Value cardValue) {
    // Ex. dacă ai nevoie de logică la plasare
    // TOTUL însă să meargă prin m_game->getBoard()
}

//--------------------------------------------------
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

//--------------------------------------------------
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

//--------------------------------------------------
void Eter_UI::updateCardStacks() {
    // Activează / dezactivează cardurile în funcție de rând
    for (CardLabel* c : cards) {
        if (!c) continue;
        if (c->isHidden()) continue;

        QString name = c->property("cardName").toString();
        bool isRedCard = name.contains("R", Qt::CaseInsensitive);
        bool isBlueCard = name.contains("B", Qt::CaseInsensitive);
        // Poți adăuga extra condiții pentru alt tip de carte

        if (isRedTurn) {
            c->setEnabled(isRedCard);
        }
        else {
            c->setEnabled(isBlueCard);
        }
    }
}

//--------------------------------------------------
void Eter_UI::cleanCardStack() {
    cards.erase(
        std::remove_if(cards.begin(), cards.end(), [](CardLabel* c) {
            return !c || c->isHidden();
            }),
        cards.end()
    );
}

//--------------------------------------------------
void Eter_UI::updateBoardDisplay() {
    // Citește starea din m_game->getBoard() și desenează
    const auto& boardData = m_game->getBoard().getBoard();
    for (int row = 0; row < (int)boardData.size(); ++row) {
        for (int col = 0; col < (int)boardData[row].size(); ++col) {
            BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(row, col)->widget());
            if (!cell) continue;

            if (!boardData[row][col].empty()) {
                const Card& topCard = boardData[row][col].back();
                // Construim numele fișierului
                QString colorPrefix = (topCard.getColor() == Card::Color::Red) ? "R" : "B";
                QString valSuffix;
                switch (topCard.getValue()) {
                case Card::Value::One:   valSuffix = "1"; break;
                case Card::Value::Two:   valSuffix = "2"; break;
                case Card::Value::Three: valSuffix = "3"; break;
                case Card::Value::Four:  valSuffix = "4"; break;
                case Card::Value::Eter:  valSuffix = "E"; break;
                default:                 valSuffix = "";
                }
                QString imagePath = QCoreApplication::applicationDirPath()
                    + "/cards/" + colorPrefix + valSuffix + ".png";

                QPixmap pixmap(imagePath);
                if (!pixmap.isNull()) {
                    cell->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
                }
                else {
                    cell->clear();
                }
            }
            else {
                cell->clear();
            }
        }
    }

    updateShiftButtons();
}

//--------------------------------------------------
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

//--------------------------------------------------
void Eter_UI::activateWizardPower(size_t powerIndex, Player& player, Game& game) {
    // Exemplu: vrăjitorul face ceva, apoi afișăm cartea
    Wizard& wizard = Wizard::getInstance();
    if (wizard.play(powerIndex, player, game, /*check=*/false)) {
        QString powerName = QString::fromStdString(wizard.getWizardName(powerIndex));
        displayPowerCard(powerName);
    }
    else {
        QMessageBox::warning(this, "Activare eșuată", "Puterea nu a putut fi activată.");
    }
}
