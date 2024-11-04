#pragma once

#include <cstdint>
#include <ostream>

class Card {
public:
    enum class Value: uint8_t {
        Eter,
        One,
        Two,
        Three,
        Four,
    };

    enum class Color: uint8_t {
        Red,
        Blue
    };

    explicit Card(const Value& value, const Color& color);
    ~Card() = default;
    [[nodiscard]] Value getValue() const;
    [[nodiscard]] Color getColor() const;

    friend std::ostream& operator<<(std::ostream& os, const Card& card);

protected:
    Value m_value;
    Color m_color;
};

