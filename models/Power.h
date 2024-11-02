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
		static void controlledExplosion();
		static void destruction();
		static void flame();
		static void fire();
		static void ash();
		static void spark();
		static void squall();
		static void gale();
		static void hurricane();
		static void gust();
		static void mirage();
		static void storm();
		static void tide();
		static void mist();
		static void wave();
		static void whirlpool();
		static void blizzard();
		static void waterfall();
		static void support();
		static void earthquake();
		static void crumble();
		static void border();
		static void avalanche();
		static void rock();
	};

	void play(const size_t _index) const {
		m_powers[_index]();
	}

	using FuncType = std::function<void()>;

	std::array<FuncType, power_count> m_powers =
	{
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