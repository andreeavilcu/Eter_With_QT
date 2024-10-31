#pragma once

#include <iostream>

class Player;

class Wizard {
public:
    using FuncType = std::function<void()>;
    static constexpr auto wizard_count = 8;

    Wizard(const Wizard&) = delete;
    Wizard& operator=(const Wizard&) = delete;

    friend class Player;

protected:
    static std::array<FuncType, wizard_count> m_wizards;

    static void play(size_t _index);

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

