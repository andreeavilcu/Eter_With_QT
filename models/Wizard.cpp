#include "Wizard.h"

template<GameType gameType>
void Wizard::WizardActions<gameType>::eliminateCard(const Card::Color _color, const size_t _row, const size_t _col) {
    std::vector stack = Game<gameType>::getInstance().m_board[_row][_col];
    Card last = stack.pop_back();
    Card curr = stack.back();

    if(curr.getColor() == last.getColor() && last.getColor() == _color)
        stack.push_back(last);
}

template<GameType gameType>
void Wizard::WizardActions<gameType>::eliminateRow() {
    std::cout << "eliminateRow" << std::endl;
}

template<GameType gameType>
void Wizard::WizardActions<gameType>::coverCard() {
    std::cout << "coverCard" << std::endl;
}

template<GameType gameType>
void Wizard::WizardActions<gameType>::sinkHole() {
    std::cout << "sinkHole" << std::endl;
}

template<GameType gameType>
void Wizard::WizardActions<gameType>::moveStackOwn() {
    std::cout << "moveStackOwn" << std::endl;
}

template<GameType gameType>
void Wizard::WizardActions<gameType>::extraEter() {
    std::cout << "extraEter" << std::endl;
}

template<GameType gameType>
void Wizard::WizardActions<gameType>::moveStackOpponent() {
    std::cout << "moveStackOpponent" << std::endl;
}

template<GameType gameType>
void Wizard::WizardActions<gameType>::moveEdge() {
    std::cout << "moveEdge" << std::endl;
}
