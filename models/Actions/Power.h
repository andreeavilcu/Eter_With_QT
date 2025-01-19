#pragma once

#include <map>
#include <array>
#include <iostream>
#include <algorithm>
#include <functional>
#include <nlohmann/json.hpp>

#include "Card.h"

class Game;
class Board;
class Player;

class Power {
public:
    static constexpr auto power_count = 24;
    friend class Player;
    friend class Board;

    Power(const Power&) = delete;
    Power& operator=(const Power&) = delete;

    static Power& getInstance() {
        static Power instance;
        return instance;
    }

    using FuncType = bool (*)(Player&, Game&, bool);

    [[nodiscard]] CardPosition getMinus(const Board& _board) const;

    void setMinus(CardPosition _position, Game& _game);
    void setMinus(Card* _ptr) { m_minus = _ptr; }

    [[nodiscard]] CardPosition getPlus(const Board& _board) const;

    void setPlus(CardPosition _position, Game& _game);
    void setPlus(Card* _ptr) { m_plus = _ptr; }

    size_t getRestrictedRow() const {
        return m_restrictedRow;
    }

    void setRestrictedRow(const size_t _row) {
        this->m_restrictedRow = _row;
    }

    size_t getRestrictedCol() const {
        return m_restrictedCol;
    }

    void setRestrictedCol(const size_t _col) {
        this->m_restrictedCol = _col;
    }

    bool getJustBlocked() const {
        return m_justBlocked;
    }

    void setJustBlocked(const bool _blocked) {
        this->m_justBlocked = _blocked;
    }

    nlohmann::json serialize(Game& _game);

private:
    Power() = default;
    ~Power() = default;

    Card* m_minus{};
    Card* m_plus{};

    size_t m_restrictedRow = -1;
    size_t m_restrictedCol = -1;

    bool m_justBlocked = false;

    struct PowerAction {
        static bool controlledExplosion(Player& _player, Game& _game, bool _check);
        static bool destruction(Player& _player, Game& _game, bool _check);
        static bool flame(Player& _player, Game& _game, bool _check);
        static bool lava(Player& _player, Game& _game, bool _check);
        static bool ash(Player& _player, Game& _game, bool _check);
        static bool spark(Player& _player, Game& _game, bool _check);
        static bool squall(Player& _player, Game& _game, bool _check);
        static bool gale(Player& _player, Game& _game, bool _check);
        static bool hurricane(Player& _player, Game& _game, bool _check);
        static bool gust(Player& _player, Game& _game, bool _check);
        static bool mirage(Player& _player, Game& _game, bool _check);
        static bool storm(Player& _player, Game& _game, bool _check);
        static bool tide(Player& _player, Game& _game, bool _check);
        static bool mist(Player& _player, Game& _game, bool _check);
        static bool wave(Player& _player, Game& _game, bool _check);
        static bool whirlpool(Player& _player, Game& _game, bool _check);
        static bool tsunami(Player& _player, Game& _game, bool _check);
        static bool waterfall(Player& _player, Game& _game, bool _check);
        static bool support(Player& _player, Game& _game, bool _check);
        static bool earthquake(Player& _player, Game& _game, bool _check);
        static bool crumble(Player& _player, Game& _game, bool _check);
        static bool border(Player& _player, Game& _game, bool _check);
        static bool avalanche(Player& _player, Game& _game, bool _check);
        static bool rock(Player& _player, Game& _game, bool _check);
    };

    [[nodiscard]] bool play(const size_t _index, Player& _player, Game& _game, const bool _check) const {
        return m_powers[_index](_player, _game, _check);
    }

    std::array<FuncType, power_count> m_powers = {
        &PowerAction::controlledExplosion,
        &PowerAction::destruction,
        &PowerAction::flame,
        &PowerAction::lava,
        &PowerAction::ash,
        &PowerAction::spark,
        &PowerAction::squall,
        &PowerAction::gale,
        &PowerAction::hurricane,
        &PowerAction::gust,
        &PowerAction::mirage,
        &PowerAction::storm,
        &PowerAction::tide,
        &PowerAction::mist,
        &PowerAction::wave,
        &PowerAction::whirlpool,
        &PowerAction::tsunami,
        &PowerAction::waterfall,
        &PowerAction::support,
        &PowerAction::earthquake,
        &PowerAction::crumble,
        &PowerAction::border,
        &PowerAction::avalanche,
        &PowerAction::rock
    };
};
