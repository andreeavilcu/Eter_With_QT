#pragma once

#include <optional>
#include <utility>
#include <vector>
#include <random>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "../Actions/Card.h"
#include "../Actions/Wizard.h"
#include "../Actions/Power.h"

class Game;

class Player {
protected: 
    Card::Color m_color{};

    std::vector<Card> m_cards{};

    size_t m_wizardIndex{};
    std::pair<size_t, size_t> m_powerIndexes{};

    bool m_playedIllusion{false};

    /*
     * std::reference_wrapper does not work here, as initially there is no "last placed card"
     * std::shared_ptr does not work as the matrix does not use pointers,
     * and there were some issues regarding addresses and also maybe memory padding?
     * std::shared_ptr<Card> m_last_placed_card_ptr{};
     */

    Card* m_lastPlacedCard{};

    double m_timeLeft{};

public:
    friend class Wizard;
    friend class Board;

    [[nodiscard]] const std::vector<Card>& getCards() const;
    void setCards(const std::vector<Card>& _cards);

    void setTimer(int _duration);

    explicit Player(Card::Color _color, const std::vector<Card> &_cards, size_t _wizardIndex, size_t _powerIndexFirst,
                    size_t _powerIndexSecond);
    void returnCard(Card&& _card);
    void returnCard(Card& _card);

    void printCards();

    bool subtractTime(double _time);
    [[nodiscard]] double getTimeLeft() const;

    [[nodiscard]] Card::Color getColor() const;
    void setColor(Card::Color _color);

    [[nodiscard]] size_t getCardCount() const;
    [[nodiscard]] size_t getCardCount(Card::Value _value) const;

    void resetCards();

    [[nodiscard]] const Card* getLastPlacedCard() const;
    void setLastPlacedCard(Card& _card);

    [[nodiscard]] int getWizardIndex() const;
    void setWizardIndex(size_t _index);

    [[nodiscard]] std::pair<int, int> getPowersIndex() const;
    void setPowersIndex(std::pair<size_t, size_t> _index);

    [[nodiscard]] bool getPlayedIllusion() const;
    void setPlayedIllusion(bool _played);

    [[nodiscard]] bool wasIllusionPlayed() const;

    void shiftBoard(Game& _game);

    bool playCard(Game& _game);
    bool playIllusion(Game& _game);

    std::optional<Card> useCard(Card::Value _value);
    std::optional<Card> useIllusion(Card::Value _value);

    void playExplosion(Game& _game);

    bool playerTurn(Game& _game);

    [[nodiscard]] nlohmann::json toJson(Game& _game) const;
    Player(const nlohmann::json& _json, Game& _game);

private:
    std::optional<Card> playCardCheck(Game &_game, size_t _x, size_t _y, size_t _int_value);
    std::optional<Card> playIllusionCheck(Game &_game, size_t _x, size_t _y, size_t _int_value);

    [[nodiscard]] bool playWizard(Game& _game, bool _check);
    [[nodiscard]] bool playPower(Game& _game, bool _check);

    bool useWizard(Game& _game, bool _check);
    bool usePower(Game& _game, bool _first, bool _check);
};
