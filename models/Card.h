#pragma once

#include <cstdint>
#include <iostream>
#include <string>

class Player;

class Card {
public:
    enum class Value: size_t {
        Eter = 0,
        One,
        Two,
        Three,
        Four,
    };

    enum class Color: size_t {
        Undefined = 0,
        Player1,
        Player2
    };

    explicit Card(Value _value, Color _color = Color::Undefined);
    ~Card() = default;
    Card(const Card& other) = default;
    Card& operator=(const Card& other) = default;
    Card(Card&& other) noexcept;
    Card& operator=(Card&& other) noexcept;

    [[nodiscard]] Value getValue() const;
    [[nodiscard]] Color getColor() const;

    [[nodiscard]] bool isIllusion() const;

    friend std::ostream& operator<<(std::ostream& os, const Card& card);
    friend class Player;


private:
    Value m_value;
    Color m_color;

    bool m_illusion = false;

    void setColor(Color _color);
    void setIllusion();

public:
    void resetIllusion();

};

