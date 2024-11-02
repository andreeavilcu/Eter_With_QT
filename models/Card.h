#pragma once

#include <cstdint>

class Card {
public:
    enum class Value: uint8_t {
        eter,
        one,
        two,
        three,
        four,
    };

    enum class Color: uint8_t {
        Red,
        Blue
    };

    explicit Card(const Value& value, const Color& color);
    ~Card() = default;
    [[nodiscard]] Value getValue() const;
    [[nodiscard]] Color getColor() const;
protected:
    Value m_value;
    Color m_color;
};

