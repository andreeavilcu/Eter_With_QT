#pragma once

#include <cstdint>
#include <ostream>

class Card {
public:
    enum class Value: uint8_t {
        Eter = 0,
        One,
        Two,
        Three,
        Four,
    };

    explicit Card(const Value& value);
    ~Card() = default;
    Card(const Card& other) = default;
    Card& operator=(const Card& other) = default;
    Card(Card&& other) noexcept;
    Card& operator=(Card&& other) noexcept;

    [[nodiscard]] Value getValue() const;

    friend std::ostream& operator<<(std::ostream& os, const Card& card);

private:
    Value m_value;

};

