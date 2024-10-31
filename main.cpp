#include "models/Player.h"

int main()
{
    Player player({}, rand() % Wizard::wizard_count);
    player.playWizard();
}
