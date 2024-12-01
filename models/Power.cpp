#include "Power.h"
#include "Board.h"
#include "Game.h"

bool Power::PowerAction::controlledExplosion(Player& _player, Game& _game) {
    Board &board = _game.m_board;

    auto explosionEffects = Explosion::getInstance().generateExplosion(board.getSize());
    auto returnedCards = board.useExplosion(explosionEffects);

    auto &opponent = _player.getColor() == Card::Color::Player1 ? _game.m_player2 : _game.m_player1;

    for (auto &card: returnedCards)
        if (card.getColor() == _player.getColor())
            _player.returnCard(card);

        else
            opponent.returnCard(card);

    _game.m_playedExplosion = true;

    return true;
}

bool Power::PowerAction::destruction(Player& _player, Game& _game) {
    Board& board = _game.m_board;

    auto& opponent = _player.getColor() == Card::Color::Player1 ? _game.m_player2 : _game.m_player1;

    auto [lastRow, lastCol] = opponent.getLastPlacedCard();

    if (lastRow == -1 || lastCol == -1) {
        return false;
    }

    if (!board.checkIndexes(lastRow, lastCol) || board.m_board[lastRow][lastCol].empty()) {
        return false;
    }

    auto& stack = board.m_board[lastRow][lastCol];
    Card affectedCard = std::move(stack.back());
    stack.pop_back();

    if (!board.checkBoardIntegrity()) {
        stack.push_back(std::move(affectedCard));
        return false;
    }

    return true;
}

bool Power::PowerAction::flame(Player& _player, Game& _game) {
    Board& board = _game.m_board;

    size_t illusionRow = -1, illusionCol = -1;
    bool illusionFound = false;

    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            if (board.checkIllusion(row, col, _player.getColor() == Card::Color::Player1 ? Card::Color::Player2 : Card::Color::Player1)) {
                illusionRow = row;
                illusionCol = col;
                illusionFound = true;
                break;
            }
        }
        if (illusionFound) break;
    }

    if (illusionFound) {
        board.resetIllusion(illusionRow, illusionCol);
    }

    size_t x, y, int_value;
    std::cin >> x >> y >> int_value;

    if (!_game.checkPartial(x, y, int_value, 0)) {
        return false;
    }

    auto playedCard = _player.useCard(static_cast<Card::Value>(int_value));
    if (!playedCard) {
        return false;
    }

    board.placeCard(x, y, std::move(*playedCard));
    _player.placeCard(x, y);
    return true;
}

bool Power::PowerAction::lava(Player& _player, Game& _game) {
    size_t chosenValue;
    std::cin >> chosenValue;

    if (chosenValue < 1 || chosenValue > 4) {
        return false;
    }

    Card::Value targetValue = static_cast<Card::Value>(chosenValue);

    size_t count = 0;
    Board& board = _game.m_board;
    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            if (!board.m_board[row][col].empty() &&
                board.m_board[row][col].back().getValue() == targetValue &&
                !board.m_board[row][col].back().isIllusion()) {
                ++count;
                }
        }
    }

    if (count < 2) {
        return false;
    }

    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            if (!board.m_board[row][col].empty()) {
                Card topCard = board.m_board[row][col].back();
                if (topCard.getValue() == targetValue && !topCard.isIllusion()) {
                    Card::Color cardOwner = topCard.getColor();

                    board.m_board[row][col].pop_back();

                    if (cardOwner == Card::Color::Player1) {
                        _game.m_player1.returnCard(topCard);
                    } else if (cardOwner == Card::Color::Player2) {
                        _game.m_player2.returnCard(topCard);
                    }
                }
            }
        }
    }

    return true;
}

bool Power::PowerAction::ash(Player& _player, Game& _game) {
    Board& board = _game.m_board;

    const auto& eliminatedCards = _player.getEliminatedCards();
    if (eliminatedCards.empty()) {
        return false;
    }

    for (size_t i = 0; i < eliminatedCards.size(); ++i) {
        std::cout << i + 1 << ": " << eliminatedCards[i] << std::endl;
    }

    size_t cardIndex;
    std::cin >> cardIndex;

    if (cardIndex < 1 || cardIndex > eliminatedCards.size()) {
        return false;
    }

    Card chosenCard = eliminatedCards[cardIndex - 1];

    _player.getEliminatedCards().erase(_player.getEliminatedCards().begin() + 1);

    size_t x, y;
    std::cout << "Enter (x, y) coordinates to place the card (0-indexed): ";
    std::cin >> x >> y;

    if(!board.checkIndexes(x, y) || !board.checkValue(x, y, chosenCard.getValue())) {
        _player.addEliminatedCard(chosenCard);
        return false;
    }

    board.placeCard(x, y, std::move(chosenCard));
    _player.placeCard(x, y);
    return true;
}

bool Power::PowerAction::spark(Player& _player, Game& _game) {
    Board& board = _game.m_board;

    std::vector<std::tuple<size_t, size_t, Card>> coveredCards;

    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            const auto& stack = board.m_board[row][col];
            if(stack.size() >= 2) {
                const Card& topCard = stack.back();
                const Card& playerCard = stack[stack.size() - 2];

                if(playerCard.getColor() == _player.getColor() &&
                    topCard.getColor() != _player.getColor()) {
                    coveredCards.emplace_back(row, col, playerCard);
                }
            }
        }
    }

    if(coveredCards.empty()) {
        return false;
    }

    for(size_t i = 0; i < coveredCards.size(); ++i) {
        auto [row, col, card] = coveredCards[i];
        std::cout << i + 1 << ": Card" << static_cast<int>(card.getValue()) << " at position (" << row << ", " << col << ")\n";
    }

    size_t choice;
    std::cin >> choice;

    if (choice < 1 || choice > coveredCards.size()) {
        return false;
    }

    auto [origRow, origCol, chosenCard] = coveredCards[choice -1 ];

    auto& originalStack = board.m_board[origRow][origCol];
    originalStack.erase(originalStack.end() - 2);

    size_t newRow, newCol;
    std::cout << "Enter the new position (x, y) to place the card (0-indexed): ";
    std::cin >> newRow >> newCol;

    if(!board.checkIndexes(newRow, newCol) ||
        !board.checkValue(newRow, newCol, chosenCard.getValue())) {
        originalStack.insert(originalStack.end() - 1, chosenCard);
        return false;
    }

    board.placeCard(newRow, newCol, std::move(chosenCard));
    _player.placeCard(newRow, newCol);

    return true;
}

bool Power::PowerAction::squall(Player& _player, Game& _game) {
    Board& board = _game.m_board;

    std::vector<std::tuple<size_t, size_t, Card>> visibleOpponentCards;

    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            if (!board.m_board[row][col].empty()) {
                const Card& topCard = board.m_board[row][col].back();

                if (topCard.getColor() != _player.getColor() && !topCard.isIllusion()) {
                    visibleOpponentCards.emplace_back(row, col, topCard);
                }
            }
        }
    }

    if (visibleOpponentCards.empty()) {
        return false;
    }

    std::cout << "Choose a card to return to opponent's hand:\n";
    for (size_t i = 0; i < visibleOpponentCards.size(); ++i) {
        auto [row, col, card] = visibleOpponentCards[i];
        std::cout << i + 1 << ": Card " << static_cast<int>(card.getValue())
                  << " at position (" << row << ", " << col << ")\n";
    }

    size_t choice;
    std::cin >> choice;

    if (choice < 1 || choice > visibleOpponentCards.size()) {
        return false;
    }

    auto [selectedRow, selectedCol, selectedCard] = visibleOpponentCards[choice - 1];

    if (!board.checkBoardIntegrity()) {
        board.m_board[selectedRow][selectedCol].push_back(selectedCard);
        return false;
    }

    board.m_board[selectedRow][selectedCol].pop_back();

    auto& opponent = (_player.getColor() == Card::Color::Player1)
                     ? _game.m_player2
                     : _game.m_player1;
    opponent.returnCard(selectedCard);

    return true;
}


bool Power::PowerAction::gale(Player& _player, Game& _game) {
    Board& board = _game.m_board;

    std::vector<std::tuple<Card, size_t, size_t>> coveredCards;

    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            const auto& stack = board.m_board[row][col];
            if (stack.size() > 1) {
                for (size_t i = 0; i < stack.size() - 1; ++i) {
                    coveredCards.emplace_back(stack[i], row, col);
                }
            }
        }
    }

    if (coveredCards.empty()) {
        return false;
    }

    for (const auto& [card, row, col] : coveredCards) {
        auto& stack = board.m_board[row][col];

        auto it = std::find(stack.begin(), stack.end(), card);
        if (it != stack.end()) {
            stack.erase(it);
        }

        if (card.getColor() == Card::Color::Player1) {
            _game.m_player1.returnCard(card);
        } else if (card.getColor() == Card::Color::Player2) {
            _game.m_player2.returnCard(card);
        }
    }

    return true;
}


bool Power::PowerAction::hurricane(Player& _player, Game& _game) {
    Board& board = _game.m_board;
    std::cout << "Shift a full row or column in the desired direction.\n";

    char typeChoice, directionChoice;
    int index;

    std::cout << "Choose type to shift (r: row, c: column): ";
    do {
        std::cin >> typeChoice;
        if (typeChoice != 'r' && typeChoice != 'c')
            std::cout << "Invalide choise!Choose type to shift (r: row, c: column):\n ";
    } while (typeChoice != 'r' && typeChoice != 'c');

    do {
        std::cout << "Choose direction (w: up, a: left, s: down, d: right): ";
        std::cin >> directionChoice;

        if ((typeChoice == 'r' && (directionChoice != 'a' && directionChoice != 'd')) ||
            (typeChoice == 'c' && (directionChoice != 'w' && directionChoice != 's'))) {
            std::cerr << "Invalid direction for the chosen type! Try again.\n";
        }
        else
            break;
    } while (true);

    if (typeChoice == 'r')
        std::cout << "Choose row index to shift (0 to " << board.m_board.size() - 1 << "): ";
    else
        std::cout << "Choose column index to shift (0 to " << board.m_board[0].size() - 1 << "): ";

    std::cin >> index;

    if (typeChoice == 'r' && (index < 0 || index >= board.m_board.size()))
        return false;

    if (typeChoice == 'c' && (index < 0 || index >= board.m_board[0].size()))
        return false;

    /*if (typeChoice == 'r') {
        auto& row = board.m_board[index];
        std::vector<Card> shiftedRow(row.size());

        if (directionChoice == 'a') {
            Card outCard = std::move(row[0]);
            for (size_t i = 0; i < row.size() - 1; ++i) {
                shiftedRow[i] = std::move(row[i + 1]);
            }
            shiftedRow[row.size() - 1] = std::move(outCard);
            row = std::move(shiftedRow);

            _player.returnCard(outCard);
        }
        else if (directionChoice == 'd') {
            Card outCard = std::move(row[row.size() - 1]);
            for (size_t i = row.size() - 1; i > 0; --i) {
                shiftedRow[i] = std::move(row[i - 1]);
            }
            shiftedRow[0] = std::move(outCard);
            row = std::move(shiftedRow);

            _player.returnCard(outCard);
        }

    }
    else if (typeChoice == 'c') {
        std::vector<Card> column(board.m_board.size());
        for (size_t i = 0; i < board.m_board.size(); ++i) {
            column[i] = std::move(board.m_board[i][index]);
        }

        std::vector<Card> shiftedColumn(column.size());
        if (directionChoice == 's') {
            Card outCard = std::move(board.m_board[board.m_board.size() - 1][index]);
            for (size_t i = board.m_board.size() - 1; i > 0; --i) {
                board.m_board[i][index] = std::move(board.m_board[i - 1][index]);
            }
            board.m_board[0][index] = std::move(outCard);

            _player.returnCard(outCard);
        }

        else if (directionChoice == 'w') {
            Card outCard = std::move(board.m_board[0][index]);
            for (size_t i = 0; i < board.m_board.size() - 1; ++i) {
                board.m_board[i][index] = std::move(board.m_board[i + 1][index]);
            }
            board.m_board[board.m_board.size() - 1][index] = std::move(outCard);


            _player.returnCard(outCard);
        }
        }
    */

    return true;
}


bool Power::PowerAction::gust(Player& _player, Game& _game) {
    size_t x, y;
    Board& board = _game.m_board;

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
    Board& board = _game.m_board;

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
    Board& board = _game.m_board;

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
    Board& board = _game.m_board;

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
    Board& board = _game.m_board;

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
    size_t x, y;
    Board& board = _game.m_board;

    std::cout << "Move the stack to an adjacent empty position. Play a card on the new empty position.\n";
    std::cout << "Enter (x, y) coordinates for power action (0-indexed): ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y)) 
        return false;
    
    if (!board.isAPile(y, x)) 
        return false;
    
    const std::array<std::pair<int, int>, 8> directions = { {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    } };

    std::vector<std::pair<size_t, size_t>> emptyNeighbours;
    for (const auto& direction : directions) {
        int newRow = static_cast<int>(y) + direction.first;
        int newCol = static_cast<int>(x) + direction.second;

        if (newRow >= 0 && newRow < board.getSize() &&  newCol >= 0 && newCol < board.getSize() && board.m_board[newRow][newCol].empty()) {
            emptyNeighbours.emplace_back(newRow, newCol);
        }
    }
    if (emptyNeighbours.empty()) 
        return false;
    
    std::cout << "Available positions:\n";
    for (size_t i = 0; i < emptyNeighbours.size(); ++i) 
        std::cout << i + 1 << ": (" << emptyNeighbours[i].first << ", " << emptyNeighbours[i].second << ")\n";
    
    size_t choice;
    std::cout << "Choose a position by number: ";
    std::cin >> choice;

    if (choice < 1 || choice > emptyNeighbours.size()) {
        std::cout << "Invalid choice.\n";
        return false;
    }

    size_t Y = emptyNeighbours[choice - 1].first;
    size_t X = emptyNeighbours[choice - 1].second;

    board.m_board[Y][X] = std::move(board.m_board[y][x]);
    board.m_board[y][x].clear();

    /// board.playCard();

    return true;
}

bool Power::PowerAction::whirlpool(Player& _player, Game& _game) {
    size_t x, y;
    Board& board = _game.m_board;

    std::cout << "Move 2 cards from the same row or column, but separated by an empty space, onto that empty space. The card with the higher number is placed on top, and in case of a tie, the player chooses.\n";
    std::cout << "Enter (x, y) coordinates for the empty spot (0-indexed): ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y) || !board.m_board[x][y].empty()) {
        std::cout << "Invalid coordinates or not an empty spot.\n";
        return false;
    }

    size_t leftIndex = static_cast<size_t>(-1), rightIndex = static_cast<size_t>(-1);
    size_t topIndex = static_cast<size_t>(-1), bottomIndex = static_cast<size_t>(-1);

    if (y > 0 && !board.m_board[x][y - 1].empty())
        leftIndex = y - 1;
    if (y < board.m_board[x].size() - 1 && !board.m_board[x][y + 1].empty())
        rightIndex = y + 1;

    if (x > 0 && !board.m_board[x - 1][y].empty())
        topIndex = x - 1;
    if (x < board.m_board.size() - 1 && !board.m_board[x + 1][y].empty())
        bottomIndex = x + 1;

    bool hasRowCards = (leftIndex != static_cast<size_t>(-1) && rightIndex != static_cast<size_t>(-1));
    bool hasColumnCards = (topIndex != static_cast<size_t>(-1) && bottomIndex != static_cast<size_t>(-1));

    if (!hasRowCards && !hasColumnCards)
        return false;

    bool useRow = hasRowCards;
    if (hasRowCards && hasColumnCards) {
        int choice;
        do {
            std::cout << "Choose the direction to perform the operation:\n";
            std::cout << "1. Row\n";
            std::cout << "2. Column\n";
            std::cin >> choice;
            if (choice != 1 && choice != 2)
                std::cout << "Invalid option!\n";
        } while (choice != 1 && choice != 2);
        useRow = (choice == 1);
    }

    /*auto moveCards = [&](Card* card1, size_t index1, Card* card2, size_t index2) -> bool {
            if (card1->getValue() == card2->getValue() && card1->getColor() != card2->getColor()) {
                int choice;
                auto colorCard1 = card1->getColor();
                auto colorCard2 = card2->getColor();

                do {
                    std::cout << "Cards have the same value. Choose which card goes on top:\n";
                    std::cout << "1. First card (" << colorCard1 << ")\n";
                    std::cout << "2. Second card (" << colorCard2 << ")\n";
                    std::cin >> choice;

                    if (choice != 1 && choice != 2)
                        std::cout << "Invalid choice. Please choose 1 or 2.\n";
                } while (choice != 1 && choice != 2);

                if (choice == 1) {
                    board.m_board[x][y].push_back(*card1);
                    board.m_board[x][index1].pop_back();
                }
                else {
                    board.m_board[x][y].push_back(*card2);
                    board.m_board[x][index2].pop_back();
                }
            }
            else {
                if (card1->getValue() > card2->getValue()) {
                    board.m_board[x][y].push_back(*card1);
                    board.m_board[x][index1].pop_back();
                }
                else {
                    board.m_board[x][y].push_back(*card2);
                    board.m_board[x][index2].pop_back();
                }
            }

            };

        if (useRow) {
            return moveCards(board.m_board[x][leftIndex], leftIndex, board.m_board[x][rightIndex], rightIndex);
        }
        else {
            return moveCards(board.m_board[topIndex][y], topIndex, board.m_board[bottomIndex][y], bottomIndex);
        }*/

    return false;
}

bool Power::PowerAction::tsunami(Player& _player, Game& _game) {
    char line;
    Board& board = _game.m_board;
    std::cout << "Tsunami!";
    std::cout << "Choose a row ('r') ora column ('c') to restrict:\n";
    std::cin >> line;

    if (line != 'r' && line != 'c')
        return false;

    size_t index;
    std::cout << "Enter the index of the row/column to restrict:\n";
    std::cin >> index;

    if (index >= board.getSize())
        return false;

    bool hasFreeSpace = false;
    for (size_t i = 0; i < board.getSize(); ++i) {
        for (size_t j = 0; j < board.getSize(); ++j) {
            if (board.m_board[i][j].empty() &&
                !((line == 'r' && i == index) || (line == 'c' && j == index))) {
                hasFreeSpace = true;
                break;
            }
        }
        if (hasFreeSpace) break;
    }

    if (!hasFreeSpace) {
        std::cout << "The opponent must have at least one free space to play a card outside the restricted row/column.\n";
        return false;
    }

    if (line == 'r') {
        board.m_restrictedRow = index;
    }
    else {
        board.m_restrictedCol = index;
    }

    board.m_justBlocked = true;
    return true;
}

bool Power::PowerAction::waterfall(Player& _player, Game& _game) {
    Board& board = _game.m_board;
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
    Board& board = _game.m_board;

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
    Board& board= _game.m_board;
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
    Board& board = _game.m_board;

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

    //TODO :daca cartea este acoperita sau întoarsă în mână isi pierde bonusul?


    if(selectedCard.getValue() == Card::Value::One || selectedCard.getValue() == Card::Value::Eter) {
        return false;
    }

    board.m_minus = { x,y };
    std::cout << "The value of the selected card has been decreased by 1.\n";

    return true;
}

bool Power::PowerAction::border(Player& _player, Game& _game) {
    size_t x, y;
    Board& board = _game.m_board;

    std::cout << "Border!\n";
    std::cout << "Place a neutral card on the board to define at least one boundary.\n";
    std::cout << "Enter the coordinates for placing the neutral  card.\n";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y))
        return false;

    if ((x != 0 && x != board.m_board.size() - 1 && y != 0 && y != board.m_board.size() - 1)) {
        return false;
    }


    bool canShift = false;
    if (x == 0) { 
        board.circularShiftUp();
        canShift = true;
    }
    else if (x == board.m_board.size() - 1) { 
        board.circularShiftDown();
        canShift = true;
    }
    else if (y == 0) { 
        board.circularShiftLeft();
        canShift = true;
    }
    else if (y == board.m_board.size() - 1) { 
        board.circularShiftRight();
        canShift = true;
    }

    if (!canShift) {
        return false;
    }

    bool definesBoundary = false;
    if (x == 0 || x == board.m_board.size() - 1 || y == 0 || y == board.m_board.size() - 1) {
        definesBoundary = true;
    }

    if (!definesBoundary) {
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
    return true;
}

bool Power::PowerAction::avalanche(Player& _player, Game& _game) {
    size_t x1, y1, x2, y2;
    Board& board = _game.m_board;

    std::cout << "Avalanche!\n";
    std::cout << "Choose two adiacent stacks on the board.\n";
    std::cout << "Enter the coordinates for the first stack.\n";
    std::cin >> x1 >> y1;
    std::cout << "Eneter the coordinates for the second stack/.\n";
    std::cin >> x2 >> y2;

    if (!board.checkIndexes(x1, y1) && !board.checkIndexes(x2, y2))
        return false;

    if (!(x1 == x2 && abs(static_cast<int>(y1) - static_cast<int>(y2)) == 1) || (y1 == y2 && abs(static_cast<int>(x1) - static_cast<int>(x2)) == 1))
        return false;

    if (x1 == x2) {
        if (y1 < y2) {
            if (y1 > 0 && board.m_board[x1][y1 - 1].empty()) {
                board.m_board[x1][y1 - 1] = std::move(board.m_board[x1][y1]);
                board.m_board[x1][y1] = std::move(board.m_board[x1][y2]);
                board.m_board[x1][y2].clear();
            }
            else if (y2 < board.getSize() - 1 && board.m_board[x1][y2 + 1].empty()) {
                board.m_board[x1][y2 + 1] = std::move(board.m_board[x1][y2]);
                board.m_board[x1][y2] = std::move(board.m_board[x1][y1]);
                board.m_board[x1][y1].clear();
            }
            else
                return false;
        }
    }
    else if (y1 == y2) {
        if (x1 < x2) {
            if (x1 > 0 && board.m_board[x1 - 1][y1].empty()) {
                board.m_board[x1 - 1][y1] = std::move(board.m_board[x1][y1]);
                board.m_board[x1][y1] = std::move(board.m_board[x2][y1]);
                board.m_board[x2][y1].clear();
            }
            else if (x2 < board.getSize() - 1 && board.m_board[x2 + 1][y1].empty()) {
                board.m_board[x2 + 1][y1] = std::move(board.m_board[x2][y1]);
                board.m_board[x2][y1] = std::move(board.m_board[x1][y1]);
                board.m_board[x1][y1].clear();
            }
            else
                return false;
        }
    }

    return true;
}

bool Power::PowerAction::rock(Player& _player, Game& _game) {
    size_t x, y;
    Board& board = _game.m_board;
    
    std::cout << "Rock.\n";
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
