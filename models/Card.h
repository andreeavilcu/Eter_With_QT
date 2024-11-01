#pragma once

#include <stdio.h>
#include <iostream>

class Card {
protected:
    enum class Value {
        one,
        two,
        three,
        four,
        five
    };

    enum class Color {
        Red,
        Blue
    };

    Value m_value;
    Color m_color;

public:

    Card(int value, const std::string& color);
    ~Card() = default;
    Value getValue() const;
    Color getColor() const;

};

