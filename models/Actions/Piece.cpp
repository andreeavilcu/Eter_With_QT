#include "Piece.h"

void Piece::setColor(const Card::Color _color){
  m_color = _color;
}
Card::Color Piece::getColor() const{
  return m_color;
}
Piece::Piece(const Card::Color _color)
    : m_color(_color) {
    }