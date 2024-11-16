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

    if (board.m_board[y][x].size() < 2)
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
    size_t startX, startY, endX, endY;
    Game::Board& board = _game.m_board;

    std::cout << "Move a stack with your own card on top to an empty position.\n";
    std::cout << "Enter the coordinates of the stack:\n";
    std::cin >> startX >> startY;

    if (!board.checkIndexes(startX,startY))
        return false;

    if (board.m_board[startX][startY].empty() || board.m_board[startX][startY].size() < 2)
        return false;

    if (board.m_board[startX][startY].back().getColor() != _player.getColor())
        return false;

    std::cout << "Enter coordinates for the stack destination:\n";
    std::cin >> endX >> endY;

    if (!board.checkIndexes(endX,endY))
        return false;

    if (!board.m_board[endX][endY].empty())
        return false;

    board.m_board[endX][endY] = std::move(board.m_board[startX][startY]);
    board.m_board[startX][startY].clear();

    std::cout << "Stack moved successfully!";
    return true;
}

bool Wizard::WizardActions::extraEter(Player& _player, Game& _game) {
    size_t x, y;
    Game::Board & board = _game.m_board;

    std::cout << "You recived an extra Eter card.Place now!\n";
    std::cout << "Enter (x, y) coordinates for wizard action (0-indexed)\n";
    std::cin >> x >> y;

    if (x >= board.m_board.size() || y >= board.m_board.size())
        return false;

    if (board.m_board[x][y].back().getColor() != _player.getColor())
        return false;
    
    board.m_board[x][y].emplace_back( Card::Value::Eter, _player.getColor() );
    
    return true;
}

bool Wizard::WizardActions::moveStackOpponent(Player& _player, Game& _game) {
    size_t startX, startY, endX, endY;
    Game::Board& board = _game.m_board;

    std::cout << "Move an opponent's stack with your card on top to an empty position.\n";
    std::cout << "Enter coordinates of the stack:\n";
    std::cin >> startX >> startY;

    if (board.checkIndexes(startX,startY))
        return false;

    if (board.m_board[startX][startY].empty() || board.m_board[startX][startY].size() < 2)
        return false;

    if (board.m_board[startX][startY].back().getColor() == _player.getColor())
        return false;

    std::cout << "Enter coordinates for the destination of the stack:\n";
    std::cin >> endX >> endY;

    if (board.checkIndexes(endX,endY))
        return false;

    if (!board.m_board[endX][endY].empty())
        return false;


    board.m_board[endX][endY] = std::move(board.m_board[startX][startY]);
    board.m_board[startX][startY].clear();

    std::cout << "Stack moved successfully!\n";
    return true;
}

bool Wizard::WizardActions::moveEdge(Player& _player, Game& _game) {
    return true;
}
