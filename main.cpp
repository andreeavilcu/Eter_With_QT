#include <random>
#include "models/Player.h"
#include "models/Game.h"

int main() {
    std::random_device rd;
    std::mt19937 gen{ rd() };

    std::uniform_int_distribution<size_t> wizardDistribution{ 0, Wizard::wizard_count };
    std::uniform_int_distribution<size_t> powerDistribution{ 0, Power::power_count };

    Player player{ {}, wizardDistribution(gen), { powerDistribution(gen), powerDistribution(gen) } };
    player.playWizard();
    player.playPower(true);
    player.playPower(false);

    Game& game = Game::getInstance();
}
