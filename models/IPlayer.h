#pragma once

#include <vector>
#include <optional>
#include <utility>
#include "Card.h"

class Game;

class IPlayer {
public:
    virtual ~IPlayer() = default;

    virtual const std::vector<Card>& getCards() const = 0;
    virtual size_t getCardCount() const = 0;
    virtual std::optional<Card> useCard(Card::Value value) = 0;

    virtual void printCards() = 0;
    virtual void placeCard(size_t row, size_t col) = 0;
    virtual std::pair<size_t, size_t> getLastPlacedCard() const = 0;

    virtual Card::Color getColor() const = 0;
    virtual void setColor(Card::Color color) = 0;

    virtual bool playWizard(Game& game, bool check) = 0;
    virtual bool playPower(Game& game, bool check) = 0;
    virtual bool playIllusion(Game& game) = 0;
    virtual void playExplosion(Game& game) = 0;

    virtual void shiftBoard(Game& game) = 0;

    virtual bool playerTurn(Game& game) = 0;

    virtual void resetCards() = 0;
};

