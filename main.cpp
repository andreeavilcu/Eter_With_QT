#include "models/Match.h"

int main() {
    Match::getInstance(
        Match::MatchType::Normal,
        false,
        Game::GameType::Training,
        false,
        false).runMatch();
}
