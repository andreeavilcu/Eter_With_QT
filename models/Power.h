#pragma once

#include <iostream>
#include <functional>
#include <array>

class Player;

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
		static bool controlledExplosion();
		static bool destruction();
		static bool flame();
		static bool fire();
		static bool ash();
		static bool spark();
		static bool squall();
		static bool gale();
		static bool hurricane();
		static bool gust();
		static bool mirage();
		static bool storm();
		static bool tide();
		static bool mist();
		static bool wave();
		static bool whirlpool();
		static bool blizzard();
		static bool waterfall();
		static bool support();
		static bool earthquake();
		static bool crumble();
		static bool border();
		static bool avalanche();
		static bool rock();
	};

	[[nodiscard]] bool play(const size_t _index) const {
		return m_powers[_index]();
	}

	using FuncType = std::function<bool()>;

	std::array<FuncType, power_count> m_powers = {
		PowerAction::ash,
		PowerAction::avalanche,
		PowerAction::blizzard,
		PowerAction::border,
		PowerAction::controlledExplosion,
		PowerAction::crumble,
		PowerAction::destruction,
		PowerAction::earthquake,
		PowerAction::fire,
		PowerAction::flame,
		PowerAction::gale,
		PowerAction::gust,
		PowerAction::hurricane,
		PowerAction::mirage,
		PowerAction::mist,
		PowerAction::rock,
		PowerAction::spark,
		PowerAction::squall,
		PowerAction::storm,
		PowerAction::support,
		PowerAction::tide,
		PowerAction::waterfall,
		PowerAction::wave,
		PowerAction::whirlpool
	};

};