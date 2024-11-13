#include "Power.h"

#include "Game.h"

bool Power::PowerAction::controlledExplosion(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::destruction(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::flame(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::fire(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::ash(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::spark(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::squall(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::gale(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::hurricane(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::gust(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::mirage(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::storm(Player& _player, Game& _game) {
    size_t x, y;
    Game::Board& board = _game.m_board;

    std::cout << "Remove from play any pile of cards containing 2 or more cards.\n";
    std::cout << "Enter (x, y) coordinates for power action (0-indexed)\n";
    std::cin >> x >> y;

    if (x >= board.m_board.size() || y >= board.m_board.size())
        return false;
 
    if (board.m_board[y][x].size() < 2)
        return false;

    auto eliminatedCard = std::move(board.m_board[x][y].back());
    board.m_board[x][y].pop_back();

    return true;
}

bool Power::PowerAction::tide(Player& _player, Game& _game) {
    size_t x, y,v,w;
    Game::Board& board = _game.m_board;

    std::cout << "Switch 2 pile of cards.\n";
    std::cout << "Enter (x, y) coordinates for power action (0-indexed)(FIRST Pile)\n";
    std::cin >> x >> y;
    std::cout << "Enter (x, y) coordinates for power action (0-indexed)(SECOND Pile)\n";
    std::cin >> v >> w;

    if (x >= board.m_board.size() || y >= board.m_board.size()|| v >= board.m_board.size() || w >= board.m_board.size())
        return false;
    
    if (board.m_board[y][x].size() < 2 || board.m_board[w][v].size() < 2)
        return false;
    
    std::swap(board.m_board[x][y], board.m_board[v][w]);

    return true;
}

bool Power::PowerAction::mist(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::wave(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::whirlpool(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::blizzard(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::waterfall(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::support(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::earthquake(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::crumble(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::border(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::avalanche(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::rock(Player& _player, Game& _game) {
    return true;
}
