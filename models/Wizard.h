#pragma once

#include <iostream>

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
        static void eliminateCard();
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

    std::array<FuncType, wizard_count> m_wizards = {
        WizardActions::eliminateCard, WizardActions::eliminateRow,
        WizardActions::coverCard, WizardActions::sinkHole,
        WizardActions::moveStackOwn, WizardActions::extraEter,
        WizardActions::moveStackOpponent, WizardActions::moveEdge
    };
};

