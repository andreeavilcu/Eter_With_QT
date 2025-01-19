#include "Eter_UI.h"
#include <QPainter>
#include <QLinearGradient>
#include "BoardCell.h"
#include "CardLabel.h"
#include <QScreen>

bool Eter_UI::isValidMove(size_t row, size_t col, Card::Value cardValue)
{
    if(!m_game) return false;

    return m_game->getBoard().checkPartial(row, col, static_cast<size_t>(cardValue));
}

void Eter_UI::createButton(QPointer<QPushButton>& button, const QString& text, int x, int y, int width, int height, const QFont& font, void (Eter_UI::* slot)()) {
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

    createButton(buttonTraining, "Training mode", centerX, centerY - yOffset, buttonWidth, buttonHeight, buttonFont, &Eter_UI::OnButtonClick);
    createButton(buttonWizard, "Wizards duel", centerX, centerY + spacing + buttonHeight - yOffset, buttonWidth, buttonHeight, buttonFont, &Eter_UI::OnButtonClick);
    createButton(buttonPowers, "Powers duel", centerX, centerY + (spacing + buttonHeight) * 2 - yOffset, buttonWidth, buttonHeight, buttonFont, &Eter_UI::OnButtonClick);
    createButton(buttonTournament, "Tournament game", centerX, centerY + (spacing + buttonHeight) * 3 - yOffset, buttonWidth, buttonHeight, buttonFont, &Eter_UI::drawTournamentMenu);
    createButton(buttonSpeed, "Speed game", centerX, centerY + (spacing + buttonHeight) * 4 - yOffset, buttonWidth, buttonHeight, buttonFont, &Eter_UI::drawSpeedMenu);
    //createButton(buttonWizardPowers, "Wizards and Powers", centerX, centerY + (spacing + buttonHeight) * 5 - yOffset, buttonWidth, buttonHeight, buttonFont, &Eter_UI::onWizardPowersClicked);
}

Eter_UI::Eter_UI(QWidget* parent)
    : QMainWindow(parent), isStartPage(true), isRedTurn(false), m_game(nullptr), gameBoard(nullptr), boardLayout(nullptr) {
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
    if (gameBoard) delete gameBoard;
}

void Eter_UI::paintEvent(QPaintEvent* event) {
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
            logoRect = QRect(width() / 2 - logo.width() / 2, height() / 4 - 150, logo.width(), logo.height());
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
            // Adaugă conexiunea aici
            connect(cell, &BoardCell::cardPlaced, this, &Eter_UI::onCardPlaced);
            boardLayout->addWidget(cell, i, j);
            boardCells.append(cell);
        }
    }

    int widgetSize = boardSize * cellSize;
    boardWidget->setLayout(boardLayout);
    boardWidget->setGeometry(width() / 2 - widgetSize / 2, height() / 2 - widgetSize / 2, widgetSize, widgetSize);
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
        blueCards = { "Bcard1", "Bcard1", "Bcard2", "Bcard2", "Bcard3", "Bcard3", "Bcard4" };
        redCards = { "Rcard1", "Rcard1", "Rcard2", "Rcard2", "Rcard3", "Rcard3", "Rcard4" };
    }
    else if (clickedButton == buttonWizard) {
        blueCards = { "Bcard1", "Bcard1", "Bcard2", "Bcard2", "Bcard2", "Bcard3", "Bcard3", "Bcard3", "Bcard4", "BcardE" };
        redCards = { "Rcard1", "Rcard1", "Rcard2", "Rcard2", "Rcard2", "Rcard3", "Rcard3", "Rcard3", "Rcard4", "RcardE" };
    }
    else if (clickedButton == buttonPowers || clickedButton == buttonWizardPowers) {
        blueCards = { "Bcard1", "Bcard2", "Bcard2", "Bcard2", "Bcard3", "Bcard3", "Bcard3", "Bcard4", "BcardE" };
        redCards = { "Rcard1", "Rcard2", "Rcard2", "Rcard2", "Rcard3", "Rcard3", "Rcard3", "Rcard4", "RcardE" };
    }
    else {
        return;
    }

    auto createCard = [&](const QString& cardName, int& startX, int startY, const QColor& color) {
        QString imagePath = cardsPath + cardName + ".png";
        if (!QFile::exists(imagePath)) {
            qDebug() << "Error: Image not found:" << imagePath;
            return;
        }

        CardLabel* card = new CardLabel(imagePath, charToCardValue(cardName[cardName.size() - 1].unicode()), this);
        card->setGeometry(startX, startY, 100, 150);
        connect(card, &CardLabel::cardMoved, this, &Eter_UI::removeCard);
        cards.append(card);
        card->show();
        startX += 110;
        };

    int startXBlue = width() / 2 - ((blueCards.size() / 2) * 110);
    int startYBlue = height() / 2 + 230;
    for (const QString& cardName : blueCards) {
        createCard(cardName, startXBlue, startYBlue, Qt::blue);
    }

    int startXRed = width() / 2 - ((redCards.size() / 2) * 110);
    int startYRed = height() / 2 - 340;
    for (const QString& cardName : redCards) {
        createCard(cardName, startXRed, startYRed, Qt::red);
    }
}

Card::Value Eter_UI::charToCardValue(char value) {
    switch (value) {
    case '1':
        return Card::Value::One;
    case '2':
        return Card::Value::Two;
    case '3':
        return Card::Value::Three;
    case '4':
        return Card::Value::Four;
    case 'E':
        return Card::Value::Eter;
    ;
    }
}

void Eter_UI::OnButtonClick() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    isStartPage = false;

    // Determină tipul jocului bazat pe butonul apăsat
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

    // Inițializează jocul
    std::pair<size_t, size_t> wizardIndices = { -1, -1 }; // sau generează random
    bool illusions = false; // setează după preferință
    bool explosion = false; // setează după preferință
    m_game = std::make_unique<Game>(gameType, wizardIndices, illusions, explosion, false);


    for (QObject* child : children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this && widget != turnLabel) {
                widget->hide();
                widget->deleteLater();
            }
        }
    }

    createBoard(clickedButton);
    createCards(clickedButton);

    turnLabel->setGeometry(width() / 2 - 100, 50, 200, 40);
    turnLabel->show();
    updateTurnLabel();
    update();
}

void Eter_UI::updateTurnLabel() {
    if (turnLabel) {
        turnLabel->setText(isRedTurn ? "Red Player's Turn" : "Blue Player's Turn");
        turnLabel->setStyleSheet(QString("color: white; background-color: %1; padding: 5px;")
            .arg(isRedTurn ? "rgba(255, 0, 0, 128)" : "rgba(0, 0, 255, 128)"));
        turnLabel->adjustSize();
        turnLabel->move(width() - turnLabel->width() - 20, 20);
    }
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



void Eter_UI::onCardPlaced(QDropEvent* event, BoardCell* cell)
{
    if (!m_game) return;

    int row = -1, col = -1;
    for (int i = 0; i < boardCells.size(); ++i) {
        if (boardCells[i] == cell) {
            row = i / m_game->getBoard().getSize();
            col = i % m_game->getBoard().getSize();
            break;
        }
    }

    if (row == -1 || col == -1) return;

    // Obține valoarea cărții din datele MIME
    Card::Value cardValue = static_cast<Card::Value>(event->mimeData()->data("card-value").toInt());

    if (isRedTurn) {
        processCardPlacement(m_game->getPlayer1(), row, col, cardValue);
    }
    else {
        processCardPlacement(m_game->getPlayer2(), row, col, cardValue);
    }

    isRedTurn = !isRedTurn;
    updateTurnLabel();

    Card::Color winner = m_game->getBoard().checkWin();
    if (winner != Card::Color::Undefined) {
        showWinMessage(winner);
    }
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
    for (QObject* child : this->children()) {
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

    createButton(buttonTraining, "Training Mode", centerX, centerY, buttonWidth, buttonHeight, buttonFont, &Eter_UI::OnButtonClick);
    createButton(buttonWizard, "Wizard Mode", centerX, centerY + buttonHeight + spacing, buttonWidth, buttonHeight, buttonFont, &Eter_UI::OnButtonClick);
    createButton(buttonPowers, "Powers Mode", centerX, centerY + (buttonHeight + spacing) * 2, buttonWidth, buttonHeight, buttonFont, &Eter_UI::OnButtonClick);
    createButton(buttonWizardPowers, "Wizard and Powers", centerX, centerY + (buttonHeight + spacing) * 3, buttonWidth, buttonHeight, buttonFont, &Eter_UI::onWizardPowersClicked);
}


void Eter_UI::drawSpeedMenu() {
    isStartPage = false; 
    for (QObject* child : this->children()) {
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

    createButton(buttonTraining, "Training Mode", centerX, centerY, buttonWidth, buttonHeight, buttonFont, &Eter_UI::OnButtonClick);
    createButton(buttonWizard, "Wizard Mode", centerX, centerY + buttonHeight + spacing, buttonWidth, buttonHeight, buttonFont, &Eter_UI::OnButtonClick);
    createButton(buttonPowers, "Powers Mode", centerX, centerY + (buttonHeight + spacing) * 2, buttonWidth, buttonHeight, buttonFont, &Eter_UI::OnButtonClick);
    createButton(buttonWizardPowers, "Wizard and Powers", centerX, centerY + (buttonHeight + spacing) * 3, buttonWidth, buttonHeight, buttonFont, &Eter_UI::onWizardPowersClicked);

      ///to do: + meniu de tunament
}
