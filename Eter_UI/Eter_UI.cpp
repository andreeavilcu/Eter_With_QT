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
    m_match(nullptr),
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
void Eter_UI::OnButtonClick() {
    // Identifică butonul apăsat
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    // Ascunde toate componentele existente și curăță interfața
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
    bool includeWizards = false; // Flag pentru a decide dacă se adaugă vrăjitori
    bool includePowers = false;  // Flag pentru a decide dacă se adaugă puteri

    // Determină tipul de joc pe baza butonului apăsat
    if (clickedButton == buttonTraining) {
        gameType = Game::GameType::Training;
    }
    else if (clickedButton == buttonWizard) {
        gameType = Game::GameType::WizardDuel;
        includeWizards = true; // Adaugă vrăjitori
    }
    else if (clickedButton == buttonPowers) {
        gameType = Game::GameType::PowerDuel;
        includePowers = true; // Adaugă puteri
    }
    else if (clickedButton == buttonWizardPowers) {
        gameType = Game::GameType::WizardAndPowerDuel;
        includeWizards = true;
        includePowers = true; // Adaugă și vrăjitori, și puteri
    }
    else {
        return; // Dacă nu s-a identificat butonul, ieșim
    }

    // Crearea și configurarea instanței jocului
    try {
        // Constructor specific pentru clasa Game
        m_game = std::make_unique<Game>(
            gameType,                     // Tipul jocului
            std::make_pair<size_t, size_t>(0, 1), // Indicii vrăjitorilor (explicit size_t)
            false,                        // illusionsAllowed
            false,                        // explosionAllowed
            false                         // tournament
        );
    }
    catch (const std::exception& e) {
        qDebug() << "Error creating game instance:" << e.what();
        QMessageBox::critical(this, "Error", "Failed to create game instance.");
        return;
    }

    // Creează tabla de joc
    createBoard(clickedButton);

    // Creează cărțile specifice tipului de joc
    createCards(clickedButton);

    // Adaugă butoane de shift (dacă sunt necesare pentru joc)
    createShiftButtons();

    // Actualizează eticheta pentru indicarea rândului
    turnLabel->setGeometry(width() / 2 - 100, 50, 200, 40);
    turnLabel->show();
    updateTurnLabel();

    // Reînnoiește interfața
    update();
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

    bool includeWizards = true;

    if (clickedButton == buttonTraining) {
        blueCards = { "B1", "B1", "B2", "B2", "B3", "B3", "B4" };
        redCards = { "R1", "R1", "R2", "R2", "R3", "R3", "R4" };
        includeWizards = false; // No wizards in Training Mode
    }
    else if (clickedButton == buttonWizard) {
        blueCards = { "B1", "B1", "B2", "B2", "B2", "B3", "B3", "B3", "B4", "BE" };
        redCards = { "R1", "R1", "R2", "R2", "R2", "R3", "R3", "R3", "R4", "RE" };
    }
    else if (clickedButton == buttonPowers || clickedButton == buttonWizardPowers) {
        blueCards = { "B1", "B2", "B2", "B2", "B3", "B3", "B3", "B4", "BE" };
        redCards = { "R1", "R2", "R2", "R2", "R3", "R3", "R3", "R4", "RE" };
        includeWizards = clickedButton == buttonWizardPowers;

        // Add random powers
        auto addRandomPowers = [&](const QStringList& powerNames, int startX, int startY) {
            for (int i = 0; i < 2; ++i) {
                int powerIndex = QRandomGenerator::global()->bounded(powerNames.size());
                QString powerName = powerNames[powerIndex];
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
                    qDebug() << "Error: Power image not found:" << powerImagePath;
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
    else {
        return;
    }

    // Select random wizards
    int blueWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
    int redWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
    QString blueWizard = wizardNames[blueWizardIndex];
    QString redWizard = wizardNames[redWizardIndex];

    // Lambda to create a card
    auto createCard = [&](const QString& cardName, int& startX, int startY) {
        QString imagePath = cardsPath + cardName + ".png";
        if (!QFile::exists(imagePath)) {
            qDebug() << "Error: Image not found:" << imagePath;
            return;
        }

        CardLabel* card = new CardLabel(imagePath,
            charToCardValue(cardName[cardName.size() - 1].toLatin1()),
            this);

        card->setProperty("cardName", cardName); // Set the cardName property
        card->setGeometry(startX, startY, 100, 150);
        cards.append(card); // Add the card to the global list
        card->show();
        startX += 110;
        };

    // Lambda to add a wizard card
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
            qDebug() << "Error: Wizard image not found:" << wizardImagePath;
        }
        };

    // Create blue cards and position the wizard at the start
    int startXBlue = width() / 2 - ((blueCards.size() / 2) * 110);
    int startYBlue = height() / 2 + 230;
    addWizardCard(blueWizard, startXBlue - 120, startYBlue);
    for (const QString& cardName : blueCards) {
        createCard(cardName, startXBlue, startYBlue);
    }

    // Create red cards and position the wizard at the start
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
        ///default:  return Card::Value::Undefined;
    }
}

void Eter_UI::createShiftButtons() {
    const int buttonSize = 50;
    const int spacing = 10;

    int boardSize = boardCells.size() * 50;  // va fi 150 pentru 3x3 sau 200 pentru 4x4
    int startX = width() / 2 + boardSize / 2 + spacing * 2;
    int startY = height() / 2 - (buttonSize * 2 + spacing);

    QString buttonStyle = "QPushButton {"
        "background-color: rgba(255, 255, 255, 180);"
        "border: 2px solid black;"
        "border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "background-color: rgba(255, 255, 255, 220);"
        "}"
        "QPushButton:disabled {"
        "background-color: rgba(128, 128, 128, 180);"
        "}";

    shiftUpButton = new QPushButton("↑", this);
    shiftDownButton = new QPushButton("↓", this);
    shiftLeftButton = new QPushButton("←", this);
    shiftRightButton = new QPushButton("→", this);

    // Aranjare verticală a butoanelor
    if (shiftUpButton) {
        shiftUpButton->setFixedSize(buttonSize, buttonSize);
        shiftUpButton->setStyleSheet(buttonStyle);
        shiftUpButton->setFont(QFont("Arial", 20, QFont::Bold));
        shiftUpButton->move(startX, startY);
        shiftUpButton->show();
    }

    if (shiftLeftButton) {
        shiftLeftButton->setFixedSize(buttonSize, buttonSize);
        shiftLeftButton->setStyleSheet(buttonStyle);
        shiftLeftButton->setFont(QFont("Arial", 20, QFont::Bold));
        shiftLeftButton->move(startX, startY + buttonSize + spacing);
        shiftLeftButton->show();
    }

    if (shiftRightButton) {
        shiftRightButton->setFixedSize(buttonSize, buttonSize);
        shiftRightButton->setStyleSheet(buttonStyle);
        shiftRightButton->setFont(QFont("Arial", 20, QFont::Bold));
        shiftRightButton->move(startX, startY + (buttonSize + spacing) * 2);
        shiftRightButton->show();
    }

    if (shiftDownButton) {
        shiftDownButton->setFixedSize(buttonSize, buttonSize);
        shiftDownButton->setStyleSheet(buttonStyle);
        shiftDownButton->setFont(QFont("Arial", 20, QFont::Bold));
        shiftDownButton->move(startX, startY + (buttonSize + spacing) * 3);
        shiftDownButton->show();
    }

    connect(shiftUpButton, &QPushButton::clicked, this, &Eter_UI::onShiftUp);
    connect(shiftDownButton, &QPushButton::clicked, this, &Eter_UI::onShiftDown);
    connect(shiftLeftButton, &QPushButton::clicked, this, &Eter_UI::onShiftLeft);
    connect(shiftRightButton, &QPushButton::clicked, this, &Eter_UI::onShiftRight);

    updateShiftButtons();
}

void Eter_UI::onShiftUp() {
    qDebug() << "Shift Up button pressed";
    if (!m_game) return;

    if (m_game->getBoard().circularShiftUp()) {
       
        m_game->getBoard().circularShiftUp();
        // Salvăm pixmapurile într-o matrice temporară
        QVector<QVector<QPixmap>> tempPixmaps;
        const int size = m_game->getBoard().getSize();
        tempPixmaps.resize(size);
        for (int i = 0; i < size; i++) {
            tempPixmaps[i].resize(size);
            for (int j = 0; j < size; j++) {
                BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(i, j)->widget());
                if (cell && !cell->pixmap().isNull()) {
                    tempPixmaps[i][j] = cell->pixmap(Qt::ReturnByValue);
                }
            }
        }

        // Mutăm pixmapurile
        for (int row = 0; row < size - 1; row++) {
            for (int col = 0; col < size; col++) {
                BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(row, col)->widget());
                if (cell) {
                    if (!tempPixmaps[row + 1][col].isNull()) {
                        cell->setPixmap(tempPixmaps[row + 1][col].scaled(100, 150, Qt::KeepAspectRatio));
                    }
                    else {
                        cell->clear();
                    }
                }
            }
        }

        // Curățăm ultima linie
        for (int col = 0; col < size; col++) {
            BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(size - 1, col)->widget());
            if (cell) {
                cell->clear();
            }
        }

        updateShiftButtons();
    }
}

void Eter_UI::onShiftDown() {
    if (!m_game) return;

    if (m_game->getBoard().circularShiftDown()) {
        const int size = m_game->getBoard().getSize();
        QVector<QVector<QPixmap>> tempPixmaps;
        tempPixmaps.resize(size);
        for (int i = 0; i < size; i++) {
            tempPixmaps[i].resize(size);
            for (int j = 0; j < size; j++) {
                BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(i, j)->widget());
                if (cell && !cell->pixmap().isNull()) {
                    tempPixmaps[i][j] = cell->pixmap(Qt::ReturnByValue);
                }
            }
        }

        for (int row = size - 1; row > 0; row--) {
            for (int col = 0; col < size; col++) {
                BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(row, col)->widget());
                if (cell) {
                    if (!tempPixmaps[row - 1][col].isNull()) {
                        cell->setPixmap(tempPixmaps[row - 1][col].scaled(100, 150, Qt::KeepAspectRatio));
                    }
                    else {
                        cell->clear();
                    }
                }
            }
        }

        // Curățăm prima linie
        for (int col = 0; col < size; col++) {
            BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(0, col)->widget());
            if (cell) {
                cell->clear();
            }
        }

        updateShiftButtons();
    }
}

void Eter_UI::onShiftLeft() {
    if (!m_game) return;

    if (m_game->getBoard().circularShiftLeft()) {
        const int size = m_game->getBoard().getSize();
        QVector<QVector<QPixmap>> tempPixmaps;
        tempPixmaps.resize(size);
        for (int i = 0; i < size; i++) {
            tempPixmaps[i].resize(size);
            for (int j = 0; j < size; j++) {
                BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(i, j)->widget());
                if (cell && !cell->pixmap().isNull()) {
                    tempPixmaps[i][j] = cell->pixmap(Qt::ReturnByValue);
                }
            }
        }

        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size - 1; col++) {
                BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(row, col)->widget());
                if (cell) {
                    if (!tempPixmaps[row][col + 1].isNull()) {
                        cell->setPixmap(tempPixmaps[row][col + 1].scaled(100, 150, Qt::KeepAspectRatio));
                    }
                    else {
                        cell->clear();
                    }
                }
            }
        }

        // Curățăm ultima coloană
        for (int row = 0; row < size; row++) {
            BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(row, size - 1)->widget());
            if (cell) {
                cell->clear();
            }
        }

        updateShiftButtons();
    }
}

void Eter_UI::onShiftRight() {
    if (!m_game) return;

    if (m_game->getBoard().circularShiftRight()) {
        const int size = m_game->getBoard().getSize();
        QVector<QVector<QPixmap>> tempPixmaps;
        tempPixmaps.resize(size);
        for (int i = 0; i < size; i++) {
            tempPixmaps[i].resize(size);
            for (int j = 0; j < size; j++) {
                BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(i, j)->widget());
                if (cell && !cell->pixmap().isNull()) {
                    tempPixmaps[i][j] = cell->pixmap(Qt::ReturnByValue);
                }
            }
        }

        for (int row = 0; row < size; row++) {
            for (int col = size - 1; col > 0; col--) {
                BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(row, col)->widget());
                if (cell) {
                    if (!tempPixmaps[row][col - 1].isNull()) {
                        cell->setPixmap(tempPixmaps[row][col - 1].scaled(100, 150, Qt::KeepAspectRatio));
                    }
                    else {
                        cell->clear();
                    }
                }
            }
        }

        // Curățăm prima coloană
        for (int row = 0; row < size; row++) {
            BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(row, 0)->widget());
            if (cell) {
                cell->clear();
            }
        }

        updateShiftButtons();
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

void Eter_UI::updateShiftButtons() {
    if (!m_game) {
        qDebug() << "No game instance";
        return;
    }

    bool checkTwoRows = gameBoard->checkTwoRows();
    bool emptyDeck = m_game->checkEmptyDeck();
    bool fullBoard = m_game->getBoard().checkFullBoard();
    qDebug() << "Empty deck:" << emptyDeck;
    qDebug() << "Full board:" << fullBoard;

    // Verificăm posibilitatea de shift pentru fiecare direcție
    bool canShiftUp = m_game->getBoard().circularShiftUp(true);
    bool canShiftDown = m_game->getBoard().circularShiftDown(true);
    bool canShiftLeft = m_game->getBoard().circularShiftLeft(true);
    bool canShiftRight = m_game->getBoard().circularShiftRight(true);


    qDebug() << "Can shift up:" << canShiftUp;
    qDebug() << "Can shift down:" << canShiftDown;
    qDebug() << "Can shift left:" << canShiftLeft;
    qDebug() << "Can shift right:" << canShiftRight;

    if (shiftUpButton) shiftUpButton->setEnabled(canShiftUp);
    if (shiftDownButton) shiftDownButton->setEnabled(canShiftDown);
    if (shiftLeftButton) shiftLeftButton->setEnabled(canShiftLeft);
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
    if (!card || !gameBoard) {
        QMessageBox::warning(this, "Eroare", "Nu s-a putut plasa cartea.");
        return;
    }

    int row = cell->getRow();
    int col = cell->getCol();

    QString cardName = card->property("cardName").toString();
    Card::Color cardColor = isRedTurn ? Card::Color::Red : Card::Color::Blue;

    // Verifică dacă jucătorul poate plasa cartea
    if ((isRedTurn && !cardName.contains("R")) || (!isRedTurn && !cardName.contains("B"))) {
        QMessageBox::warning(this, "Mutare interzisă", "Nu este rândul tău!");
        return;
    }

    // Verifică dacă poziția este validă (adiacentă dacă nu este prima carte)
    if (gameBoard->m_firstCardPlayed && !gameBoard->checkNeighbours(row, col)) {
        QMessageBox::warning(this, "Mutare interzisă", "Cartea trebuie plasată adiacent unei cărți existente!");
        return;
    }

    // Crează cartea și adaug-o în board
    Card::Value cardValue = static_cast<Card::Value>(card->property("cardValue").toInt());
    Card newCard(cardValue, cardColor);
    gameBoard->getBoard()[row][col].push_back(newCard);

    // Actualizează vizualizarea celulei
    cell->setPixmap(card->pixmap(Qt::ReturnByValue).scaled(100, 150, Qt::KeepAspectRatio));
    card->hide();

    // Marchează prima carte plasată
    if (!gameBoard->m_firstCardPlayed) {
        gameBoard->m_firstCardPlayed = true;
    }

    // Schimbă rândul și actualizează interfața
    isRedTurn = !isRedTurn;
    updateTurnLabel();
    updateCardStacks();

    // Verifică dacă există un câștigător
    Card::Color winner = gameBoard->checkWin();
    if (winner != Card::Color::Undefined) {
        showWinMessage(winner);
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
void Eter_UI::updateBoardDisplay() {
    // Accesează tabla de joc din gameBoard
    const auto& board = gameBoard->getBoard();

    for (int row = 0; row < static_cast<int>(board.size()); ++row) {
        for (int col = 0; col < static_cast<int>(board[row].size()); ++col) {
            BoardCell* cell = qobject_cast<BoardCell*>(boardLayout->itemAtPosition(row, col)->widget());
            if (!cell) continue;

            // Verifică dacă există cărți pe această poziție
            if (!board[row][col].empty()) {
                // Ia cartea din vârf
                const auto& topCard = board[row][col].back();

                // Construiește calea către imaginea cărții
                QString cardName = (topCard.getColor() == Card::Color::Red) ? "R" : "B";
                cardName += QString::number(static_cast<int>(topCard.getValue()));
                QString imagePath = QCoreApplication::applicationDirPath() + "/cards/" + cardName + ".png";

                // Setează imaginea în celulă
                QPixmap pixmap(imagePath);
                if (!pixmap.isNull()) {
                    cell->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
                }
            }
            else {
                // Dacă nu există cărți, curăță celula
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
//void Eter_UI::createWizards() {
//    QString cardsPath = QCoreApplication::applicationDirPath() + "/cards/";
//
//    QStringList wizardNames = {
//        "eliminateCard",
//        "eliminateRow",
//        "coverCard",
//        "sinkHole",
//        "moveStackOwn",
//        "extraEter",
//        "moveStackOpponent",
//        "moveEdge"
//    };
//
//    int startX = width() - 200; // Poziția în partea dreaptă
//    int startY = 100;
//
//    for (const QString& wizardName : wizardNames) {
//        QString imagePath = cardsPath + wizardName + ".png";
//
//        if (!QFile::exists(imagePath)) {
//            qDebug() << "Imaginea vrăjitorului lipsă: " << imagePath;
//            continue;
//        }
//
//        QLabel* wizardLabel = new QLabel(this);
//        QPixmap pixmap(imagePath);
//        wizardLabel->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
//        wizardLabel->setGeometry(startX, startY, 100, 150);
//        wizardLabel->show();
//
//        startY += 160; // Spațiere între imagini
//    }
//}

