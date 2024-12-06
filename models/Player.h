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
protected: 
    Card::Color m_color{};

    std::vector<Card> m_cards{};

    size_t m_wizard_index{};
    std::pair<size_t, size_t> m_powers_index{};

    bool m_playedIllusion{false};

    std::pair<size_t, size_t> m_last_placed_card;
    
public:
    friend class Wizard;
    friend class Board;

    [[nodiscard]] const std::vector<Card>& getCards() const;
    void setCards(const std::vector<Card>& _cards);

    explicit Player(Card::Color _color, const std::vector<Card>& _cards, bool _wizard, bool _powers);
    void returnCard(const Card& _card);

    void printCards();
    void placeCard(size_t row, size_t col);

    [[nodiscard]] Card::Color getColor() const;
    void setColor(Card::Color _color);

    [[nodiscard]] size_t getCardCount() const;
    [[nodiscard]] size_t getCardCount(Card::Value _value) const;

    [[nodiscard]] std::pair<size_t, size_t> getLastPlacedCard() const;
    void setLastPlacedCard(std::pair<size_t, size_t> _cardPosition);

    [[nodiscard]] int getWizardIndex() const;
    void setWizardIndex(size_t _index);

    [[nodiscard]] std::pair<int, int> getPowersIndex() const;
    void setPowersIndex(std::pair<size_t, size_t> _index);

    [[nodiscard]] bool getPlayedIllusion() const;
    void setPlayedIllusion(bool _played);

    [[nodiscard]] bool wasIllusionPlayed() const;

    bool useWizard(Game& _game, bool _check);
    bool usePower(Game& _game, bool _first, bool _check);

    std::optional<Card> useCard(Card::Value _value);
    std::optional<Card> useIllusion(Card::Value _value);

    void shiftBoard(Game& _game);

    bool playCard(Game& _game);
    std::optional<Card> playCardCheck(Game &_game, size_t _x, size_t _y, size_t _int_value);

    bool playIllusion(Game& _game);
    std::optional<Card> playIllusionCheck(Game &_game, size_t _x, size_t _y, size_t _int_value);

    [[nodiscard]] bool playWizard(Game& _game, bool _check);
    [[nodiscard]] bool playPower(Game& _game, bool _check);

    void playExplosion(Game& _game);

    bool playerTurn(Game& _game);
};
