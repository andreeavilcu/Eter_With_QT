#include "Piece.h"

void Piece::setColor(const Card::Color _color){
  m_color = _color;
}
Card::Color Piece::getColor() const{
  return m_color;
}

nlohmann::json Piece::toJson() const {
  nlohmann::json json;

  json["color"] = m_color;

  return json;
}

Piece::Piece(const Card::Color _color)
    : m_color(_color) {
    }

Piece::Piece(nlohmann::json _json) {
  this->m_color = _json["color"];
}
