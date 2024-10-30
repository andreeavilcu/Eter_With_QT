#include <cstdint>

export module Card;

export class Card {
  protected:
    uint8_t m_value;

  public:
    explicit Card(const uint8_t _value) : m_value(_value) {}
};