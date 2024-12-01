#pragma once

#include <cstdint>
#include <iostream>
#include <string>

class Card {
public:
    enum class Value: size_t {
        Eter = 0,
        One,
        Two,
        Three,
        Four,
    };

    enum class Color: short {
        Undefined = 0,
        Player1 = 1,
        Player2 = -1
    };

    explicit Card(Value _value, Color _color = Color::Undefined);
    ~Card() = default;
    Card(const Card& other) = default;
    Card& operator=(const Card& other) = default;
    Card(Card&& other) noexcept;
    Card& operator=(Card&& other) noexcept;

    [[nodiscard]] Value getValue() const;
    [[nodiscard]] Color getColor() const;

    void setColor(Color _color);
    [[nodiscard]] bool isIllusion() const;

    friend std::ostream& operator<<(std::ostream& os, const Card& card);

    bool operator==(const Card& other) const;

private:
    Value m_value;
    Color m_color;

    bool m_illusion = false;

public:
    void setIllusion();
    void resetIllusion();
    
};

