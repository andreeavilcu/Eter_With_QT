#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

struct cardPosition {
    short x, y, z;
};

class Card {
public:
    enum class Value : short {
        Border = -1,
        Eter = 0,
        One,
        Two,
        Three,
        Four,
    };

    enum class Color: short {
        Undefined = 0,
        Red = 1,
        Blue = -1
    };

    bool operator<(const Card& rhs) const;

    explicit Card(Value _value, Color _color = Color::Undefined);
    explicit Card(const nlohmann::json& _json);
    ~Card() = default;
    Card(const Card& other) = default;
    Card& operator=(const Card& other) = default;
    Card(Card&& other) noexcept;
    Card& operator=(Card&& other) noexcept;

    [[nodiscard]] Value getValue() const;
    [[nodiscard]] Color getColor() const;

    void setColor(Color _color);
    [[nodiscard]] bool isIllusion() const;

    friend std::ostream& operator<<(std::ostream& os, const Card& _card);

    bool operator==(const Card& other) const;

    void setIllusion();
    void resetIllusion();

    [[nodiscard]] bool isJustReturned() const;
    void setJustReturned();
    void resetJustReturned();

    [[nodiscard]] nlohmann::json toJson() const;

private:
    Value m_value;
    Color m_color;

    bool m_illusion = false;
    size_t m_justReturned = 0;
};

