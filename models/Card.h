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
        five,
        Error
    };

    enum class Color {
        Red,
        Blue
    };

    Value m_value;
    Color m_color;

private:
    static Value convertToValue(int value);
    static Color convertToColor(const std::string& color);

public:

    Card(int value, const std::string& color);
    ~Card() = default;
    Value getValue() const;
    Color getColor() const;

};

