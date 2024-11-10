#include "models/Game.h"

int main() {

    srand(time(NULL));

    Game<GameType::WizardDuel>::getInstance().playExplosion();//run();
}
