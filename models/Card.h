#pragma once

#include <cstdint>
#include <ostream>

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
        Red,
        Blue,
    };
    enum class ID : size_t {
        StartID = 0,
        Player1,
        Player2,
    };

    explicit Card(Value _value, Color _color = Color::Undefined, ID _id=ID::StartID);
    ~Card() = default;
    Card(const Card& other) = default;
    Card& operator=(const Card& other) = default;
    Card(Card&& other) noexcept;
    Card& operator=(Card&& other) noexcept;

    [[nodiscard]] Value getValue() const;
    [[nodiscard]] Color getColor() const;
    [[nodiscard]] ID getPlayerID() const;

    void setPlayerID(ID _id);
    friend std::ostream& operator<<(std::ostream& os, const Card& card);
    friend class Player;


private:
    Value m_value;
    Color m_color;
    ID m_playerID;
    void setColor(Color _color);

};

