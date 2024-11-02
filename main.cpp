#include <random>

#include "models/Player.h"

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> wizardDistribution{0, Wizard::wizard_count};

    Player player{{}, wizardDistribution(gen)};
    player.playWizard();


}
