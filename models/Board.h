#pragma once

#include<array>
#include<list>
#include <algorithm>
#include<iostream>
#include "Card.h"

template<size_t gridSize>
class Board {
private:
	std::array<std::array<std::list<Card>, gridSize>, gridSize> m_board;
	std::array<std::array<bool, gridSize>, gridSize> m_occupied;

	std::array<uint8_t, gridSize> m_rowIndex;
	std::array<uint8_t, gridSize> m_colIndex;

	class RowProxy {
	private:
		Board& m_board;
		size_t m_row;
	public: 
		RowProxy(Board& _board,size_t _row) :
			m_board{ _board },
			m_row{ _row } {
		}

		Card& operator[](size_t _col){
			return m_board.m_board[m_board.m_rowIndex[m_row]][m_board.m_colIndex[_col]];
		}
		const Card& operator[](size_t _col) const {
			return m_board.m_board[m_board.m_rowIndex[m_row]][m_board.m_colIndex[_col]];
		}
	};

public:
	RowProxy& operator[](size_t _row) {
		return RowProxy{ *this, _row };
	}
	const RowProxy& operator[](size_t _row) const {
		return { *this, _row };
	}

	Board() {
		for (size_t i = 0; i < gridSize; ++i) {
			m_rowIndex[i] = i;
			m_colIndex[i] = i;
		}
	}

	void swapRow(size_t _first, size_t _second) {
		std::swap(m_rowIndex[_first], m_rowIndex[_second]);
	}
	void swapCol(size_t _first, size_t _second) {
		std::swap(m_colIndex[_first], m_colIndex[_second]);
	}

	void placeCard(size_t _row, size_t _col)
	{
		if (_row < 0 || _row >= gridSize || _col < 0 || _col >= gridSize){
			std::cout << "Error at indices\n";
			return;
		}
		m_board[_row][_col].push_back(Card{ Card::Value::eter, Card::Color::Blue});
	}

	void printTest()
	{
		for (size_t i = 0; i < gridSize; ++i) {
			for (size_t j = 0; j < gridSize; ++j)
				std::cout << m_board[i][j].size() << " ";
			std::cout << std::endl;
		}
	}
};
