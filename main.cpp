#include "models/Game.h"

int main() {
    Game<GridSize::Three, GameType::WizardDuel>::getInstance().run();
}
