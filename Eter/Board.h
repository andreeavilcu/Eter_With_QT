#pragma once

#include<array>
#include<list>
#include<iostream>
#include "Card.h"

template<size_t gridSize>
class Board {
private:
	std::array<std::array<std::list<Card>, gridSize>, gridSize> m_board;
	std::array<std::array<bool, gridSize>, gridSize> m_occupied;

	std::array<uint8_t, gridSize> m_rowIndex;
	std::array<uint8_t, gridSize> m_colIndex;
public:

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

};
