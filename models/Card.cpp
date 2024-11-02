#include "Card.h"

Card::Value Card::convertToValue(int value)
{
	switch (value) {
		case 1:
			return Value::one;
		case 2:
			return Value::two;
		case 3:
			return Value::three;
		case 4:
			return Value::four;
		case 5:
			return Value::five;
		default:
			return Value::Error;

	}
}

Card::Color Card::convertToColor(const std::string& color)
{
	if (color == "Red")
		return Color::Red;
	else
	{
		if (color == "Blue")
			return Color::Blue;
		else
		{
			throw std::invalid_argument("Invalid card color");
		}
	}
}

Card::Card(int value, const std::string& color)
	:m_color(convertToColor(color))
	,m_value(convertToValue(value))
{}

Card::Value Card::getValue() const
{
	return m_value;
}

Card::Color Card::getColor() const
{
	return m_color;
}
