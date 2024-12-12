#include "models/Match.h"

int main() {
    Match::getInstance(
        Match::MatchType::Normal,
        Game::GameType::WizardAndPowerDuel,
        false,
        false).runMatch();
}
