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
    size_t x, y;
    Game::Board& board = _game.m_board;

    std::cout << "Moves horizontally or vertically any visible card on the board to a position adjacent to a card with a lower number.\n";
    std::cout << "Enter (x, y) coordinates for power action (0-indexed)\n";
    std::cin >> x >> y;

    if (board.m_board[x][y].empty()) {
        std::cout << "There is no card in this position!\n";
        return false;
    }
    Card& currentCard = board.m_board[x][y].back();
    std::vector<std::pair<size_t, size_t>> adjacents = {
       {x - 1, y}, {x + 1, y}, 
       {x, y - 1}, {x, y + 1} 
    };
    for (auto& adj : adjacents) {
        size_t row = adj.first;
        size_t col = adj.second;
        if (board.checkIndexes(row, col) && !board.m_board[row][col].empty()) {
            Card& adjacentCard = board.m_board[row][col].back();
            if (static_cast<size_t>(currentCard.getValue()) > static_cast<size_t>(adjacentCard.getValue())) {
                board.m_board[row][col].emplace_back(currentCard.getValue(), currentCard.getColor());
                board.m_board[x][y].pop_back();
            }
            return true;
        }
    }
    std::cout << "No valid adjacent position was found to move the card.\n";
    return false;
}

bool Power::PowerAction::mirage(Player& _player, Game& _game) {
    size_t x, y;
    Game::Board& board = _game.m_board;

    std::cout << "Replace your own placed illusion with another illusion.\n";
    std::cout << "Enter (x, y) coordinates for power action (0-indexed)\n";
    std::cin >> x >> y;
    
    ///isIlusionEnable

    if (!board.checkIndexes(x, y))
        return false;

    if (board.m_board[x][y].empty()){
        std::cout << "There is no illusion in this position!\n";
        return false;
    }

    auto& topCard = board.m_board[x][y].back();
    if (topCard.isIllusion() && topCard.getColor() == _player.getColor()) {
        topCard.resetIllusion();
        topCard.setColor(_player.getColor());
        return true;
    }
    std::cout << " Could not replace the ilusion!" << std::endl;
    return false;
}

bool Power::PowerAction::storm(Player& _player, Game& _game) {
    size_t x, y;
    Game::Board& board = _game.m_board;

    std::cout << "Remove from play any pile of cards containing 2 or more cards.\n";
    std::cout << "Enter (x, y) coordinates for power action (0-indexed)\n";
    std::cin >> x >> y;

    if (!board.checkIndexes(x,y))
        return false;
 
    if (!board.isAPile(y,x))
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

    if (board.checkIndexes(x,y) || board.checkIndexes(v, w))
        return false;
    
    if (!board.isAPile(y, x) || !board.isAPile(w, v))
        return false;
    
    std::swap(board.m_board[x][y], board.m_board[v][w]);

    return true;
}

bool Power::PowerAction::mist(Player& _player, Game& _game) {
    size_t x, y;
    Game::Board& board = _game.m_board;

    std::cout << "Play an extra illusion.\n"; 
    std::cout << "Enter (x, y) coordinates for power action (0-indexed)\n";
    std::cin >> x >> y;

    ///isIlusionEnable
    if (!board.checkIndexes(x, y))
        return false;

    if (!board.m_board[x][y].empty()) {
        std::cout << "There is a illusion in this position!\n";
        return false;
    }

    for (const auto& card : board.m_board[x][y]) {
        if (card.isIllusion() && card.getColor() == _player.getColor()) {
            std::cout << "You cannot have two illusions on the board at the same time!" << std::endl;
            return false; 
        }
    }

    ///board.m_board[x][y].emplace_back(static_cast<Card::Value>(Value), _player.getColor());
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
