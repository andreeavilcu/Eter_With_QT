#include "Piece.h"

void Piece::setColor(Color _color){
  m_color = _color;
}
Piece::Color Piece::getColor() const{
  return m_color;
}
Piece::Piece(const Color _color)
    : m_color(_color) {
    }