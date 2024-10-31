#pragma once

#include <iostream>

class Player;

class Wizard {
private:
    static Wizard instance;

    Wizard() = default;
    ~Wizard() = default;

public:
    using FuncType = std::function<void()>;
    constexpr auto wizard_count = 8;
    friend class Player;

    Wizard(const Wizard&) = delete;
    Wizard& operator=(const Wizard&) = delete;

    static Wizard& getInstance() {
        return instance;
    }

private:
    std::array<FuncType, wizard_count> m_wizards = {
        eliminateCard, eliminateRow,
        coverCard, sinkHole,
        moveStackOwn, extraEter,
        moveStackOpponent,moveEdge
    };

    void play(size_t _index);

    void eliminateCard();
    void eliminateRow();
    void coverCard();
    void sinkHole();
    void moveStackOwn();
    void extraEter();
    void moveStackOpponent();
    void moveEdge();
};

