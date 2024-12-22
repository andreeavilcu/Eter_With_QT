#pragma once

#include <vector>
#include "Card.h"
#include "Player.h"
#include "Board.h"
 

class IGame {
public:
    virtual ~IGame() = default;

    [[nodiscard]] virtual Board& getBoard() = 0;
    //[[nodiscard]] virtual Game::GameType getGameType() const = 0;
    //TO DO: fisier separat pentru GameType pentru a evita problema de includere circulara
    [[nodiscard]] virtual Player& getPlayer1() = 0;
    [[nodiscard]] virtual Player& getPlayer2() = 0;

    virtual size_t run() = 0;

    [[nodiscard]] virtual bool checkEmptyDeck() const = 0;
    [[nodiscard]] virtual bool checkEndOfGame(Card::Color _color) = 0;
};
