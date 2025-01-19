#pragma once

#include <iostream>
#include <functional>
#include <array>
#include <nlohmann/json.hpp>
#include "Card.h"

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

    // Acțiuni vrăjitor
    struct WizardActions {
        static bool eliminateCard(Player& _player, Game& _game, bool _check);
        static bool eliminateRow(Player& _player, Game& _game, bool _check);
        static bool coverCard(Player& _player, Game& _game, bool _check);
        static bool sinkHole(Player& _player, Game& _game, bool _check);
        static bool moveStackOwn(Player& _player, Game& _game, bool _check);
        static bool extraEter(Player& _player, Game& _game, bool _check);
        static bool moveStackOpponent(Player& _player, Game& _game, bool _check);
        static bool moveEdge(Player& _player, Game& _game, bool _check);
    };

    // Get hole și set hole pentru gestionarea sinkholes
    std::pair<size_t, size_t> getHole() {
        return this->m_hole;
    }

    void setHole(const std::pair<size_t, size_t>& _hole) {
        this->m_hole = _hole;
    }

    nlohmann::json serialize() {
        nlohmann::json json;
        json["hole"] = this->m_hole;
        return json;
    }

    [[nodiscard]] bool play(const size_t _index, Player& _player, Game& _game, const bool _check) const {
        return m_wizards[_index](_player, _game, _check);
    }

    const std::string& getWizardName(size_t index) const {
        return wizardNames.at(index);
    }
private:
    Wizard() = default;
    ~Wizard() = default;

    const std::array<std::string, wizard_count> wizardNames = {
       "eliminateCard",
       "eliminateRow",
       "coverCard",
       "sinkHole",
       "moveStackOwn",
       "extraEter",
       "moveStackOpponent",
       "moveEdge"
    };

    using FuncType = std::function<bool(Player&, Game&, bool)>;

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

    std::pair<size_t, size_t> m_hole{ -1, -1 };
};