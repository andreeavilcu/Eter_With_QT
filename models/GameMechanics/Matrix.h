#pragma once

#include <vector>

constexpr auto MAX_HEIGHT = 9; // proof

template<typename T>
using Matrix = std::vector<std::vector<std::deque<T>>>;
