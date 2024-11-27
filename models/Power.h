#pragma once

#include <iostream>
#include <functional>
#include <array>

#include "Card.h"

class Game;

class Power {
public:
    static constexpr auto power_count = 24;
    friend class Player;

    Power(const Power&) = delete;
    Power& operator=(const Power&) = delete;

    static Power& getInstance() {
        static Power instance;
        return instance;
    }

private:
    Power() = default;
    ~Power() = default;

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

    using FuncType = std::function<bool(Player&, Game&)>;

    std::array<FuncType, power_count> m_powers = {
//      PowerAction::controlledExplosion, 
        [](Player& _player, Game& _game) { return PowerAction::controlledExplosion(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::destruction(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::flame(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::lava(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::ash(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::spark(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::squall(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::gale(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::hurricane(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::gust(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::mirage(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::storm(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::tide(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::mist(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::wave(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::whirlpool(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::tsunami(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::waterfall(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::support(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::earthquake(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::crumble(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::border(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::avalanche(_player, _game); },
        [](Player& _player, Game& _game) { return PowerAction::rock(_player, _game); }
    };
};
