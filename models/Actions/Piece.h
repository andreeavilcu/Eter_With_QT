#pragma once

#include "Card.h"

class Piece {
public:
    explicit Piece(Card::Color _color);
    ~Piece() = default;
    Piece(const Piece& other) = default;
private:
    Card::Color m_color;
public:
  void setColor(Card::Color _color);
  [[nodiscard]] Card::Color getColor() const;
};

