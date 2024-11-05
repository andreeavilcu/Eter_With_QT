#include "models/Game.h"

int main() {
    Game<GridSize::Three>::getInstance().run(GameType::WizardDuel);
}
