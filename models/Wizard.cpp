#include "Wizard.h"

#include "Game.h"

bool Wizard::WizardActions::eliminateCard(Player &_player, Game &_game) {
    size_t x, y;
    Game::Board &board = _game.m_board;

    std::cout << "Eliminate an opponent's card that covers your own.\n";
    std::cout << "Enter (x, y) coordinates for wizard action (0-indexed)\n";
    std::cin >> x >> y;

    if (x >= board.m_board.size() || y >= board.m_board.size())
        return false;

    if (board.m_board[y][x].size() < 2)
        return false;

    if (board.m_board[x][y].back().getColor() == _player.getColor())
        return false;

    if (const size_t secondLastCardIndex = board.m_board[x][y].size() - 2;
        board.m_board[x][y][secondLastCardIndex].getColor() != _player.getColor())
        return false;

    auto eliminatedCard = std::move(board.m_board[x][y].back());
    board.m_board[x][y].pop_back();

    return true;
}

bool Wizard::WizardActions::eliminateRow(Player &_player, Game &_game) {
    size_t rowIndex;
    Game::Board &board = _game.m_board;

    std::cout << "Eliminate an entire row of stacks\n";
    std::cout << "Enter the row index (0-indexed): ";
    std::cin >> rowIndex;

    if (rowIndex >= board.m_board.size())
        return false;

    const std::vector<std::vector<Card> > &row = board.m_board[rowIndex];
    size_t ownVisibleCards = 0;
    size_t nonEmptyStacks = 0;

    for (const std::vector<Card> &stack: row) {
        if (!stack.empty()) {
            nonEmptyStacks++;
            const Card& topCard = stack.back();
            if (stack.back().getColor() == _player.getColor() && !topCard.isIllusion())
                ownVisibleCards++;
        }
    }

    if (nonEmptyStacks < 3)
        return false;

    if (ownVisibleCards == 0)
        return false;

    for (std::vector<Card> &stack: board.m_board[rowIndex])
        stack.clear();
    return true;
}

bool Wizard::WizardActions::coverCard(Player &_player, Game &_game) {
    size_t x, y, cardIndex;
    Game::Board &board = _game.m_board;

    std::cout << "Cover an opponent's card with one of your own of strictly lower value.\n";
    std::cout << "Enter (x, y) coordinates for the target card (0-indexed) and the card value: ";
    std::cin >> x >> y >> cardIndex;

    if (!board.checkIndexes(x, y))
        return false;

    std::vector<Card> &targetStack = board.m_board[x][y];
    if (targetStack.empty()) {
        return false;
    }

    const Card &target = targetStack.back();
    if (target.getColor() == _player.getColor())
        return false;

    if (cardIndex >= _player.m_cards.size())
        return false;

    Card &selectedCard = _player.m_cards[cardIndex];
    if (selectedCard.getColor() != _player.getColor())
        return false;

    if (selectedCard.getValue() >= target.getValue())
        return false;

    targetStack.push_back(std::move(selectedCard));
    _player.m_cards.erase(_player.m_cards.begin() + cardIndex);

    return true;
}


bool Wizard::WizardActions::sinkHole(Player &_player, Game &_game) {
    size_t x, y;
    Game::Board &board = _game.m_board;

    std::cout << "Transform an empty space on the board into a sinkhole.\n";
    std::cout << "Enter (x, y) coordinates for the empty space (0-indexed): ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y))
        return false;

    if (!board.m_board[y][x].empty())
        return false;

    if (board.checkHole(y, x))
        return false;

    board.m_hole = std::make_pair(y, x);

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

bool Wizard::WizardActions::extraEter(Player &_player, Game &_game) {
    size_t x, y;
    Game::Board &board = _game.m_board;

    std::cout << "You recived an extra Eter card.Place now!\n";
    std::cout << "Enter (x, y) coordinates for wizard action (0-indexed)\n";
    std::cin >> x >> y;

    if (x >= board.m_board.size() || y >= board.m_board.size())
        return false;

    if (board.m_board[x][y].back().getColor() != _player.getColor())
        return false;

    board.m_board[x][y].emplace_back(Card::Value::Eter, _player.getColor());

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

bool Wizard::WizardActions::moveEdge(Player &_player, Game &_game) {
    return true;
}
