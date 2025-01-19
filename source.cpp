#include "models/GameMechanics/Match.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <regex>

namespace fs = std::filesystem;

void startNewGame() {
    auto matchType = Match::MatchType::Tournament;
    auto timerDuration = Match::TimerDuration::Short;
    auto gameType = Game::GameType::WizardAndPowerDuel;

    bool illusions;
    bool explosion;

    int matchTypeInput;
    std::cout << "Select Match Type:\n1. Normal\n2. Tournament\nChoice: ";
    std::cin >> matchTypeInput;
    matchType = static_cast<Match::MatchType>(matchTypeInput - 1);

    int timerInput;
    std::cout << "Select Timer Duration:\n1. Untimed\n2. Short\n3. Medium\n4. Long\nChoice: ";
    std::cin >> timerInput;

    if (timerInput == 1) timerDuration = Match::TimerDuration::Untimed;
    else if (timerInput == 2) timerDuration = Match::TimerDuration::Short;
    else if (timerInput == 3) timerDuration = Match::TimerDuration::Medium;
    else if (timerInput == 4) timerDuration = Match::TimerDuration::Long;

    int gameTypeInput;
    std::cout << "Select Game Type:\n1. Training\n2. WizardDuel\n3. PowerDuel\n4. WizardAndPowerDuel\nChoice: ";
    std::cin >> gameTypeInput;
    gameType = static_cast<Game::GameType>(gameTypeInput - 1);

    std::cout << "Enable illusions? (1 for Yes, 0 for No): ";
    std::cin >> illusions;

    std::cout << "Enable explosions? (1 for Yes, 0 for No): ";
    std::cin >> explosion;

    Match::run(matchType, timerDuration, gameType, illusions, explosion);
}

void loadSavedGame() {
    const std::string saveDirectory = "SavedGames";

    if (!fs::exists(saveDirectory)) {
        std::cerr << "No SavedGames folder found. Exiting.\n";
        return;
    }

    std::vector<std::string> saveFiles;
    const std::regex filePattern("backup_\\d{4}-\\d{2}-\\d{2}_\\d{2}-\\d{2}-\\d{2}\\.json");

    for (const auto &entry : fs::directory_iterator(saveDirectory)) {
        if (entry.is_regular_file() && std::regex_match(entry.path().filename().string(), filePattern)) {
            saveFiles.push_back(entry.path().string());
        }
    }

    if (saveFiles.empty()) {
        std::cerr << "No saved games found in the directory. Exiting.\n";
        return;
    }

    std::cout << "Saved games available:\n";
    for (size_t i = 0; i < saveFiles.size(); ++i) {
        std::cout << i + 1 << ". " << saveFiles[i] << "\n";
    }

    size_t choice;
    std::cout << "Select a saved game to load (1 - " << saveFiles.size() << "): ";
    std::cin >> choice;

    if (choice < 1 || choice > saveFiles.size()) {
        std::cerr << "Invalid choice. Exiting.\n";
        return;
    }

    Match::run(saveFiles[choice - 1]);
}
int main() {
    int choice;
    std::cout << "Select an option:\n1. Start New Game\n2. Load Existing Game\nChoice: ";
    std::cin >> choice;

    if (choice == 1) {
        startNewGame();
    } else if (choice == 2) {
        loadSavedGame();
    } else {
        std::cerr << "Invalid choice. Exiting.\n";
    }

    return 0;
}
