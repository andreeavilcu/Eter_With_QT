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
    Game::Board& board = _game.m_board;
    size_t row;
    char direction;

    std::cout << "Waterfall!";
    std::cout << "Choose a row with at least 3 occupied positions.\n";
    std::cin >> row;

    if (row >= board.getSize()) {
        std::cout << "Invalid row selection";
        return false;
    }

    int occupiedPositions = 0;
    for (size_t col = 0; col < board.getSize(); ++col) {
        if (!board.m_board[row][col].empty())
            ++occupiedPositions;
    }

    if (occupiedPositions < 3) {
        std::cout << "Not enough occupied positions.\n";
        return false;
    }

    std::cout << "Choose the direction of the cascade(l for left, 1 for right)\n";
    std::cin >> direction;

    if (direction != 'l' && direction != 'r') {
        std::cout << "\n";
        return false;
    }

    std::vector<Card> mergedStack;

    if (direction == 'l') {
        for (size_t col = 0; col < board.getSize(); ++col) {
            while (!board.m_board[row][col].empty()) {
                mergedStack.push_back(std::move(board.m_board[row][col].back()));
                board.m_board[row][col].pop_back();
            }
        }
        board.m_board[row][0] = std::move(mergedStack);
    }
    else {
        for (size_t col = board.getSize() - 1; col >= 0; --col) {
            while (!board.m_board[row][col].empty()) {
                mergedStack.push_back(std::move(board.m_board[row][col].back()));
                board.m_board[row][col].pop_back();
            }
        }

        board.m_board[row][board.getSize() - 1] = std::move(mergedStack);
    }

    std::cout << "Cards have merged successfully.\n";
    return true;
}

bool Power::PowerAction::support(Player& _player, Game& _game) {
    size_t x, y;
    Game::Board& board = _game.m_board;

    std::cout << "Support!";
    std::cout << " Choose a card of value 1, 2 or 3 to increase its value by 1.\n";
    std::cout << "Enter the coordinates of the card:\n";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y)) {
        return false;
    }

    if (board.m_board[x][y].empty()) {
        return false;
    }

    if (board.m_board[x][y].back().isIllusion()) {
        return false;
    }

    Card& selectedCard = board.m_board[x][y].back();
    if (selectedCard.getColor() != _player.getColor()) {
        return false;
    }

    if (selectedCard.getValue() == Card::Value::Four || selectedCard.getValue() == Card::Value::Eter) {
        return false;
    }

    board.m_plus = { x,y };
    std::cout << "The value of the selected card has been increased by 1.\n";

    return true;
}


bool Power::PowerAction::earthquake(Player& _player, Game& _game) {
    

    Game::Board& board= _game.m_board;
    bool anyRemoved = false;
    

    for (size_t row = 0; row < board.m_board.size(); ++row) {
        for (size_t col = 0; col < board.m_board.size(); ++col) {
            if (board.m_board[row][col].back().getValue() == Card::Value::One && !board.m_board[row][col].back().isIllusion())
            {
                board.m_board[row][col].pop_back();
                anyRemoved = true;
            }

        }
    }

    if (anyRemoved) {
        std::cout << "Earthquake! All cards with the value 1 have been removed.\n";
    }
    else {
        std::cout << "There were no cards with the value 1 on the board.\n";
    }
    return anyRemoved;
}

bool Power::PowerAction::crumble(Player& _player, Game& _game) {
    size_t x, y;
    Game::Board& board = _game.m_board;

    std::cout << "Choose an opponent's card of value 2, 3, or 4 to decrease its value by 1.\n";
    std::cout << "Enter the coordinates of the card:\n";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y)) {
        return false;
    }

    if (board.m_board[x][y].empty()) {
        return false;
    }

    if (board.m_board[x][y].back().isIllusion()) {
        return false;
    }

    Card& selectedCard = board.m_board[x][y].back();
    if (selectedCard.getColor() == _player.getColor()) {
        return false;
    }

    //TO DO :daca cartea este acoperita sau întoarsă în mână isi pierde bonusul?


    if(selectedCard.getValue() == Card::Value::One || selectedCard.getValue() == Card::Value::Eter) {
        return false;
    }

    board.m_minus = { x,y };
    std::cout << "The value of the selected card has been decreased by 1.\n";

    return true;
}


bool Power::PowerAction::border(Player& _player, Game& _game) {
    size_t x, y;
    Game::Board& board = _game.m_board;

    std::cout << "Border!\n";
    std::cout << "Place a neutral card on the board to define at least one boundary.\n";
    std::cout << "Enter the coordinates for placing the neutral  card.\n";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y))
        return false;

    if ((x != 0 && x != board.m_board.size() - 1 && y != 0 && y != board.m_board.size() - 1)) {
        std::cout << "Invalid position! The neutral card must be placed on the boundary of the board.\n";
        return false;
    }

    //TO DO daca putem da shift puem pune cartea , daca nu return false
    // daca cartea nu defineste niciun border, iar return false
    Card neutralCard(Card::Value::Eter);
    board.placeCard(x, y, std::move(neutralCard));


    std::cout << "Now play a card from your hand.\n";
    size_t cardIndex;
    std::cin >> cardIndex;

    auto cardToPlay = _player.useCard(static_cast<Card::Value>(cardIndex));

    if (!cardToPlay) {
        std::cout << "Failed to play the card.\n";
        return false;
    }

    board.placeCard(x, y, std::move(*cardToPlay));
    std::cout << "The card was placed successfully!\n";
    return true;
}

bool Power::PowerAction::avalanche(Player& _player, Game& _game) {
    return true;
}

bool Power::PowerAction::rock(Player& _player, Game& _game) {
    size_t x, y;
    Game::Board& board = _game.m_board;
    
    std::cout << "Cover any illusion with a card (from your hand) without flipping the illusion face up.";
    std::cout << "Enter the coordinates for the illusion to cover:\n";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y))
        return false;

    if (!board.m_board[x][y].back().isIllusion())
        return false;

    std::cout << "Choose a card value to cover the illusion (0: Eter, 1: One, 2: Two, 3: Three, 4: Four):\n";
    int cardValue;
    std::cin >> cardValue;

    auto selectedCard = _player.useCard(static_cast<Card::Value>(cardValue));

    if (selectedCard == std::nullopt)
        return false;
    
    board.placeCard(x, y, std::move(*selectedCard));
   
   
    std::cout << "Illusion covered with a card.\n";
    return true;
}
