#include "models/GameMechanics/Match.h"

int main() {
    Match::getInstance(
        Match::MatchType::Tournament,
        Match::TimerDuration::Short,
        Game::GameType::WizardAndPowerDuel,
        true,
        true).runMatch();
}
