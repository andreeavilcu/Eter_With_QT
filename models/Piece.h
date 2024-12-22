#pragma once

class Piece {
public:
    enum class Color: short {
        Undefined = 0,
        Player1 = 1,
        Player2 = -1
    };
    explicit Piece(Color _color = Color::Undefined);
    ~Piece() = default;
    Piece(const Piece& other) = default;
private:
    Color m_color;
public:
  void setColor(Color _color);
  [[nodiscard]] Color getColor() const;
};

