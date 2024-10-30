#pragma once

#include "../../utils/constants.h"
#include <stdio.h>

class Card {
protected:
    size_t m_value;

public:
    explicit Card(size_t _value);
};

