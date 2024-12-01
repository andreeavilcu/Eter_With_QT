#pragma once

#include <optional>
#include <utility>
#include <vector>
#include <random>

#include "Card.h"
#include "Wizard.h"
#include "Power.h"

class Game;

class Player {
public: // TODO: protected + get & set
    Card::Color m_color{};

    std::vector<Card> m_cards{};

    size_t m_wizard_index{};
    std::pair<size_t, size_t> m_powers_index{};

    bool m_playedIllusion{false};

    std::vector<Card> m_eliminated_cards{}; //TODO: use addEliminatedCard in every method that eliminates a card
    std::pair<size_t, size_t> m_last_placed_card;
public:
    friend class Wizard;

    explicit Player(Card::Color _color, const std::vector<Card>& _cards, bool _wizard, bool _powers);
    void returnCard(const Card& _card);

    void printCards();
    void placeCard(size_t row, size_t col);

    void addEliminatedCard(const Card& _card);

    [[nodiscard]] std::vector<Card> getEliminatedCards();
    [[nodiscard]] Card::Color getColor() const;
    [[nodiscard]] size_t getCardCount() const;
    [[nodiscard]] size_t getCardCount(Card::Value _value) const;
    [[nodiscard]] std::pair<size_t, size_t> getLastPlacedCard() const;

    [[nodiscard]] int getWizardIndex() const;
    [[nodiscard]] std::pair<int, int> getPowersIndex() const;
    [[nodiscard]] bool wasIllusionPlayed() const;

    bool useWizard(Game& _game);
    bool usePower(Game& _game, bool _first);

    std::optional<Card> useCard(Card::Value _value);
    std::optional<Card> useIllusion(Card::Value _value);
};
