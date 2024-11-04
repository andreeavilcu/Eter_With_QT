#include "Card.h"

Card::Card(const Value& value)
	: m_value{ value } {
}

Card::Card(Card&& other) noexcept : m_value(other.m_value) {
	other.m_value = Value::Eter;
}

Card& Card::operator=(Card&& other) noexcept {
	if (this != &other) {
		m_value = other.m_value;
		other.m_value = Value::Eter;
	}
	return *this;
}

Card::Value Card::getValue() const {
	return m_value;
}

std::ostream& operator<<(std::ostream& os, const Card& card) {
	os << static_cast<int>(card.getValue());
	return os;
}
