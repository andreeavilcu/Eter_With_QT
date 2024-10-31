#pragma once

#include <iostream>
#include "../utils/constants.h"

class Wizard {
public:
    using FuncType = std::function<void()>;

    static Wizard& getInstance() {
        static Wizard instance;
        return instance;
    }

    Wizard(const Wizard&) = delete;
    Wizard& operator=(const Wizard&) = delete;

    static void play(size_t _index);

protected:
    static std::array<FuncType, eter::wizard_count> m_wizards;

    Wizard() = default;
    ~Wizard() = default;

private:
    static void eliminateCard();
    static void eliminateRow();
    static void coverCard();
    static void sinkHole();
    static void moveStackOwn();
    static void extraEter();
    static void moveStackOpponent();
    static void moveEdge();
};

