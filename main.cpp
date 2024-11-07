#include "models/Game.h"

int main() {
    Game<GameType::Training>::getInstance().run();
}
