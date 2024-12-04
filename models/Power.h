#pragma once

#include <iostream>
#include <functional>
#include <array>
#include<map>

#include "Card.h"
#include "Board.h"

class Game;
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

    std::pair<size_t, size_t> getMinus() const {
        return m_minus;
    }

    void setMinus(size_t _row, size_t _col) {
        this->m_minus = { _row, _col };
    }

    std::pair<size_t, size_t> getPlus() const {
        return m_plus;
    }

    void setPlus(size_t _row, size_t _col) {
        this->m_plus = { _row, _col };
    }

    size_t getRestrictedRow() const {
        return m_restrictedRow;
    }

    void setRestrictedRow(size_t _row) {
        this->m_restrictedRow = _row;
    }

    size_t getRestrictedCol() const {
        return m_restrictedCol;
    }

    void setRestrictedCol(size_t _col) {
        this->m_restrictedCol = _col;
    }

    bool getJustBlocked() const {
        return m_justBlocked;
    }

    void setJustBlocked(bool _blocked) {
        this->m_justBlocked = _blocked;
    }

private:
    Power() = default;
    ~Power() = default;

    std::pair<size_t, size_t> m_minus{ -1, -1 };
    std::pair<size_t, size_t> m_plus{ -1, -1 };

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
