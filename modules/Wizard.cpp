#include "Wizard.h"

std::array<Wizard::FuncType, eter::wizard_count> Wizard::m_wizards = {
    eliminateCard, eliminateRow, coverCard, sinkHole, moveStackOwn, extraEter, moveStackOpponent, moveEdge
};

Wizard::Wizard(const size_t _value) : Card(_value) {}

void Wizard::playWizard() const {
    m_wizards[this->m_value]();
}