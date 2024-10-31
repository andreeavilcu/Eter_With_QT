#include "models/Player.h"
#include "models/cards/Wizard.h"

int main()
{
    srand(time(NULL));
    //Comm
    Wizard w{ static_cast<size_t>(rand() % eter::wizard_count) };
    w.playWizard();

    Player player{ static_cast<size_t>(rand() % eter::power_count) };
    player.get_playing_cards();
    player.get_powers();
    player.get_wizard();
    player.move();
}
