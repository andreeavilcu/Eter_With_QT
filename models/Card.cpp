#include "Card.h"

Card::Card(const Value _value, const Color _color)
	: m_value{ _value },
	m_color{ _color } {
}

Card::Card(Card&& other) noexcept
	: m_value(other.m_value),
	m_color(other.m_color),
	m_illusion(other.m_illusion) {
	other.m_value = Value::Eter;
	other.m_color = Color::Undefined;
	other.m_illusion = false;
}


Card& Card::operator=(Card&& other) noexcept {
	if (this != &other) {
		m_value = other.m_value;
		m_color = other.m_color;
		m_illusion = other.m_illusion;
		other.m_value = Value::Eter;
		other.m_color = Color::Undefined;
		other.m_illusion = false;
	}
	return *this;
}

Card::Value Card::getValue() const {
	return m_value;
}

Card::Color Card::getColor() const {
	return m_color;
}

bool Card::isIllusion() const {
	return this->m_illusion;
}

bool Card::operator==(const Card &other) const {
	return this->m_value == other.m_value;
}

void Card::setColor(const Color _color) {
	m_color = _color;
}

void Card::resetIllusion() {
	this->m_illusion = false;
}


void Card::setIllusion() {
	this->m_illusion = true;
}

std::ostream& operator<<(std::ostream& os, const Card& _card) {
	if (_card.m_illusion)
		os << "#";

	else os << (static_cast<int>(_card.m_value) == 0 ? "E" : std::to_string(static_cast<int>(_card.getValue())));

	os << (_card.getColor() == Card::Color::Player1 ? "R " : "B ");

	return os;
}
