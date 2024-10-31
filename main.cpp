#include "models/Wizard.h";

int main()
{
    srand(time(NULL));

    Wizard::play(rand() % eter::wizard_count);
}
