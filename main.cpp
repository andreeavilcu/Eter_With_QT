#include <random>
#include "models/Player.h"

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> wizardDistribution{ 0, Wizard::wizard_count };
    std::uniform_int_distribution<size_t> powerDistribution{ 0, Power::power_count };

    Player player{ {}, wizardDistribution(gen) };
    player.playWizard();

    std::pair<size_t, size_t> powerIndices = { powerDistribution(gen), powerDistribution(gen) };
    Player player2{ {}, wizardDistribution(gen), powerIndices };
    player2.playPower();
}
