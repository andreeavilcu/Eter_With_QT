#include "models/GameMechanics/Match.h"

int main() {
    Match::getInstance(
        Match::MatchType::Tournament,
        false,
        Game::GameType::Training,
        false,
        false).runMatch();
}
