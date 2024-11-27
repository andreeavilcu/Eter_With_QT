#include "models/Game.h"

int main() {

    srand(time(NULL));

    Game game{ Game::GameType::Training };
    game.run();
}
