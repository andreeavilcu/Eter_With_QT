#pragma once

#include <iostream>
#include <functional>
#include <array>

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
        static bool eliminateCard();
        static bool eliminateRow();
        static bool coverCard();
        static bool sinkHole();
        static bool moveStackOwn();
        static bool extraEter();
        static bool moveStackOpponent();
        static bool moveEdge();
    };

    [[nodiscard]] bool play(const size_t _index) const {
        return m_wizards[_index]();
    }

    using FuncType = std::function<bool()>;

    std::array<FuncType, wizard_count> m_wizards = {
        []{ return WizardActions::eliminateCard(); },
        []{ return WizardActions::eliminateRow(); },
        []{ return WizardActions::coverCard(); },
        []{ return WizardActions::sinkHole(); },
        []{ return WizardActions::moveStackOwn(); },
        []{ return WizardActions::extraEter(); },
        []{ return WizardActions::moveStackOpponent(); },
        []{ return WizardActions::moveEdge(); }
    };

};

