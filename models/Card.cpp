#include "Card.h"

Card::Card(const Value _value, const Color _color, const ID _id)
	: m_value{ _value },
	m_color{ _color },
	m_playerID{ _id } {
}

Card::Card(Card&& other) noexcept
	: m_value(other.m_value), m_color(other.m_color), m_playerID(other.m_playerID) {
	other.m_value = Value::Eter;
	other.m_color = Color::Undefined;
	other.m_playerID = ID::StartID;
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

Card::ID Card::getPlayerID() const{
	return m_playerID;
}

void Card::setPlayerID(ID _id) {
	m_playerID  = _id;
}

void Card::setColor(const Color _color) {
	m_color = _color;
}

std::ostream& operator<<(std::ostream& os, const Card& card) {
	os << static_cast<int>(card.getValue());
	return os;
}
