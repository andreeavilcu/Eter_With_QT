#include "Eter_UI.h"
#include <QPainter>
#include <QLinearGradient>
#include "BoardCell.h"
#include "CardLabel.h"
#include <QScreen>


Eter_UI::Eter_UI(QWidget* parent)
    : QMainWindow(parent), isStartPage(true) {
    ui.setupUi(this);

    
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        this->setGeometry(screenGeometry);
    }
    QFont buttonFont;
    buttonFont.setPointSize(16);
    buttonFont.setBold(true);

    const int buttonWidth = 200;
    const int buttonHeight = 100;
    const int spacing = 10;
    const int yOffset = 60;

    ///drawButton((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 - yOffset, buttonWidth, buttonHeight, "Training mode");
    //// Configurare butoane
    buttonTraining = new QPushButton("Traning mode", this);
    buttonTraining->setFont(buttonFont); 
    buttonTraining->setGeometry((this->width() - buttonWidth) / 2,  (this->height() - buttonHeight) / 2 -yOffset, buttonWidth, buttonHeight);
    buttonTraining->show();
    connect(buttonTraining, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonWizard = new QPushButton("Wizards duel", this);
    buttonWizard->setFont(buttonFont);
    buttonWizard->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing+buttonHeight) - yOffset, buttonWidth, buttonHeight);
    buttonWizard->show();
    connect(buttonWizard, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonPowers = new QPushButton("Powers duel", this);
    buttonPowers->setFont(buttonFont);
    buttonPowers->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing + buttonHeight)*2 -yOffset, buttonWidth, buttonHeight);
    buttonPowers->show();
    connect(buttonPowers, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonTournament = new QPushButton("Tournament game", this);
    buttonTournament->setFont(buttonFont);
    buttonTournament->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing + buttonHeight)*3 -yOffset, buttonWidth, buttonHeight);
    buttonTournament->show();
    connect(buttonTournament, &QPushButton::clicked, this, &Eter_UI::drawTournamentMenu);

    buttonSpeed = new QPushButton("Speed game", this);
    buttonSpeed->setFont(buttonFont);
    buttonSpeed->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing + buttonHeight)*4 -yOffset, buttonWidth, buttonHeight);
    buttonSpeed->show();
    connect(buttonSpeed, &QPushButton::clicked, this, &Eter_UI::drawSpeedMenu);

}
Eter_UI::~Eter_UI() {}

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
        if (isStartPage) {
            QRect centerRect(this->width() / 2 - logo.width()/2, this->height() / 4 - 150, logo.width(), logo.height());
            painter.drawPixmap(centerRect, logo);
        }
        else {
            QRect topLeftRect(10, 10, 150, 75);
            painter.drawPixmap(topLeftRect, logo);
        }
    }
    else {
        qDebug() << "Eroare: Nu s-a putut încărca imaginea:" << logoPath;
    }
}

void Eter_UI::createBoard() {
    gameBoard = new Board(3);
    QWidget* boardWidget = new QWidget(this);
    boardLayout = new QGridLayout(boardWidget);
    boardLayout->setSpacing(0);  
    boardLayout->setContentsMargins(0, 0, 0, 0);  

    const int cellSize = 100;  

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            BoardCell* cell = new BoardCell(this);
            cell->setAcceptDrops(true);
            cell->setFixedSize(cellSize, cellSize);  
            boardLayout->addWidget(cell, i, j);
            boardCells.append(cell);
        }
    }

    boardWidget->setLayout(boardLayout);
    boardWidget->setGeometry(this->width() / 2 - 150, this->height() / 2 - 120, 300, 300);
    boardWidget->show();
}

void Eter_UI::createCards(QPushButton* clickedButton) {
    QString cardsPath = QCoreApplication::applicationDirPath() + "/cards/";

    if (!QDir(cardsPath).exists()) {
        qDebug() << "Folderul 'cards' nu există la:" << cardsPath;
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
    else if (clickedButton == buttonPowers) {
        blueCards = { "Bcard1", "Bcard2", "Bcard2", "Bcard2", "Bcard3", "Bcard3", "Bcard3", "Bcard4", "BcardE" };
        redCards = { "Rcard1", "Rcard2", "Rcard2", "Rcard2", "Rcard3", "Rcard3", "Rcard3", "Rcard4", "RcardE" };
    }
    else {
        return;
    }

    int startXBlue = this->width() / 2 - ((blueCards.size() / 2) * 110);
    int startYBlue = this->height() / 2 + 190;

    int startXRed = this->width() / 2 - ((redCards.size() / 2) * 110);
    int startYRed = this->height() / 2 - 300;

    for (const QString& cardName : redCards) {
        QString imagePath = cardsPath + cardName + ".png";
        if (!QFile::exists(imagePath)) {
            qDebug() << "Eroare: Nu s-a găsit imaginea:" << imagePath;
            continue;
        }

        CardLabel* card = new CardLabel(imagePath, this);
        card->setGeometry(startXRed, startYRed, 100, 150);
        connect(card, &CardLabel::cardMoved, this, &Eter_UI::removeCard);
        cards.append(card);
        card->show();
        startXRed += 110;
    }

    for (const QString& cardName : blueCards) {
        QString imagePath = cardsPath + cardName + ".png";
        if (!QFile::exists(imagePath)) {
            qDebug() << "Eroare: Nu s-a găsit imaginea:" << imagePath;
            continue;
        }

        CardLabel* card = new CardLabel(imagePath, this);
        card->setGeometry(startXBlue, startYBlue, 100, 150);
        connect(card, &CardLabel::cardMoved, this, &Eter_UI::removeCard);
        cards.append(card);
        card->show();
        startXBlue += 110;
    }
}
void Eter_UI::removeCard(CardLabel* card) {
    cards.removeOne(card); 
    card->deleteLater();   
}
void Eter_UI::OnButtonClick() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    isStartPage = false;

    for (QObject* child : this->children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this) {
                widget->hide();
                widget->deleteLater();
            }
        }
    }

    createBoard();

    createCards(clickedButton);
    update();
}

void Eter_UI::drawTournamentMenu()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    isStartPage = false;

    for (QObject* child : this->children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this) {
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
    const int yOffset = 60;

    buttonTraining = new QPushButton("Training mode", this);
    buttonTraining->setFont(buttonFont);
    buttonTraining->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 - yOffset, buttonWidth, buttonHeight);
    buttonTraining->show();
    connect(buttonTraining, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonWizard = new QPushButton("Wizards duel", this);
    buttonWizard->setFont(buttonFont);
    buttonWizard->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing + buttonHeight) - yOffset, buttonWidth, buttonHeight);
    buttonWizard->show();
    connect(buttonWizard, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonPowers = new QPushButton("Powers duel", this);
    buttonPowers->setFont(buttonFont);
    buttonPowers->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing + buttonHeight) * 2 - yOffset, buttonWidth, buttonHeight);
    buttonPowers->show();
    connect(buttonPowers, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);
   
    ///to do: de declarat
    buttonWizardPowers = new QPushButton("Wizards and powers", this);
    buttonWizardPowers->setFont(buttonFont);
    buttonWizardPowers->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing + buttonHeight) *3 - yOffset, buttonWidth, buttonHeight);
    buttonWizardPowers-> show();
    connect(buttonWizardPowers, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);
}
void Eter_UI::drawSpeedMenu()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    isStartPage = false;

    for (QObject* child : this->children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != this) {
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
    const int yOffset = 60;

    buttonTraining = new QPushButton("Training mode", this);
    buttonTraining->setFont(buttonFont);
    buttonTraining->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 - yOffset, buttonWidth, buttonHeight);
    buttonTraining->show();
    connect(buttonTraining, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonWizard = new QPushButton("Wizards duel", this);
    buttonWizard->setFont(buttonFont);
    buttonWizard->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing + buttonHeight) - yOffset, buttonWidth, buttonHeight);
    buttonWizard->show();
    connect(buttonWizard, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonPowers = new QPushButton("Powers duel", this);
    buttonPowers->setFont(buttonFont);
    buttonPowers->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing + buttonHeight) * 2 - yOffset, buttonWidth, buttonHeight);
    buttonPowers->show();
    connect(buttonPowers, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    ///to do: de declarat
    buttonWizardPowers = new QPushButton("Wizards and powers", this);
    buttonWizardPowers->setFont(buttonFont);
    buttonWizardPowers->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing + buttonHeight) * 3 - yOffset, buttonWidth, buttonHeight);
    buttonWizardPowers->show();
    connect(buttonWizardPowers, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    ///to do: de declarat
    buttonTournament = new QPushButton("Tournament duel", this);
    buttonTournament->setFont(buttonFont);
    buttonTournament->setGeometry((this->width() - buttonWidth) / 2, (this->height() - buttonHeight) / 2 + (spacing + buttonHeight) * 4 - yOffset, buttonWidth, buttonHeight);
    buttonTournament->show();
    connect(buttonTournament, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);
}
