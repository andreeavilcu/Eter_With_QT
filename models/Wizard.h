#pragma once

#include <iostream>
#include <functional>
#include <array>

#include "Card.h"
#include "Board.h"

class Game;
class Player;

class Wizard {
public:
    static constexpr auto wizard_count = 8;
    friend class Player;

    Wizard(const Wizard&) = delete;
    Wizard& operator=(const Wizard&) = delete;

    static Wizard& getInstance() {
        static Wizard instance;
        return instance;
    }

private:
    Wizard() = default;
    ~Wizard() = default;

    struct WizardActions {
        static bool eliminateCard(Player& _player, Game& _game);
        static bool eliminateRow(Player& _player, Game& _game);
        static bool coverCard(Player& _player, Game& _game);
        static bool sinkHole(Player& _player, Game& _game);
        static bool moveStackOwn(Player& _player, Game& _game);
        static bool extraEter(Player& _player, Game& _game);
        static bool moveStackOpponent(Player& _player, Game& _game);
        static bool moveEdge(Player& _player, Game& _game);
    };

    [[nodiscard]] bool play(const size_t _index, Player& _player, Game& _game) const {
        return m_wizards[_index](_player, _game);
    }

    using FuncType = std::function<bool(Player&, Game&)>;

    std::array<FuncType, wizard_count> m_wizards = {
        
        &WizardActions::eliminateCard,
        &WizardActions::eliminateRow,
        &WizardActions::coverCard,
        &WizardActions::sinkHole,
        &WizardActions::moveStackOwn,
        &WizardActions::extraEter,
        &WizardActions::moveStackOpponent,
        &WizardActions::moveEdge
 
    };

public:
    std::pair<size_t, size_t> getHole();
    void setHole(const std::pair<size_t, size_t>& _hole);

private:
    std::pair<size_t, size_t> m_hole{-1, -1};

};