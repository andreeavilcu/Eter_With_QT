#pragma once

#include <cstdint>
#include <iostream>

class Player;

class Card {
public:
    enum class Value: size_t {
        Eter = 1,
        One = 1,
        Two,
        Three,
        Four,
    };

    enum class Color: size_t {
        Undefined = 0,
        Red,
        Blue,
    };

    explicit Card(Value _value, Color _color = Color::Undefined);
    ~Card() = default;
    Card(const Card& other) = default;
    Card& operator=(const Card& other) = default;
    Card(Card&& other) noexcept;
    Card& operator=(Card&& other) noexcept;

    [[nodiscard]] Value getValue() const;
    [[nodiscard]] Color getColor() const;

    friend std::ostream& operator<<(std::ostream& os, const Card& card);
    friend class Player;


private:
    Value m_value;
    Color m_color;
    void setColor(Color _color);

};

