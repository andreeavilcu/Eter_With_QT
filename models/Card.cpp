#include "Card.h"

Card::Card(const Value _value, const Color _color)
	: m_value{ _value },
	m_color{ _color } {
}

Card::Card(Card&& other) noexcept
	: m_value(other.m_value),
	m_color(other.m_color) {
	other.m_value = Value::Eter;
	other.m_color = Color::Undefined;
}


Card& Card::operator=(Card&& other) noexcept {
	if (this != &other) {
		m_value = other.m_value;
		m_color = other.m_color;
		other.m_value = Value::Eter;
		other.m_color = Color::Undefined;
	}
	return *this;
}

Card::Value Card::getValue() const {
	return m_value;
}

Card::Color Card::getColor() const {
	return m_color;
}

void Card::setColor(const Color _color) {
	m_color = _color;
}

std::ostream& operator<<(std::ostream& os, const Card& card) {
	if (card.getColor() == Card::Color::Red)
		os << "\033[31m";

	if(card.getColor() == Card::Color::Blue)
		os << "\033[34m";

	if (card.getValue() == Card::Value::Eter)
		os << "E" << "\033[0m";

	else
		os << static_cast<int>(card.getValue()) << "\033[0m";

	return os;
}
