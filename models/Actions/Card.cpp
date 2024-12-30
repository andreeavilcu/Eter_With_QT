#include "Card.h"

bool Card::operator<(const Card &rhs) const {
	return this->m_value < rhs.m_value;
}

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

bool Card::isJustReturned() const {
	return this->m_justReturned;
}

void Card::setJustReturned() {
	this->m_justReturned = 2;
}

void Card::resetJustReturned() {
	if (this->m_justReturned == 0)
		return;

	this->m_justReturned--;
}

void Card::setIllusion() {
	this->m_illusion = true;
}

std::ostream& operator<<(std::ostream& os, const Card& _card) {
	
	if (_card.m_value == Card::Value::Border) {
		os << "== ";
		return os;
	}

	if (_card.m_illusion)
		os << "#";
	
	else os << ( _card.m_value== Card::Value::Eter  ? "E" : std::to_string(static_cast<int>(_card.getValue())));

	if (_card.isJustReturned())
		os << "X ";

	else os << (_card.getColor() == Card::Color::Player1 ? "R " : "B ");

	return os;
}
