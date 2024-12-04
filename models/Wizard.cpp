#include "Wizard.h"
#include "Board.h"
#include "Game.h"

bool Wizard::WizardActions::eliminateCard(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    std::cout << "Eliminate an opponent's card that covers your own.\n";
    std::cout << "Enter (x, y) coordinates for wizard action (0-indexed)\n";
    std::cin >> x >> y;

    if (x >= board.m_board.size() || y >= board.m_board.size())
        return false;

    if (board.m_board[y][x].size() < 2)
        return false;

    if (board.m_board[x][y].back().getColor() == _player.getColor())
        return false;

    if (const size_t secondLastcardValue = board.m_board[x][y].size() - 2;
        board.m_board[x][y][secondLastcardValue].getColor() != _player.getColor())
        return false;

    auto eliminatedCard = std::move(board.m_board[x][y].back());
    board.m_board[x][y].pop_back();

    return true;
}

bool Wizard::WizardActions::eliminateRow(Player &_player, Game &_game, const bool _check) {
    size_t index;
    char choice;
    Board &board = _game.m_board;

    std::cout << "Eliminate an entire row or column of stacks.\n";
    std::cout << "Enter 'r' for row or 'c' for column: ";

    do {
        std::cin >> choice;
    }
    while (tolower(choice) != 'r' && towlower(choice) != 'c');

    std::cout << "Enter the index (0-indexed): ";
    std::cin >> index;

    if (index >= board.m_board.size())
        return false;

    const size_t boardSize = board.m_board.size();
    size_t ownVisibleCards = 0;
    size_t nonEmptyStacks = 0;

    if (tolower(choice) == 'r') {
        const std::vector<std::vector<Card> > &row = board.m_board[index];
        for (const std::vector<Card> &stack: row) {
            if (!stack.empty()) {
                nonEmptyStacks++;
                const Card &topCard = stack.back();
                if (topCard.getColor() == _player.getColor() && !topCard.isIllusion())
                    ownVisibleCards++;
            }
        }
    }

    else {
        for (size_t i = 0; i < boardSize; ++i) {
            const std::vector<Card> &stack = board.m_board[i][index];
            if (!stack.empty()) {
                nonEmptyStacks++;
                const Card &topCard = stack.back();
                if (topCard.getColor() == _player.getColor() && !topCard.isIllusion())
                    ownVisibleCards++;
            }
        }
    }

    if (nonEmptyStacks < 3)
        return false;

    if (ownVisibleCards == 0)
        return false;

    std::vector<std::vector<Card> > savedSection;
    if (tolower(choice) == 'r') {
        savedSection = board.m_board[index];
        for (std::vector<Card> &stack: board.m_board[index])
            stack.clear();
    }

    else {
        savedSection.resize(boardSize);
        for (size_t i = 0; i < boardSize; ++i) {
            savedSection[i] = std::move(board.m_board[i][index]);
            board.m_board[i][index].clear();
        }
    }

    if (!board.checkBoardIntegrity()) {
        if (tolower(choice) == 'r') {
            board.m_board[index] = std::move(savedSection);
        } else {
            for (size_t i = 0; i < boardSize; ++i) {
                board.m_board[i][index] = std::move(savedSection[i]);
            }
        }
        return false;
    }

    return true;
}

bool Wizard::WizardActions::coverCard(Player &_player, Game &_game, const bool _check) {
    size_t x, y, cardValue;
    Board &board = _game.m_board;

    std::cout << "Cover an opponent's card with one of your own of strictly lower value.\n";
    std::cout << "Enter (x, y) coordinates for the target card (0-indexed) and the card value: ";
    std::cin >> x >> y >> cardValue;

    if (!board.checkIndexes(x, y))
        return false;

    std::vector<Card> &targetStack = board.m_board[x][y];
    if (targetStack.empty()) {
        return false;
    }

    const Card &target = targetStack.back();
    if (target.getColor() == _player.getColor())
        return false;

    if (cardValue >= _player.m_cards.size())
        return false;

    auto selectedCard = _player.useCard(static_cast<Card::Value>(cardValue));
    if (selectedCard == std::nullopt)
        return false;

    if (selectedCard->getColor() != _player.getColor()) {
        return false;
    }

    if (selectedCard->getValue() >= target.getValue()) {
        _player.returnCard(*selectedCard);
        return false;
    }

    targetStack.push_back(std::move(*selectedCard));

    return true;
}

bool Wizard::WizardActions::sinkHole(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    std::cout << "Transform an empty space on the board into a sinkhole.\n";
    std::cout << "Enter (x, y) coordinates for the empty space (0-indexed): ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y))
        return false;

    if (!board.m_board[y][x].empty())
        return false;

    if (board.checkHole(y, x))
        return false;

    Wizard::getInstance().setHole(std::make_pair(y, x));

    return true;
}

bool Wizard::WizardActions::moveStackOwn(Player &_player, Game &_game, const bool _check) {
    size_t startX, startY, endX, endY;
    Board &board = _game.m_board;

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

    if (board.checkBoardIntegrity())
        return true;

    board.m_board[startX][startY] = std::move(board.m_board[endX][endY]);
    board.m_board[endX][endY].clear();

    return false;
}

bool Wizard::WizardActions::extraEter(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    std::cout << "You received an extra Eter card.Place now!\n";
    std::cout << "Enter (x, y) coordinates for wizard action (0-indexed)\n";
    std::cin >> x >> y;

    if (board.checkIndexes(x,y))
        return false;

    if (!board.m_board[x][y].empty())
        return false;

    board.m_board[x][y].emplace_back(Card::Value::Eter, _player.getColor());

    return true;
}

bool Wizard::WizardActions::moveStackOpponent(Player &_player, Game &_game, const bool _check) {
    size_t startX, startY, endX, endY;
    Board &board = _game.m_board;

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

    if (board.checkBoardIntegrity())
        return true;

    board.m_board[startX][startY] = std::move(board.m_board[endX][endY]);
    board.m_board[endX][endY].clear();

    return false;
}

bool Wizard::WizardActions::moveEdge(Player& _player, Game& _game, const bool _check) {
    Board& board = _game.m_board; 
    std::cout << "Move edge of the playing field and move it to a different edge\n";
    char choice;
    std::cout << "Choose direction (w: up, a: left, s: down, d: right): ";
    std::cin >> choice;

    std::vector<std::vector<Card>> movedLine;

    switch (choice) {
    case 'w': {
        movedLine = std::move(board.m_board[0]);
        board.circularShiftUp();
        board.m_board[board.m_board.size() - 1] = std::move(movedLine);
        break;
    }
    case 'a': {
     
        for (auto& row : board.m_board) {
            movedLine.emplace_back(std::move(row[0]));
        }
        board.circularShiftLeft();
        for (size_t i = 0; i < board.m_board.size(); ++i) {
            board.m_board[i][board.m_board[i].size() - 1] = std::move(movedLine[i]);
        }
        break;
    }
    case 's': {
        movedLine = std::move(board.m_board[board.m_board.size() - 1]);
        board.circularShiftDown();
        board.m_board[0] = std::move(movedLine);
        break;
    }
    case 'd': {
        for (auto& row : board.m_board) {
            movedLine.emplace_back(std::move(row[row.size() - 1]));
        }
        board.circularShiftRight();
        for (size_t i = 0; i < board.m_board.size(); ++i) {
            board.m_board[i][0] = std::move(movedLine[i]);
        }
        break;
    }
    default: {
        return false;
    }
    }
    return true;
}
