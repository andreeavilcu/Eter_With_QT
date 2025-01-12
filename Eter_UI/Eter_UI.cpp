#include "Eter_UI.h"

Eter_UI::Eter_UI(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Dimensiunile butoanelor
    const int buttonWidth = 150;
    const int buttonHeight = 75;
    const int spacing = 10; // Spatiu între butoane

    // Calculam poztiile pentru centrare
    int xPos = (this->width() - buttonWidth) / 2;

    buttonTraning = new QPushButton("Traning mode", this);
    buttonTraning->setGeometry(xPos, 50, buttonWidth, buttonHeight);///deci pun butonul pe ecran; (0,0) sunt in coltul din stanga al consolei; parm. 3 si 4 sunt pt marimeau butonului
    buttonTraning->show();
    connect(buttonTraning, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);///ii zic "butonului"(adica obiectului) ca la actiunea de click sa se apeleze acea functie si sa se puna pe window ul curent (adica this)


    buttonWizard = new QPushButton("Wizards duel", this);
    buttonWizard->setGeometry(xPos, 50 + buttonHeight + spacing, buttonWidth, buttonHeight);
    buttonWizard->show();

    buttonPowers = new QPushButton("Powers duel", this);
    buttonPowers->setGeometry(xPos, 50 + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    buttonPowers->show();

    buttonTurnament = new QPushButton("Turnament", this);
    buttonTurnament->setGeometry(xPos, 50 + 3 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    buttonTurnament->show();

    buttonTimed = new QPushButton("Timed duel", this);
    buttonTimed->setGeometry(xPos, 50 + 4 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    buttonTimed->show();
}

Eter_UI::~Eter_UI()
{}

void Eter_UI::OnButtonClick() {

    // stergem butoanele
    for (QObject* child : this->children()) {
        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
            if (widget != buttonTraning && widget != buttonWizard && widget != buttonPowers && widget != buttonTurnament && widget != buttonTimed) {
                delete widget;
            }
        }
    }

    // dimensiuni pt carti
    const int cardWidth = 100;
    const int cardHeight = 150;
    const int cardSpacing = 10;

    // pozzitii pt carti
    int redX = 50;
    int redY = 50;
    int blueX = this->width() - cardWidth - 50;
    int blueY = 50;

    // cale foldor cards
    QString cardsPath = QCoreApplication::applicationDirPath() + "/cards/";

    if (!QDir(cardsPath).exists()) {
        qDebug() << "Folderul cards nu există la:" << cardsPath;
        return;
    }

    // redCards
    QStringList redCards = { "Rcard1", "Rcard1", "Rcard2", "Rcard2", "Rcard3", "Rcard3", "Rcard4" };
    for (const QString& cardName : redCards) {
        QString imagePath = cardsPath + cardName + ".png";
        QPixmap pixmap(imagePath);
        if (pixmap.isNull()) {
            qDebug() << "Eroare: Nu s-a putut încărca imaginea:" << imagePath;
            continue;
        }
        QLabel* label = new QLabel(this);
        label->setPixmap(pixmap.scaled(cardWidth, cardHeight, Qt::KeepAspectRatio));
        label->setGeometry(redX, redY, cardWidth, cardHeight);
        label->show();
        redY += cardHeight + cardSpacing;
    }

    // blueCards
    QStringList blueCards = { "Bcard1", "Bcard1", "Bcard2", "Bcard2", "Bcard3", "Bcard3", "Bcard4" };
    for (const QString& cardName : blueCards) {
        QString imagePath = cardsPath + cardName + ".png";
        QPixmap pixmap(imagePath);
        if (pixmap.isNull()) {
            qDebug() << "Eroare: Nu s-a putut încărca imaginea:" << imagePath;
            continue;
        }
        QLabel* label = new QLabel(this);
        label->setPixmap(pixmap.scaled(cardWidth, cardHeight, Qt::KeepAspectRatio));
        label->setGeometry(blueX, blueY, cardWidth, cardHeight);
        label->show();
        blueY += cardHeight + cardSpacing;
    }
}


