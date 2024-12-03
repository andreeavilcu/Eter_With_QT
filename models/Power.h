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

    using FuncType = bool (*)(Player&, Game&);

    [[nodiscard]] FuncType getPowerAction(size_t _index) const;
     void setPowerAction(size_t _index, FuncType _func);

    [[nodiscard]] std::pair<size_t, size_t> getMinus() const;
    void setMinus(size_t _row, size_t _col);

    [[nodicard]] std::pair<size_t, size_t> getPlus() const;
    void setPlus(size_t _row, size_t _col);

    [[nodiscard]] size_t getRestrictedRow() const;
    void setRestrictedRow(size_t _row);

    [[nodiscard]]size_t getRestrictedCol() const;
    void setRestrictedCol(size_t _col);

    [[nodiscard]]bool getJustBlocked() const;
    void setJustBlocked(bool _blocked);
private:
    Power() = default;
    ~Power() = default;

    std::pair<size_t, size_t> m_minus{ -1, -1 };
    std::pair<size_t, size_t> m_plus{ -1, -1 };

    size_t m_restrictedRow = -1;
    size_t m_restrictedCol = -1;

    bool m_justBlocked = false;

    struct PowerAction {
        static bool controlledExplosion(Player& _player, Game& _game);
        static bool destruction(Player& _player, Game& _game);
        static bool flame(Player& _player, Game& _game);
        static bool lava(Player& _player, Game& _game);
        static bool ash(Player& _player, Game& _game);
        static bool spark(Player& _player, Game& _game);
        static bool squall(Player& _player, Game& _game);
        static bool gale(Player& _player, Game& _game);
        static bool hurricane(Player& _player, Game& _game);
        static bool gust(Player& _player, Game& _game);
        static bool mirage(Player& _player, Game& _game);
        static bool storm(Player& _player, Game& _game);
        static bool tide(Player& _player, Game& _game);
        static bool mist(Player& _player, Game& _game);
        static bool wave(Player& _player, Game& _game);
        static bool whirlpool(Player& _player, Game& _game);
        static bool tsunami(Player& _player, Game& _game);
        static bool waterfall(Player& _player, Game& _game);
        static bool support(Player& _player, Game& _game);
        static bool earthquake(Player& _player, Game& _game);
        static bool crumble(Player& _player, Game& _game);
        static bool border(Player& _player, Game& _game);
        static bool avalanche(Player& _player, Game& _game);
        static bool rock(Player& _player, Game& _game);
    };

    [[nodiscard]] bool play(const size_t _index, Player& _player, Game& _game) const {
        return m_powers[_index](_player, _game);
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
