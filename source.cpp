#include "models/GameMechanics/Match.h"

constexpr auto load = false;

//test

auto matchType = Match::MatchType::Tournament;
auto timerDuration = Match::TimerDuration::Short;
auto gameType = Game::GameType::WizardAndPowerDuel;

bool illusions = true;
bool explosion = true;

int main() {
    if (load) Match::run("SavedGames/test.json");
    else Match::run(
            matchType,
            timerDuration,
            gameType,
            illusions,
            explosion
        );

    return 0;
}
