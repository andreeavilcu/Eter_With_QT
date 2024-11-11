#include "Wizard.h"

#include "Game.h"

bool Wizard::WizardActions::eliminateCard(Player& _player, Game& _game) {
    size_t x, y;
    Game::Board& board = _game.m_board;

    std::cout << "Eliminate an opponent's card that covers your own.\n";
    std::cout << "Enter (x, y) coordinates for wizard action (0-indexed)\n";
    std::cin >> x >> y;

    if (x >= board.m_board.size() || y >=board.m_board.size())
        return false;

    if (board.m_board[x][y].empty() || board.m_board[y][x].size() < 2)
        return false;

    if (board.m_board[x][y].back().getColor() == _player.getColor())
        return false;

    if (const size_t secondLastCardIndex = board.m_board[x][y].size() - 2; board.m_board[x][y][secondLastCardIndex].getColor() != _player.getColor())
        return false;

    auto eliminatedCard = std::move(board.m_board[x][y].back());
    board.m_board[x][y].pop_back();

    return true;
}

bool Wizard::WizardActions::eliminateRow(Player& _player, Game& _game) {
    return true;
}

bool Wizard::WizardActions::coverCard(Player& _player, Game& _game) {
    return true;
}

bool Wizard::WizardActions::sinkHole(Player& _player, Game& _game) {
    return true;
}

bool Wizard::WizardActions::moveStackOwn(Player& _player, Game& _game) {
    return true;
}

bool Wizard::WizardActions::extraEter(Player& _player, Game& _game) {
    return true;
}

bool Wizard::WizardActions::moveStackOpponent(Player& _player, Game& _game) {
    return true;
}

bool Wizard::WizardActions::moveEdge(Player& _player, Game& _game) {
    return true;
}
