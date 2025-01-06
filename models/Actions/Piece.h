#pragma once

#include "Card.h"
#include <nlohmann/json.hpp>

class Piece {
public:
    explicit Piece(Card::Color _color);
    explicit Piece(nlohmann::json _json);
    ~Piece() = default;
    Piece(const Piece& other) = default;
private:
    Card::Color m_color;
public:
  void setColor(Card::Color _color);
  [[nodiscard]] Card::Color getColor() const;

    nlohmann::json toJson() const;
};

