#include "Match.h"

void Match::runMatch() {
    size_t matchesPlayed = m_gameType == Game::GameType::Training ? 3 : 5;
    size_t winsNeeded = matchesPlayed / 2 + 1;

    if (this->m_matchType == MatchType::Normal) {
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
                "        " + formatScore(this->m_scores.first) +
                "    -    " + formatScore(this->m_scores.second);
            std::cout << std::endl << std::endl;

            switch (Game game{ m_gameType , wizardIndices, this->m_illusions, this->m_explosion}; game.run()) {
                case 1:
                    this->m_scores.first++;
                break;
                case 2:
                    this->m_scores.second++;
                break;
                case 0:
                    this->m_scores.first += 0.5f;
                this->m_scores.second += 0.5f;

                matchesPlayed++;
                default:
                    break;
            }

            if (this->m_scores.first >= static_cast<float>(winsNeeded) || this->m_scores.second >= static_cast<float>(winsNeeded))
                break;
        }
    }

    bool player1Win = this->m_scores.first > this->m_scores.second;

    std::cout << "Match winner: Player " << (player1Win ? "1" : "2") << std::endl;
}
