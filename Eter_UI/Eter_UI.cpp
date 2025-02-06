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
#include <QRandomGenerator>
#include "BoardCell.h"
#include "CardLabel.h"

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
    scoreLabel->hide();  // Va fi afișat doar în modurile de joc corespunzătoare
}

Eter_UI::~Eter_UI() {
}

// Funcție generică pentru a crea butoane
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

// Inițializează butoanele din ecranul principal (Start Page)
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

    // Butoane jocuri simple
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

    // Butoane ce duc la alte meniuri (Tournament, Speed)
    createButton(buttonTournament, "Tournament game",
        centerX, centerY + (spacing + buttonHeight) * 3 - yOffset,
        buttonWidth, buttonHeight, buttonFont,
        &Eter_UI::drawTournamentMenu);

    createButton(buttonSpeed, "Speed game",
        centerX, centerY + (spacing + buttonHeight) * 4 - yOffset,
        buttonWidth, buttonHeight, buttonFont,
        &Eter_UI::drawSpeedMenu);
}

// Funcție apelată când se apasă butoanele din ecranul principal sau secundar
void Eter_UI::OnButtonClick() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    isStartPage = false;

    // Ascunde și șterge toate celelalte widget-uri (butoane, etc.) cu excepția label-urilor
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

    // Stabilim tipul de joc în funcție de buton
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
        // Joc cu vrăjitori și puteri
        gameType = Game::GameType::WizardAndPowerDuel;
        includeWizards = true;
        includePowers = true;
    }
    else {
        // Dacă nu e niciunul din cazurile de mai sus, nu facem nimic
        return;
    }

    // Creăm instanța de joc
    try {
        m_game = std::make_unique<Game>(
            gameType,
            std::make_pair<size_t, size_t>(0, 1),  // exemplu: indices wizards
            false, // illusionsAllowed
            false, // explosionAllowed
            false  // tournament (false deocamdată)
        );
    }
    catch (const std::exception& e) {
        qDebug() << "Error creating game instance:" << e.what();
        QMessageBox::critical(this, "Error", "Failed to create game instance.");
        return;
    }

    // Construim tabla, cărțile și butoanele de shift
    createBoard(clickedButton);
    createCards(clickedButton);
    //createShiftButtons();

    // Afișăm turnLabel
    turnLabel->setGeometry(width() / 2 - 100, 50, 200, 40);
    turnLabel->show();
    updateTurnLabel();

    // Afișăm și scoreLabel (doar pentru tipurile de joc simple sau duel)
    if (m_game &&
        (m_game->getGameType() == Game::GameType::Training ||
            m_game->getGameType() == Game::GameType::WizardDuel ||
            m_game->getGameType() == Game::GameType::PowerDuel ||
            m_game->getGameType() == Game::GameType::WizardAndPowerDuel))
    {
        scoreLabel->show();
    }

    update();
}

// Desenăm fundalul și eventual logo-ul
void Eter_UI::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);

    QLinearGradient gradient(0, height(), width(), 0);
    gradient.setColorAt(0.0, QColor(128, 0, 0));
    gradient.setColorAt(0.5, QColor(64, 0, 64));
    gradient.setColorAt(1.0, QColor(0, 0, 64));

    painter.fillRect(rect(), gradient);

    // Încercăm să încărcăm un logo (logo.png)
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

// Creăm tabla de joc (3x3 sau 4x4)
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
            cell->setStyleSheet("border: none; background: transparent;"); // Adaugă această linie
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

    if ((isRedTurn && redIllusionPlayed) || (!isRedTurn && blueIllusionPlayed)) {
        QMessageBox::warning(this, "Eroare", "Ați folosit deja iluzia!");
        return;
    }

    // Get card value from player
    bool ok;
    QString selectedValue = QInputDialog::getItem(this, "Alegere Valoare",
        "Alegeți valoarea cărții:", QStringList() << "1" << "2" << "3" << "4", 0, false, &ok);

    if (!ok || selectedValue.isEmpty())
        return;

    // Find a matching card in the player's hand (cards vector)
    QString prefix = isRedTurn ? "R" : "B";
    QString cardName = prefix + selectedValue;

    CardLabel* selectedCard = nullptr;
    for (CardLabel* card : cards) {
        if (card->property("cardName").toString() == cardName && !card->isHidden()) {
            selectedCard = card;
            break;
        }
    }

    if (!selectedCard) {
        QMessageBox::warning(this, "Eroare", "Nu aveți această carte în mână!");
        return;
    }

    // Set card as illusion
    selectedCard->setProperty("isIllusion", true);

    // Update card image to illusion
    QString imagePath = QCoreApplication::applicationDirPath() + "/cards/" +
        prefix + "I.png";
    selectedCard->setPixmap(QPixmap(imagePath).scaled(100, 150, Qt::KeepAspectRatio));
}


// Creăm cărțile pentru fiecare jucător, în funcție de butonul apăsat
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
    QStringList wizardDescriptions = {
        "Eliminate an opponent's card.",
        "Destroy an entire row of cards.",
        "Cover an opponent's card with a weaker one.",
        "Create a sinkhole at the target position.",
        "Move your stack to an empty spot.",
        "Gain an extra Eter card.",
        "Move an opponent's stack to an empty spot.",
        "Shift the edges of the board."
    };

    QStringList powerNames = {
        "controlledExplosion", "destruction", "flame", "lava",
        "ash", "spark", "squall", "gale", "hurricane", "gust",
        "mirage", "storm", "tide", "mist", "wave", "whirlpool",
        "tsunami", "waterfall", "support", "earthquake", "crumble",
        "border", "avalanche", "rock"
    };
    QStringList powerDescriptions = {
        "Controlled Explosion: Use an explosion to return cards to players' hands.",
        "Destruction: Destroy an opponent's card.",
        "Flame: Remove illusions from the board.",
        "Lava: Remove all cards of a selected value.",
        "Ash: Bring back an eliminated card.",
        "Spark: Cover an opponent's card with your own.",
        "Squall: Return an opponent's card to their hand.",
        "Gale: Clear all but the top cards in each stack.",
        "Hurricane: Shift a row or column of cards.",
        "Gust: Move a card to an adjacent position.",
        "Mirage: Replace your illusion with another card.",
        "Storm: Eliminate a pile with two or more cards.",
        "Tide: Switch two stacks of cards.",
        "Mist: Play an additional illusion card.",
        "Wave: Move a stack to an adjacent empty position.",
        "Whirlpool: Combine two separated cards into one stack.",
        "Tsunami: Restrict a row or column of the board.",
        "Waterfall: Merge all stacks in a row or column.",
        "Support: Increase a card's value by 1.",
        "Earthquake: Eliminate all cards with value 1.",
        "Crumble: Decrease an opponent's card value by 1.",
        "Border: Place a border card to limit the playable area.",
        "Avalanche: Swap two adjacent stacks.",
        "Rock: Cover an illusion without revealing it."
    };

    bool includeWizards = false;

    // Seturi diferite de cărți, în funcție de buton
    if (clickedButton == buttonTraining) {
        blueCards = { "B1", "B1", "B2", "B2", "B3", "B3", "B4" };
        redCards = { "R1", "R1", "R2", "R2", "R3", "R3", "R4" };
        includeWizards = false;
    }
    else if (clickedButton == buttonWizard) {
        blueCards = { "B1", "B1", "B2", "B2", "B2", "B3", "B3", "B3", "B4", "BE" };
        redCards = { "R1", "R1", "R2", "R2", "R2", "R3", "R3", "R3", "R4", "RE" };
        includeWizards = true;
    }
    else if (clickedButton == buttonPowers || clickedButton == buttonWizardPowers) {
        blueCards = { "B1", "B2", "B2", "B2", "B3", "B3", "B3", "B4", "BE" };
        redCards = { "R1", "R2", "R2", "R2", "R3", "R3", "R3", "R4", "RE" };
        // Doar dacă e butonWizardPowers, atunci avem vrăjitori
        includeWizards = (clickedButton == buttonWizardPowers);

        // Exemplu: adăugăm 2 puteri random la fiecare
        auto addRandomPowers = [&](const QStringList& pnames, int startX, int startY) {
            for (int i = 0; i < 2; ++i) {
                int idx = QRandomGenerator::global()->bounded(pnames.size());
                QString powerName = pnames[idx];
                QString powerImagePath = cardsPath + powerName + ".png";
                if (QFile::exists(powerImagePath)) {
                    CardLabel* powerLabel = new CardLabel(powerImagePath, Card::Value::Two, this); // exemplu Value
                    powerLabel->setDescription(powerDescriptions[idx]); // Hover text
                    powerLabel->setGeometry(startX, startY, 100, 150);
                    cards.append(powerLabel);
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

    // Creăm vrăjitorii (random) dacă e cazul
    int blueWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
    int redWizardIndex = QRandomGenerator::global()->bounded(wizardNames.size());
    QString blueWizard = wizardNames[blueWizardIndex];
    QString redWizard = wizardNames[redWizardIndex];

    // Funcție locală pentru a crea o carte normală
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

    // Funcție locală pentru a crea cartea de vrăjitor, dacă `includeWizards == true`
    auto addWizardCard = [&](const QString& wizName, int startX, int startY) {
        if (!includeWizards) return;
        QString wizPath = cardsPath + wizName + ".png";
        if (QFile::exists(wizPath)) {
            int wizardIndex = wizardNames.indexOf(wizName);
            CardLabel* wizardLabel = new CardLabel(wizPath, Card::Value::Three, this);
            // Setați description pt hover
            if (wizardIndex >= 0 && wizardIndex < wizardDescriptions.size()) {
                wizardLabel->setDescription(wizardDescriptions[wizardIndex]);
            }
            wizardLabel->setGeometry(startX, startY, 100, 150);
            cards.append(wizardLabel);
            wizardLabel->show();
        }
        else {
            qDebug() << "Wizard image not found:" << wizPath;
        }
        };

    // Poziționare cărți Albastre (Blue)
    int startXBlue = width() / 2 - ((blueCards.size() / 2) * 110);
    int startYBlue = height() / 2 + 230;
    addWizardCard(blueWizard, startXBlue - 120, startYBlue);
    for (const QString& cardName : blueCards) {
        createCard(cardName, startXBlue, startYBlue);
    }

    // Poziționare cărți Roșii (Red)
    int startXRed = width() / 2 - ((redCards.size() / 2) * 110);
    int startYRed = height() / 2 - 340;
    addWizardCard(redWizard, startXRed - 120, startYRed);
    for (const QString& cardName : redCards) {
        createCard(cardName, startXRed, startYRed);
    }

    // Actualizăm starea cărților (cine are voie să mute)
    updateCardStacks();
}

// Conversie caracter la valoare de carte
Card::Value Eter_UI::charToCardValue(char value) {
    switch (value) {
    case '1': return Card::Value::One;
    case '2': return Card::Value::Two;
    case '3': return Card::Value::Three;
    case '4': return Card::Value::Four;
    case 'E': return Card::Value::Eter;
    default:
        return Card::Value::One; // fallback
    }
}

// Creăm butoanele de shift (sus, jos, stânga, dreapta)
//void Eter_UI::createShiftButtons() {
//    const int buttonSize = 50;
//    const int spacing = 10;
//    const int offsetX = 30;
//
//    int boardPixels = 300;  // pentru un 3x3 => 300 pixeli
//    // (Pentru 4x4 e 400, însă se poate ajusta dacă vrei un calcul mai exact)
//    int startX = width() / 2 + boardPixels / 2 + spacing * 2 + offsetX;
//    int startY = height() / 2 - (buttonSize * 2 + spacing);
//
//    QString buttonStyle = R"(
//       QPushButton {
//          background-color: rgba(255, 255, 255, 180);
//          border: 2px solid black;
//          border-radius: 5px;
//       }
//       QPushButton:hover {
//          background-color: rgba(255, 255, 255, 220);
//       }
//       QPushButton:disabled {
//          background-color: rgba(128, 128, 128, 180);
//       }
//    )";
//
//    /*shiftUpButton = new QPushButton("↑", this);
//    shiftDownButton = new QPushButton("↓", this);
//    shiftLeftButton = new QPushButton("←", this);
//    shiftRightButton = new QPushButton("→", this);
//
//    if (shiftUpButton) {
//        shiftUpButton->setFixedSize(buttonSize, buttonSize);
//        shiftUpButton->setStyleSheet(buttonStyle);
//        shiftUpButton->setFont(QFont("Arial", 20, QFont::Bold));
//        shiftUpButton->move(startX, startY);
//        shiftUpButton->show();
//        connect(shiftUpButton, &QPushButton::clicked, this, &Eter_UI::onShiftUp);
//    }
//    if (shiftLeftButton) {
//        shiftLeftButton->setFixedSize(buttonSize, buttonSize);
//        shiftLeftButton->setStyleSheet(buttonStyle);
//        shiftLeftButton->setFont(QFont("Arial", 20, QFont::Bold));
//        shiftLeftButton->move(startX, startY + buttonSize + spacing);
//        shiftLeftButton->show();
//        connect(shiftLeftButton, &QPushButton::clicked, this, &Eter_UI::onShiftLeft);
//    }
//    if (shiftRightButton) {
//        shiftRightButton->setFixedSize(buttonSize, buttonSize);
//        shiftRightButton->setStyleSheet(buttonStyle);
//        shiftRightButton->setFont(QFont("Arial", 20, QFont::Bold));
//        shiftRightButton->move(startX, startY + (buttonSize + spacing) * 2);
//        shiftRightButton->show();
//        connect(shiftRightButton, &QPushButton::clicked, this, &Eter_UI::onShiftRight);
//    }
//    if (shiftDownButton) {
//        shiftDownButton->setFixedSize(buttonSize, buttonSize);
//        shiftDownButton->setStyleSheet(buttonStyle);
//        shiftDownButton->setFont(QFont("Arial", 20, QFont::Bold));
//        shiftDownButton->move(startX, startY + (buttonSize + spacing) * 3);
//        shiftDownButton->show();
//        connect(shiftDownButton, &QPushButton::clicked, this, &Eter_UI::onShiftDown);
//    }*/
//
//    // Verificăm dacă shift-urile sunt permise (opțional)
//    //updateShiftButtons();
//}

//void Eter_UI::updateShiftButtons() {
//    if (!m_game) return;
//
//    Board& board = m_game->getBoard();
//
//    bool canShiftUp = board.circularShiftUp(true);
//    bool canShiftDown = board.circularShiftDown(true);
//    bool canShiftLeft = board.circularShiftLeft(true);
//    bool canShiftRight = board.circularShiftRight(true);
//
//    if (shiftUpButton)    shiftUpButton->setEnabled(canShiftUp);
//    if (shiftDownButton)  shiftDownButton->setEnabled(canShiftDown);
//    if (shiftLeftButton)  shiftLeftButton->setEnabled(canShiftLeft);
//    if (shiftRightButton) shiftRightButton->setEnabled(canShiftRight);
//}

// Funcții de shift
//void Eter_UI::onShiftUp() {
//    if (!m_game) return;
//    Board& board = m_game->getBoard();
//    if (board.circularShiftUp()) {
//        updateBoardDisplay();
//        checkWinCondition();
//    }
//}
//
//void Eter_UI::onShiftDown() {
//    if (!m_game) return;
//    Board& board = m_game->getBoard();
//    if (board.circularShiftDown()) {
//        updateBoardDisplay();
//        checkWinCondition();
//    }
//}
//
//void Eter_UI::onShiftLeft() {
//    if (!m_game) return;
//    Board& board = m_game->getBoard();
//    if (board.circularShiftLeft()) {
//        updateBoardDisplay();
//        checkWinCondition();
//    }
//}
//
//void Eter_UI::onShiftRight() {
//    if (!m_game) return;
//    Board& board = m_game->getBoard();
//    if (board.circularShiftRight()) {
//        updateBoardDisplay();
//        checkWinCondition();
//    }
//}


void Eter_UI::onCardPlaced(QDropEvent* event, BoardCell* cell) {
    if (!m_game) return;
    CardLabel* card = qobject_cast<CardLabel*>(event->source());
    if (!card) return;

    int row = cell->getRow();
    int col = cell->getCol();
    Board& board = m_game->getBoard();

    QString cardName = card->property("cardName").toString();
    if ((isRedTurn && !cardName.contains("R")) || (!isRedTurn && !cardName.contains("B"))) {
        QMessageBox::warning(this, "Mutare interzisă",
            QString("Nu este rândul jucătorului %1!").arg(isRedTurn ? "roșu" : "albastru"));
        return;
    }

    Card::Value val = charToCardValue(cardName.back().toLatin1());

    if (firstCardPlaced && !board.checkNeighbours(row, col)) {
        QMessageBox::warning(this, "Mutare interzisă",
            "Cartea trebuie plasată adiacent unei cărți existente!");
        return;
    }

    if (!board.checkValue(row, col, val)) {
        QMessageBox::warning(this, "Mutare interzisă",
            "Cartea nu poate fi plasată în acest loc!");
        return;
    }

    bool isIllusion = card->property("isIllusion").toBool();
    Card newCard(val, isRedTurn ? Card::Color::Red : Card::Color::Blue);
    if (isIllusion) {
        newCard.setIllusion();
        QString imagePath = QCoreApplication::applicationDirPath() + "/cards/" +
            (isRedTurn ? "RI.png" : "BI.png");
        cell->setPixmap(QPixmap(imagePath).scaled(100, 150, Qt::KeepAspectRatio));
    }
    else {
        cell->setPixmap(card->pixmap(Qt::ReturnByValue).scaled(100, 150, Qt::KeepAspectRatio));
    }

    board.getBoard()[row][col].push_back(newCard);

    auto checkForCompleteLines = [&]() {
        bool rowComplete = std::all_of(board.getBoard()[row].begin(),
            board.getBoard()[row].end(),
            [](const auto& cell) { return !cell.empty(); });

        bool colComplete = true;
        for (size_t r = 0; r < board.getSize(); r++) {
            if (board.getBoard()[r][col].empty()) {
                colComplete = false;
                break;
            }
        }

        if (rowComplete) {
            bool topEmpty = row > 0 &&
                std::all_of(board.getBoard()[0].begin(),
                    board.getBoard()[0].end(),
                    [](const auto& cell) { return cell.empty(); });
            bool bottomEmpty = row < board.getSize() - 1 &&
                std::all_of(board.getBoard()[board.getSize() - 1].begin(),
                    board.getBoard()[board.getSize() - 1].end(),
                    [](const auto& cell) { return cell.empty(); });

            if (topEmpty) board.circularShiftUp();
            else if (bottomEmpty) board.circularShiftDown();
        }
        else if (colComplete) {
            bool leftEmpty = col > 0 &&
                std::all_of(board.getBoard().begin(),
                    board.getBoard().end(),
                    [](const auto& row) { return row[0].empty(); });
            bool rightEmpty = col < board.getSize() - 1 &&
                std::all_of(board.getBoard().begin(),
                    board.getBoard().end(),
                    [&](const auto& row) { return row[board.getSize() - 1].empty(); });

            if (leftEmpty) board.circularShiftLeft();
            else if (rightEmpty) board.circularShiftRight();
        }
        };

    checkForCompleteLines();
    updateBoardDisplay();

    if (isIllusion) {
        if (isRedTurn) {
            redIllusionPlayed = true;
        }
        else {
            blueIllusionPlayed = true;
        }
    }

    card->hide();
    firstCardPlaced = true;
    isRedTurn = !isRedTurn;
    updateTurnLabel();
    updateCardStacks();
    checkWinCondition();
}


// Se apelează când se plasează o carte pe tablă (drag&drop)
//void Eter_UI::onCardPlaced(QDropEvent* event, BoardCell* cell) {
//    if (!m_game) return;
//
//    CardLabel* card = qobject_cast<CardLabel*>(event->source());
//    if (!card) {
//        QMessageBox::warning(this, "Eroare", "Nu s-a putut plasa cartea.");
//        return;
//    }
//
//    int row = cell->getRow();
//    int col = cell->getCol();
//
//    QString cardName = card->property("cardName").toString();
//    Card::Color cardColor = isRedTurn ? Card::Color::Red : Card::Color::Blue;
//
//    // Verificăm dacă jucătorul are dreptul să plaseze cartea
//    if (isRedTurn && !cardName.contains("R")) {
//        QMessageBox::warning(this, "Mutare interzisă", "Nu este rândul jucătorului roșu!");
//        return;
//    }
//    if (!isRedTurn && !cardName.contains("B")) {
//        QMessageBox::warning(this, "Mutare interzisă", "Nu este rândul jucătorului albastru!");
//        return;
//    }
//
//    Board& board = m_game->getBoard();
//
//    char valChar = cardName.at(cardName.size() - 1).toLatin1();
//    Card::Value val = charToCardValue(valChar);
//
//    // Prima carte poate fi plasată oriunde, apoi doar adiacent
//    if (firstCardPlaced) {
//        if (!board.checkNeighbours(row, col)) {
//            QMessageBox::warning(this, "Mutare interzisă",
//                "Cartea trebuie plasată lângă (adiacent) unei cărți deja existente!");
//            return;
//        }
//    }
//
//    // Verificăm dacă valoarea e permisă acolo
//    if (!board.checkValue(row, col, val)) {
//        QMessageBox::warning(this, "Mutare interzisă",
//            "Cartea nu poate fi plasată în acest loc!");
//        return;
//    }
//
//    // Plasăm cartea propriu-zis în model
//    Card newCard(val, cardColor);
//    board.getBoard()[row][col].push_back(newCard);
//
//    // Setăm imaginea top pe BoardCell
//    cell->setPixmap(card->pixmap(Qt::ReturnByValue).scaled(100, 150, Qt::KeepAspectRatio));
//    card->hide(); // ascundem cartea din "mână"
//
//    if (!firstCardPlaced) {
//        firstCardPlaced = true;
//    }
//
//    // Schimbăm rândul
//    isRedTurn = !isRedTurn;
//    updateTurnLabel();
//    updateCardStacks();
//
//    // Verificăm dacă cineva a câștigat
//    Card::Color winner = board.checkWin();
//    if (winner != Card::Color::Undefined) {
//        showWinMessage(winner);
//    }
//}

// Actualizează eticheta cu cine e la rând
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

// Buton "Wizard and Powers" (din meniu) -> apel direct
void Eter_UI::onWizardPowersClicked() {
    // Apelăm direct OnButtonClick, astfel încât logica să fie aceeași
    OnButtonClick();
}

// Meniu "Tournament game"
void Eter_UI::drawTournamentMenu() {
    isStartPage = false;

    // Ascundem ce era pe ecranul anterior (cu excepția label-urilor)
    for (QObject* child : children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this && widget != turnLabel && widget != scoreLabel) {
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

    // Aici creăm butonul wizardPowers
    createButton(buttonWizardPowers, "Wizard and Powers",
        centerX, centerY + (buttonHeight + spacing) * 3,
        buttonWidth, buttonHeight,
        buttonFont, &Eter_UI::onWizardPowersClicked);
}

// Meniu "Speed game"
void Eter_UI::drawSpeedMenu() {
    isStartPage = false;

    // Ascundem ce era pe ecranul anterior (cu excepția label-urilor)
    for (QObject* child : children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this && widget != turnLabel && widget != scoreLabel) {
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

// Marchează cărțile care aparțin jucătorului curent ca active
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

// Curăță vectorul de cărți de obiecte șterse/ascunse
void Eter_UI::cleanCardStack() {
    cards.erase(
        std::remove_if(cards.begin(), cards.end(), [](CardLabel* c) {
            return !c || c->isHidden();
            }),
        cards.end()
    );
}

// Re-actualizează ce se vede pe tablă după shift
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
                // Atenție: dacă e Eter (val=E), atunci e "RE" sau "BE"
                QString valSuffix = (topCard.getValue() == Card::Value::Eter)
                    ? "E"
                    : QString::number(static_cast<int>(topCard.getValue()));
                QString imagePath = QCoreApplication::applicationDirPath() + "/cards/" + colorPrefix + valSuffix + ".png";
                QPixmap pixmap(imagePath);
                if (!pixmap.isNull()) {
                    cell->setPixmap(pixmap.scaled(100, 150, Qt::KeepAspectRatio));
                }
                else {
                    cell->clear();
                }
            }
        }
    }
}

// Afișează o carte de "power" într-un label lateral
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

// Exemplu: activarea unei puteri de vrăjitor
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

// Începe un nou tur (resetează tabla și reafișează cărțile, scorul rămâne)
void Eter_UI::startNewTurn() {
    if (!m_game) return;

    // Curățăm tabla
    Matrix<Card>& boardData = m_game->getBoard().getBoard();
    for (auto& row : boardData) {
        row.clear(); // sau iterezi și dai row[i].clear()
        row.resize(m_game->getBoard().getSize());
        // Asigurându-ne că rămâne mărimea inițială, dar fără cărți
    }

    m_game->getBoard().m_firstCardPlayed = false;

    // Ștergem cărțile existente
    cleanCardStack();
    // Reconstruim "mâinile" jucătorilor
    createCards(nullptr);  // e un artificiu (poți adapta să chemi altă funcție)

    isRedTurn = true;
    firstCardPlaced = false;

    updateTurnLabel();
    updateBoardDisplay();
    updateCardStacks();

    QMessageBox::information(this, "New Turn", "A new turn has started!");
}

// Afișează mesajul când cineva câștigă runda / meciul
void Eter_UI::showWinMessage(Card::Color winner) {
    QString winnerText = (winner == Card::Color::Red) ? "Red Player" : "Blue Player";

    // Dacă e unul din modurile de duel/training (best of 3 sau best of 5)
    if (m_game &&
        (m_game->getGameType() == Game::GameType::Training ||
            m_game->getGameType() == Game::GameType::WizardDuel ||
            m_game->getGameType() == Game::GameType::PowerDuel ||
            m_game->getGameType() == Game::GameType::WizardAndPowerDuel))
    {
        if (winner == Card::Color::Blue) {
            m_blueScore++;
        }
        else if (winner == Card::Color::Red) {
            m_redScore++;
        }

        updateScoreLabel();

        // În Training facem Best of 3, în rest se poate face Best of 5, 
        // dar poți adapta cum vrei
        int winningThreshold = (m_game->getGameType() == Game::GameType::Training) ? 2 : 3;

        if (m_blueScore == winningThreshold) {
            QMessageBox::information(
                this,
                "Game Over",
                QString("Blue Player wins the Best of %1!").arg(winningThreshold * 2 - 1)
            );
            m_blueScore = 0;
            m_redScore = 0;
            updateScoreLabel();
            return;
        }
        else if (m_redScore == winningThreshold) {
            QMessageBox::information(
                this,
                "Game Over",
                QString("Red Player wins the Best of %1!").arg(winningThreshold * 2 - 1)
            );
            m_blueScore = 0;
            m_redScore = 0;
            updateScoreLabel();
            return;
        }

        // Continuăm meciul până se atinge scorul
        QMessageBox::information(
            this,
            "Match Over",
            QString("%1 has won this match! Next match starts...").arg(winnerText)
        );
        startNewTurn();
    }
    else {
        // Altfel, e un singur joc, anunțăm și resetăm
        QMessageBox::information(
            this,
            "Game Over",
            QString("%1 has won the game! Starting a new turn...").arg(winnerText)
        );
        startNewTurn();
    }
}

// Actualizează eticheta de scor
void Eter_UI::updateScoreLabel() {
    if (!scoreLabel || !m_game) return;

    QString mode = "Training";
    if (m_game->getGameType() == Game::GameType::WizardDuel) {
        mode = "Wizard Duel";
    }
    else if (m_game->getGameType() == Game::GameType::PowerDuel) {
        mode = "Powers Duel";
    }
    else if (m_game->getGameType() == Game::GameType::WizardAndPowerDuel) {
        mode = "Wizard & Powers Duel";
    }

    scoreLabel->setText(QString("Mode: %1 | Score: Blue %2 - %3 Red")
        .arg(mode)
        .arg(m_blueScore)
        .arg(m_redScore)
    );

    scoreLabel->adjustSize();
    scoreLabel->move(width() - scoreLabel->width() - 20, 20);
}

// Metodă ajutătoare, dacă vrei să ștergi o carte, etc.
void Eter_UI::removeCard(CardLabel* card) {
    cards.removeOne(card);
    card->deleteLater();

    isRedTurn = !isRedTurn;
    updateTurnLabel();
    checkWinCondition();
}

// Verifică dacă s-a câștigat jocul
void Eter_UI::checkWinCondition() {
    if (!m_game) return;
    Card::Color winner = m_game->getBoard().checkWin();
    if (winner != Card::Color::Undefined) {
        showWinMessage(winner);
    }
}
