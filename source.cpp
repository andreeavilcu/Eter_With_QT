#include "models/GameMechanics/Match.h"

bool load = true;

int main() {
    if (!load) {
        Match::run(
        Match::MatchType::Tournament,
        Match::TimerDuration::Short,
        Game::GameType::WizardAndPowerDuel,
        true,
        true);

        return 0;
    }

    std::ifstream configFile("SavedGames/test.json");

    if (!configFile.is_open()) {
        std::cerr << "Failed to open config file" << std::endl;
        return 1;
    }

    nlohmann::json config;

    try {
        configFile >> config;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Eroare de parsare: " << e.what() << std::endl;
        return 1;
    }

    Match::run(config);

    return 0;
}
