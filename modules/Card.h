//
// Created by Andrei PRIBOI on 30.10.2024.
//

#ifndef CARD_H
#define CARD_H

#include <stdio.h>

class Card {
protected:
    size_t m_value;

public:
    explicit Card(const size_t _value) : m_value(_value) {}
};

#endif //CARD_H
