#include "Wizard.h"

std::array<Wizard::FuncType, Wizard::wizard_count> Wizard::m_wizards = {
    eliminateCard, eliminateRow,
    coverCard, sinkHole,
    moveStackOwn, extraEter,
    moveStackOpponent,moveEdge
};

void Wizard::play(size_t _index) {
    Wizard::m_wizards[_index]();
}

void Wizard::eliminateCard() {
    std::cout << "eliminateCard" << std::endl;
}
void Wizard::eliminateRow() {
    std::cout << "eliminateRow" << std::endl;
}
void Wizard::coverCard() {
    std::cout << "coverCard" << std::endl;
}
void Wizard::sinkHole() {
    std::cout << "sinkHole" << std::endl;
}
void Wizard::moveStackOwn() {
    std::cout << "moveStackOwn" << std::endl;
}
void Wizard::extraEter() {
    std::cout << "extraEter" << std::endl;
}
void Wizard::moveStackOpponent() {
    std::cout << "moveStackOpponent" << std::endl;
}
void Wizard::moveEdge() {
    std::cout << "moveEdge" << std::endl;
}
