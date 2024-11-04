#include <random>
#include "models/Player.h"
// #include "models/Game.h"
#include "models/Board.h"

int main() {
    std::random_device rd;
    std::mt19937 gen{ rd() };

    std::uniform_int_distribution<size_t> wizardDistribution{ 0, Wizard::wizard_count };
    std::uniform_int_distribution<size_t> powerDistribution{ 0, Power::power_count };

    Player player{ {}, wizardDistribution(gen), { powerDistribution(gen), powerDistribution(gen) } };
    player.playWizard();
    player.playPower(true);
    player.playPower(false);

    // Game<3>& game = Game<3>::getInstance();

    Board<3> b;
    b.placeCard(0, 0);
    b.swapRow(0, 2);
    b.swapCol(0, 2);
    b.printTest();
}
