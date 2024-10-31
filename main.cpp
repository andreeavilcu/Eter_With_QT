#include "models/Player.h"

int main()
{
    srand(time(NULL));

    Player player({}, rand() % Wizard::wizard_count);
    player.playWizard();
}
