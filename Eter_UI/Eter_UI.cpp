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
    buttonTraning->setText("");
}
