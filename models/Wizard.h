#pragma once

#include <iostream>
#include <functional>
#include <array>

#include "Game.h"
#include "Card.h"


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

    template<GameType gameType>
    struct WizardActions {
        static void eliminateCard(const Card::Color _color, const size_t _row,const size_t _col);
        static void eliminateRow();
        static void coverCard();
        static void sinkHole();
        static void moveStackOwn();
        static void extraEter();
        static void moveStackOpponent();
        static void moveEdge();
    };

    void play(const size_t _index) const {
        m_wizards[_index]();
    }
    using FuncType = std::function<void()>;

    template <GameType gameType>
    std::array<FuncType, wizard_count> m_wizards = {
            WizardActions<gameType>::eliminateCard, WizardActions<gameType>::eliminateRow,
            WizardActions<gameType>::coverCard, WizardActions<gameType>::sinkHole,
            WizardActions<gameType>::moveStackOwn, WizardActions<gameType>::extraEter,
            WizardActions<gameType>::moveStackOpponent, WizardActions<gameType>::moveEdge
        }; //TODO fix this


};

