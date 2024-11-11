#include "models/Game.h"

int main() {

    srand(time(NULL));

    Game game{ Game::GameType::WizardAndPowerDuel };
    game.run();
}
