#include "Match.h"

void Match::runMatch() {
    size_t matchesPlayed = m_gameType == Game::GameType::Training ? 3 : 5;
    size_t winsNeeded = matchesPlayed / 2 + 1;

    for (size_t index = 0; index < matchesPlayed; index++) {
        std::random_device rd;
        std::mt19937 gen{ rd() };
        std::uniform_int_distribution<size_t> wizardDistribution{ 0, Wizard::wizard_count - 1 };

        std::pair<size_t, size_t> wizardIndices = {-1, -1};

        if (m_gameType == Game::GameType::WizardDuel || m_gameType == Game::GameType::WizardAndPowerDuel) {
            do {
                wizardIndices.first = wizardDistribution(gen);
            }
            while (this->m_wizardsUsed[wizardIndices.first]);

            this->m_wizardsUsed[wizardIndices.first] = true;

            do {
                wizardIndices.second = wizardDistribution(gen);
            }
            while (this->m_wizardsUsed[wizardIndices.second]);

            this->m_wizardsUsed[wizardIndices.second] = true;
        }

        std::cout << "Match Score: Player1 - Player2\n";
        std::cout << "Game " + std::to_string(index) + "/" + std::to_string(matchesPlayed) +
            "        " + std::to_string(this->m_scores.first) +
            "    -    " + std::to_string(this->m_scores.second);
        std::cout << std::endl << std::endl;

        switch (Game game{ m_gameType , wizardIndices, this->m_illusions, this->m_explosion}; game.run()) {
            case 1:
                this->m_scores.first++;
                break;
            case 2:
                this->m_scores.second++;
                break;
            case 0:
                matchesPlayed++;
            default:
                break;
        }
    }
}
