#pragma once

#include <cstdint>

struct GameEndInfo {
    Card::Color winner;
    size_t x, y;
};