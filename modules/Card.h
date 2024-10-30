#pragma once

#include "../utils/constants.h"

class Card {
protected:
    size_t m_value;

public:
    explicit Card(const size_t _value) : m_value(_value) {}
};

