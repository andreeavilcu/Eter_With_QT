#include "Card.h"


Card::Card(const Value& value, const Color& color)
	:m_value{ value }
	,m_color{ color }
{}

Card::Value Card::getValue() const {
	return m_value;
}

Card::Color Card::getColor() const {
	return m_color;
}
