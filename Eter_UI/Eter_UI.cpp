#include "Eter_UI.h"
#include <QPainter>
#include <QLinearGradient>

Eter_UI::Eter_UI(QWidget* parent)
    : QMainWindow(parent), isStartPage(true) {
    ui.setupUi(this);

    // Maximizăm fereastra
    this->showMaximized();

    const int buttonWidth = 150;
    const int buttonHeight = 75;
    const int spacing = 10;

    // Configurare butoane
    buttonTraning = new QPushButton("Traning mode", this);
    buttonTraning->setGeometry(100, 300, buttonWidth, buttonHeight);
    buttonTraning->show();
    connect(buttonTraning, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonWizard = new QPushButton("Wizards duel", this);
    buttonWizard->setGeometry(100, 300 + buttonHeight + spacing, buttonWidth, buttonHeight);
    buttonWizard->show();
    connect(buttonWizard, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonPowers = new QPushButton("Powers duel", this);
    buttonPowers->setGeometry(100, 300 + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    buttonPowers->show();
    connect(buttonPowers, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonTurnament = new QPushButton("Turnament", this);
    buttonTurnament->setGeometry(100, 300 + 3 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    buttonTurnament->show();
    connect(buttonTurnament, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);

    buttonTimed = new QPushButton("Timed duel", this);
    buttonTimed->setGeometry(100, 300 + 4 * (buttonHeight + spacing), buttonWidth, buttonHeight);
    buttonTimed->show();
    connect(buttonTimed, &QPushButton::clicked, this, &Eter_UI::OnButtonClick);
}


Eter_UI::~Eter_UI()
{
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
        if (isStartPage) {
            QRect centerRect(width() / 2 - 150, height() / 4 - 75, 400, 200);
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

    const int cardWidth = 100;
    const int cardHeight = 150;
    const int cardSpacing = 5;
    QString cardsPath = QCoreApplication::applicationDirPath() + "/cards/";

    if (!QDir(cardsPath).exists()) {
        qDebug() << "Folderul 'cards' nu există la:" << cardsPath;
        return;
    }

    int blueX, blueY, redX, redY;

    QStringList blueCards, redCards;

    if (clickedButton == buttonTraning) {
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
    else if (clickedButton == buttonTurnament || clickedButton == buttonTimed) {
        // Nu implementam nimic inca
        return;
    }

    blueX = this->width() / 4 - cardWidth / 2;
    blueY = 50;

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
        blueX += cardWidth + cardSpacing;
    }

    redX = this->width() / 4 - cardWidth / 2;
    redY = this->height() - cardHeight - 50;

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
        redX += cardWidth + cardSpacing;
    }

    // Reîmprosptăm interfața
    update();
}