#include "../Actions/Power.h"
#include "../GameMechanics/Board.h"
#include "../GameMechanics/Game.h"

cardPosition Power::getMinus(const Board &_board) const {
    return _board.findCardIndexes(m_minus);
}

void Power::setMinus(const cardPosition _position, Game &_game) {
    m_minus = &_game.getBoard().getBoard()[_position.x][_position.y][_position.z];
}

cardPosition Power::getPlus(const Board &_board) const {
    return _board.findCardIndexes(m_plus);
}

void Power::setPlus(const cardPosition _position, Game &_game) {
    this->m_plus = &_game.getBoard().getBoard()[_position.x][_position.y][_position.z];
}

nlohmann::json Power::serialize(Game &_game) {
    nlohmann::json json;

    json["minusX"] = _game.getBoard().findCardIndexes(m_minus).x;
    json["minusY"] = _game.getBoard().findCardIndexes(m_minus).y;
    json["minusZ"] = _game.getBoard().findCardIndexes(m_minus).z;

    json["plusX"] = _game.getBoard().findCardIndexes(m_plus).x;
    json["plusY"] = _game.getBoard().findCardIndexes(m_plus).y;
    json["plusZ"] = _game.getBoard().findCardIndexes(m_plus).z;

    json["restrictedRow"] = this->m_restrictedRow;
    json["restrictedCol"] = this->m_restrictedCol;

    json["justBlocked"] = this->m_justBlocked;

    return json;
}

bool Power::PowerAction::controlledExplosion(Player &_player, Game &_game, const bool _check) {
    if (!_game.m_explosionAllowed) return false;

    Board &board = _game.m_board;

    board.useExplosion(_game.m_returnedCards, _game.m_eliminatedCards);
    _game.m_playedExplosion = true;

    return true;
}

bool Power::PowerAction::destruction(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;

    auto &opponent = _player.getColor() == Card::Color::Red ? _game.m_player2 : _game.m_player1;

    auto [lastRow, lastCol, cardHeight] = board.findCardIndexes(opponent.getLastPlacedCard());

    auto &stack = board.m_board[lastRow][lastCol];
    Card affectedCard = std::move(stack[cardHeight]);
    stack.erase(stack.begin() + cardHeight);

    if (!board.checkBoardIntegrity()) {
        stack.insert(stack.begin() + cardHeight, std::move(affectedCard));
        return false;
    }

    _game.m_eliminatedCards.push_back(std::move(affectedCard));

    return true;
}

bool Power::PowerAction::flame(Player &_player, Game &_game, const bool _check) {
    if (!_game.m_illusionsAllowed) return false;

    Board &board = _game.m_board;

    bool illusionFound = false;

    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            if (board.checkIllusion(row, col, _player.getColor() == Card::Color::Red
                                                  ? Card::Color::Blue
                                                  : Card::Color::Red)) {
                board.m_board[row][col].back().resetIllusion();
                illusionFound = true;
                break;
            }
        }

        if (illusionFound) break;
    }

    if (!illusionFound) return false;

    board.printBoard();

    std::cout << "Now place a card on any position on the board.\n";
    return _player.playCard(_game); // TODO: no checks maybe? check power desc
}

bool Power::PowerAction::lava(Player &_player, Game &_game, const bool _check) {
    size_t chosenValue;

    std::cout << "Lava: Select a value with the condition that at least 2 cards of that value are visible.\n";
    std::cin >> chosenValue;

    if (chosenValue > 4) return false;

    auto targetValue = static_cast<Card::Value>(chosenValue);

    size_t count = 0;
    Board &board = _game.m_board;
    for (size_t row = 0; row < board.getSize(); ++row)
        for (size_t col = 0; col < board.getSize(); ++col)
            if (!board.m_board[row][col].empty() &&
                board.m_board[row][col].back().getValue() == targetValue &&
                !board.m_board[row][col].back().isIllusion()) { ++count; }

    if (count < 2) return false;

    for (size_t row = 0; row < board.getSize(); ++row)
        for (size_t col = 0; col < board.getSize(); ++col)
            if (!board.m_board[row][col].empty()) {
                Card &topCard = board.m_board[row][col].back();

                if (topCard.getValue() == targetValue && !topCard.isIllusion()) {
                    _game.m_returnedCards.push_back(std::move(topCard));
                    board.m_board[row][col].pop_back();
                }
            }

    return true;
}

bool Power::PowerAction::ash(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;

    auto &eliminatedCards = _game.m_eliminatedCards;
    if (eliminatedCards.empty()) return false;

    for (size_t i = 0; i < eliminatedCards.size(); ++i)
        if (eliminatedCards[i].getColor() == _player.getColor())
            std::cout << i + 1 << ": " << eliminatedCards[i] << std::endl;

    size_t cardIndex;
    std::cout << "Select the card index: ";
    std::cin >> cardIndex;

    if (cardIndex < 1 || cardIndex > eliminatedCards.size()) return false;

    Card &chosenCard = eliminatedCards[cardIndex - 1];

    if (chosenCard.getColor() != _player.getColor()) return false;

    size_t x, y;
    std::cout << "Enter (x, y) coordinates to place the card (0-indexed): ";
    std::cin >> x >> y;

    if (!board.checkPartial(x, y, static_cast<size_t>(chosenCard.getValue()))) return false;

    _game.m_eliminatedCards.erase(_game.m_eliminatedCards.begin() + 1);

    board.placeCard(x, y, std::move(chosenCard));

    if (!board.checkBoardIntegrity()) {
        chosenCard = std::move(board.m_board[x][y].back());
        board.m_board[x][y].pop_back();

        return false;
    }

    _player.setLastPlacedCard(_game.getBoard().getBoard()[x][y].back());
    return true;
}

bool Power::PowerAction::spark(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;

    std::vector<std::tuple<size_t, size_t, Card> > coveredCards;

    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            const auto &stack = board.m_board[row][col];
            if (stack.size() >= 2) {
                const Card &topCard = stack.back();
                const Card &playerCard = stack[stack.size() - 2];

                if (playerCard.getColor() == _player.getColor() &&
                    topCard.getColor() != _player.getColor()) {
                    coveredCards.emplace_back(row, col, playerCard);
                }
            }
        }
    }

    if (coveredCards.empty()) return false;

    for (size_t i = 0; i < coveredCards.size(); ++i) {
        auto [row, col, card] = coveredCards[i];
        std::cout << i + 1 << ": " << card << "at position (" << row << ", " << col << ")\n";
    }

    size_t choice;
    std::cout << "Select the card index: ";
    std::cin >> choice;

    if (choice < 1 || choice > coveredCards.size()) return false;

    auto [origRow, origCol, chosenCard] = coveredCards[choice - 1];

    size_t newRow, newCol;
    std::cout << "Enter the new position (x, y) to place the card (0-indexed): ";
    std::cin >> newRow >> newCol;

    if (!board.checkPartial(newRow, newCol, static_cast<size_t>(chosenCard.getValue()))) {
        return false;
    }

    auto &originalStack = board.m_board[origRow][origCol];
    originalStack.erase(originalStack.end() - 2);
    board.placeCard(newRow, newCol, std::move(chosenCard));

    if (!board.checkBoardIntegrity()) {
        auto card = std::move(board.m_board[newRow][newCol].back());
        board.m_board[newRow][newCol].pop_back();

        board.m_board[origRow][origCol].push_back(std::move(card));

        return false;
    }

    _player.setLastPlacedCard(_game.getBoard().getBoard()[newRow][newCol].back());
    return true;
}

bool Power::PowerAction::squall(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;

    std::vector<std::tuple<size_t, size_t, Card> > visibleOpponentCards;

    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            if (!board.m_board[row][col].empty()) {
                const Card &topCard = board.m_board[row][col].back();

                if (topCard.getColor() != _player.getColor() && !topCard.isIllusion()) {
                    visibleOpponentCards.emplace_back(row, col, topCard);
                }
            }
        }
    }

    if (visibleOpponentCards.empty()) return false;

    std::cout << "Squall: Choose a card to return to opponent's hand:\n";
    for (size_t i = 0; i < visibleOpponentCards.size(); ++i) {
        auto [row, col, card] = visibleOpponentCards[i];
        std::cout << i + 1 << ": " << card << " at position (" << row << ", " << col << ")\n";
    }

    size_t choice;
    std::cout << "Select the card index: ";
    std::cin >> choice;

    if (choice < 1 || choice > visibleOpponentCards.size()) return false;

    auto [selectedRow, selectedCol, selectedCard] = visibleOpponentCards[choice - 1];

    auto card = std::move(board.m_board[selectedRow][selectedCol].back());
    board.m_board[selectedRow][selectedCol].pop_back();

    if (!board.checkBoardIntegrity()) {
        board.m_board[selectedRow][selectedCol].push_back(std::move(card));
        return false;
    }

    _game.m_returnedCards.push_back(std::move(card));
    return true;
}

bool Power::PowerAction::gale(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;

    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            auto &stack = board.m_board[row][col];
            if (stack.empty()) continue;

            auto topCard = std::move(stack.back());
            stack.pop_back();

            for (auto &card: stack)
                _game.m_returnedCards.emplace_back(std::move(card));

            stack.clear();
            stack.emplace_back(std::move(topCard));
        }
    }

    return true;
}

bool Power::PowerAction::hurricane(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;
    std::cout << "Hurricane: Shift a full row or column in the desired direction.\n";

    char typeChoice, directionChoice;
    size_t index;

    std::cout << "Choose type to shift (r: row, c: column): ";
    while (std::cin >> typeChoice && tolower(typeChoice) != 'r' && tolower(typeChoice != 'c')) {
        std::cout << "Invalid choice! Try again: ";
    }

    typeChoice = tolower(typeChoice);

    if (typeChoice == 'r') {
        std::cout << "Choose row index to shift (0 to " << board.m_board.size() - 1 << "): ";
    } else {
        std::cout << "Choose column index to shift (0 to " << board.m_board[0].size() - 1 << "): ";
    }
    std::cin >> index;

    if (index >= board.m_board.size()) return false;

    if (typeChoice == 'r') {
        for (const auto &stack: board.m_board[index])
            if (stack.empty()) return false;
    } else {
        for (auto &i: board.m_board)
            if (i[index].empty()) return false;
    }

    do {
        std::cout << "Choose direction (w: up, a: left, s: down, d: right): ";
        std::cin >> directionChoice;
        directionChoice = tolower(directionChoice);

        if ((typeChoice == 'r' && (directionChoice != 'a' && directionChoice != 'd')) ||
            (typeChoice == 'c' && (directionChoice != 'w' && directionChoice != 's'))) {
            std::cout << "Invalid direction for the chosen type! Try again.\n";
        } else {
            break;
        }
    } while (true);

    if (typeChoice == 'r') {
        auto &row = board.m_board[index];

        if (directionChoice == 'a') {
            auto outStack = std::move(row[0]);

            for (size_t i = 0; i < row.size() - 1; ++i)
                row[i] = std::move(row[i + 1]);

            row[row.size() - 1].clear();

            for (auto &card: outStack)
                _game.m_returnedCards.push_back(std::move(card));
        } else if (directionChoice == 'd') {
            auto outStack = std::move(row[row.size() - 1]);

            for (size_t i = row.size() - 1; i > 0; --i)
                row[i] = std::move(row[i - 1]);

            row[0].clear();

            for (auto &card: outStack)
                _game.m_returnedCards.push_back(std::move(card));
        }
    } else if (typeChoice == 'c') {
        if (directionChoice == 'w') {
            auto outStack = std::move(board.m_board[0][index]);

            for (size_t i = 0; i < board.m_board.size() - 1; ++i)
                board.m_board[i][index] = std::move(board.m_board[i + 1][index]);

            board.m_board[board.m_board.size() - 1][index].clear();

            for (auto &card: outStack)
                _game.m_returnedCards.push_back(std::move(card));
        } else if (directionChoice == 's') {
            auto outStack = std::move(board.m_board[board.m_board.size() - 1][index]);

            for (size_t i = board.m_board.size() - 1; i > 0; --i)
                board.m_board[i][index] = std::move(board.m_board[i - 1][index]);

            board.m_board[0][index].clear();

            for (auto &card: outStack)
                _game.m_returnedCards.push_back(std::move(card));
        }
    }

    return true;
}

bool Power::PowerAction::gust(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    std::cout <<
            "Gust: Move horizontally or vertically any visible card on the board to a position adjacent to a card with a lower number.\n";
    std::cout << "Enter the coordinates for the card that you want to move: ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y) || board.m_board[x][y].empty()) return false;

    std::map<char, std::pair<int, int> > directionMap = {
        {'w', {-1, 0}},
        {'s', {1, 0}},
        {'a', {0, -1}},
        {'d', {0, 1}}
    };

    std::cout << "Choose a direction (w = up, a = left, s = down, d = right): ";
    char direction;
    std::cin >> direction;
    direction = tolower(direction);

    if (directionMap.find(direction) == directionMap.end()) return false;

    auto offset = directionMap[direction];
    size_t newX = x + offset.first, newY = y + offset.second;

    if (!board.checkIndexes(newX, newY)) return false;

    if (board.m_board[newX][newY].empty()) return false;

    auto &currentCard = board.m_board[x][y].back();
    auto &targetCard = board.m_board[newX][newY].back();

    if (currentCard.getValue() <= targetCard.getValue()) return false;

    board.m_board[newX][newY].push_back(std::move(currentCard));
    board.m_board[x][y].pop_back();

    if (!board.checkBoardIntegrity()) {
        board.m_board[x][y].push_back(std::move(currentCard));
        board.m_board[newX][newY].pop_back();
    }

    return true;
}

bool Power::PowerAction::mirage(Player &_player, Game &_game, const bool _check) {
    size_t x = -1, y = -1;
    Board &board = _game.m_board;

    if (!_game.m_illusionsAllowed) return false;
    if (!_player.wasIllusionPlayed()) return false;

    for (size_t i = 0; i < board.m_board.size(); i++) {
        for (size_t j = 0; j < board.m_board[i].size(); j++) {
            if (board.m_board[i][j].empty()) continue;

            if (auto &illusion = board.m_board[i][j].back();
                illusion.isIllusion() && illusion.getColor() == _player.getColor()) {
                x = i, y = j;
                break;
            }
        }
    }

    if (!board.checkIndexes(x, y)) return false;

    std::cout << "Mirage: Replace your own placed illusion with another illusion.\n";

    size_t value;
    std::cout << "Please select new card value: ";
    std::cin >> value;

    if (value > static_cast<size_t>(Card::Value::Four)) return false;

    auto playedCard = _player.useCard(static_cast<Card::Value>(value));

    if (!playedCard) return false;

    auto &topCard = board.m_board[x][y].back();
    _player.returnCard(std::move(topCard));
    topCard.resetJustReturned();
    board.m_board[x][y].pop_back();

    playedCard->setIllusion();
    board.m_board[x][y].push_back(std::move(*playedCard));
    _player.setLastPlacedCard(_game.getBoard().getBoard()[x][y].back());

    return true;
}

bool Power::PowerAction::storm(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    std::cout << "Storm: Remove any pile of cards containing 2 or more cards.\n";
    std::cout << "Enter coordinates for pile: ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y))
        return false;

    if (!board.isAPile(x, y))
        return false;

    auto stack = std::move(board.m_board[x][y]);
    board.m_board[x][y].clear();

    if (!board.checkBoardIntegrity()) {
        board.m_board[x][y] = std::move(stack);
        return false;
    }

    while (!stack.empty()) {
        _game.m_eliminatedCards.push_back(std::move(stack.back()));
        stack.pop_back();
    }

    return true;
}

bool Power::PowerAction::tide(Player &_player, Game &_game, const bool _check) {
    size_t x, y, v, w;
    Board &board = _game.m_board;

    std::cout << "Tide: Switch 2 piles of cards.\n";
    std::cout << "Enter coordinates for the first pile (0-indexed): ";
    std::cin >> x >> y;
    std::cout << "Enter coordinates for the second pile (0-indexed): ";
    std::cin >> v >> w;

    if (!board.checkIndexes(x, y) || !board.checkIndexes(v, w)) return false;
    if (!board.isAPile(x, y) || !board.isAPile(v, w)) return false;

    std::swap(board.m_board[x][y], board.m_board[v][w]);

    return true;
}

bool Power::PowerAction::mist(Player &_player, Game &_game, const bool _check) {
    size_t x, y, value;
    Board &board = _game.m_board;

    for (auto &row: board.m_board)
        for (auto &col: row)
            if (!col.empty() && col.back().isIllusion() && col.back().getColor() == _player.getColor()) return false;

    std::cout << "Mist: Play an extra illusion.\n";
    std::cout << "Enter the coordinates and value for the new illusion\n";
    std::cin >> x >> y >> value;

    if (!board.checkIndexes(x, y)) return false;
    if (value > static_cast<size_t>(Card::Value::Four)) return false;

    auto playedCard = _player.useCard(static_cast<Card::Value>(value));

    if (!playedCard) return false;

    playedCard->setIllusion();
    board.m_board[x][y].push_back(std::move(*playedCard));
    _player.setLastPlacedCard(_game.getBoard().getBoard()[x][y].back());

    return true;
}

bool Power::PowerAction::wave(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    std::cout << "Wave: Move a pile to an adjacent empty position. Play a card on the new empty position.\n";
    std::cout << "Enter the coordinates for pile (0-indexed): ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y)) return false;
    if (!board.isAPile(x, y)) return false;

    std::map<char, std::pair<int, int> > directionMap = {
        {'w', {-1, 0}},
        {'s', {1, 0}},
        {'a', {0, -1}},
        {'d', {0, 1}}
    };

    char direction;
    bool validDirection = false;

    while (!validDirection) {
        std::cout << "Choose a direction (w = up, a = left, s = down, d = right): ";
        std::cin >> direction;
        direction = tolower(direction);

        if (directionMap.find(direction) != directionMap.end()) {
            validDirection = true;
        } else {
            std::cout << "Invalid direction! Please try again.\n";
        }
    }

    auto offset = directionMap[direction];
    int newX = static_cast<int>(x) + offset.first;
    int newY = static_cast<int>(y) + offset.second;

    if (!board.checkIndexes(newX, newY)) return false;
    if (!board.m_board[newX][newY].empty()) return false;

    size_t int_value;
    std::cout << "Enter value for the new card: ";
    std::cin >> int_value;

    auto playedCard = _player.playCardCheck(_game, x, y, int_value);

    if (!playedCard) return false;

    board.m_board[newX][newY] = std::move(board.m_board[x][y]);
    board.m_board[x][y].clear();

    _game.getBoard().placeCard(x, y, std::move(*playedCard));
    _player.setLastPlacedCard(_game.getBoard().getBoard()[x][y].back());

    return true;
}

bool Power::PowerAction::whirlpool(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    std::cout <<
            "Whirlpool: Move 2 cards from the same row or column, but separated by an empty space, onto that empty space. The card with the higher number is placed on top, and in case of a tie, the player chooses.\n";

    char choice;
    std::cout << "Choose row ('r') or column ('c'): ";
    std::cin >> choice;
    choice = tolower(choice);

    if (choice != 'r' && choice != 'c') return false;

    std::cout << "Enter the index for the empty spot on the row/column (0-indexed): ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y) || !board.m_board[x][y].empty()) return false;

    size_t firstIndex = (choice == 'c' ? x : y) - 1;
    size_t secondIndex = (choice == 'c' ? x : y) + 1;

    if (firstIndex > static_cast<size_t>(Card::Value::Four) ||
        secondIndex > static_cast<size_t>(Card::Value::Four))
        return false;

    if (choice == 'r') {
        if (board.m_board[x][firstIndex].empty() || board.m_board[x][secondIndex].empty()) return false;
    } else {
        if (board.m_board[firstIndex][x].empty() || board.m_board[secondIndex][x].empty()) return false;
    }

    std::unique_ptr<Card> firstCard = std::make_unique<Card>(
        choice == 'r' ? board.m_board[x][firstIndex].back() : board.m_board[firstIndex][y].back());
    std::unique_ptr<Card> secondCard = std::make_unique<Card>(
        choice == 'r' ? board.m_board[x][secondIndex].back() : board.m_board[secondIndex][y].back());

    const auto firstCardValue = firstCard->getValue();
    const auto secondCardValue = secondCard->getValue();

    bool switchCards;

    if (firstCardValue < secondCardValue) std::swap(firstCard, secondCard);

    if (firstCardValue == secondCardValue) {
        std::cout << "Cards have the same value. Choose which card goes on top:\n";
        std::cout << "0. " << *firstCard << "\n1. " << *secondCard << "\n";
        std::cin >> switchCards;

        if (!switchCards) std::swap(firstCard, secondCard);
    }

    board.m_board[x][y].push_back(std::move(*firstCard));
    board.m_board[x][y].push_back(std::move(*secondCard));

    board.m_board[choice == 'r' ? x : firstIndex][choice == 'r' ? firstIndex : y].pop_back();
    board.m_board[choice == 'r' ? x : secondIndex][choice == 'r' ? secondIndex : y].pop_back();

    if (!board.checkBoardIntegrity()) {
        board.m_board[choice == 'r' ? x : secondIndex][choice == 'r' ? secondIndex : y].push_back(
            std::move(board.m_board[x][y].back())
        );
        board.m_board[x][y].pop_back();

        board.m_board[choice == 'r' ? x : firstIndex][choice == 'r' ? firstIndex : y].push_back(
            std::move(board.m_board[x][y].back()
            ));
        board.m_board[x][y].pop_back();

        if (!switchCards)
            std::swap(
                board.m_board[choice == 'r' ? x : secondIndex][choice == 'r' ? secondIndex : y].back(),
                board.m_board[choice == 'r' ? x : firstIndex][choice == 'r' ? firstIndex : y].back()
            );

        return false;
    }


    return true;
}

bool Power::PowerAction::tsunami(Player &_player, Game &_game, const bool _check) {
    char line;
    Board &board = _game.m_board;
    std::cout << "Tsunami: Choose a row ('r') ora column ('c') to restrict: ";
    std::cin >> line;
    line = tolower(line);

    if (line != 'r' && line != 'c') return false;

    size_t index;
    std::cout << "Enter the index of the row/column to restrict: ";
    std::cin >> index;

    if (index >= board.getSize()) return false;

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

    if (!hasFreeSpace) return false;

    Power &power = getInstance();

    if (line == 'r') power.setRestrictedRow(index);
    else power.setRestrictedCol(index);

    power.setJustBlocked(true);
    return true;
}

bool Power::PowerAction::waterfall(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;
    char choice;
    size_t index;
    char direction;

    std::cout << "Waterfall: Choose a row ('r') ora column ('c'): ";
    std::cin >> choice;
    choice = tolower(choice);

    if (choice != 'r' && choice != 'c') return false;

    std::cout << "Enter the index of the row/column with at least 3 occupied positions: ";
    std::cin >> index;

    if ((choice == 'r' && index >= board.getSize()) || (choice == 'c' && index >= board.getSize())) return false;

    int occupiedPositions = 0;
    if (choice == 'r') {
        for (size_t col = 0; col < board.getSize(); ++col)
            if (!board.m_board[index][col].empty())
                ++occupiedPositions;
    } else {
        for (size_t row = 0; row < board.getSize(); ++row)
            if (!board.m_board[row][index].empty())
                ++occupiedPositions;
    }

    if (occupiedPositions < 3) return false;

    if (choice == 'r') std::cout << "Choose the direction of the cascade (a for left, d for right): ";
    else std::cout << "Choose the direction of the cascade (w for up, s for down): ";
    std::cin >> direction;
    direction = tolower(direction);

    if ((choice == 'r' && direction != 'a' && direction != 'd') ||
        (choice == 'c' && direction != 'w' && direction != 's'))
        return false;

    std::deque<Card> mergedStack;

    if (choice == 'r') {
        if (direction == 'a') {
            for (size_t col = 0; col < board.getSize(); ++col)
                while (!board.m_board[index][col].empty()) {
                    mergedStack.push_back(std::move(board.m_board[index][col].back()));
                    board.m_board[index][col].pop_back();
                }

            board.m_board[index][0] = std::move(mergedStack);

            for (size_t col = 1; col < board.getSize(); ++col)
                board.m_board[index][col].clear();
        } else {
            for (size_t col = board.getSize() - 1; col < board.getSize(); --col)
                while (!board.m_board[index][col].empty()) {
                    mergedStack.push_back(std::move(board.m_board[index][col].back()));
                    board.m_board[index][col].pop_back();
                }

            board.m_board[index][board.getSize() - 1] = std::move(mergedStack);

            for (size_t col = 0; col < board.getSize() - 1; ++col)
                board.m_board[index][col].clear();
        }
    } else {
        if (direction == 'w') {
            for (size_t row = 0; row < board.getSize(); ++row)
                while (!board.m_board[row][index].empty()) {
                    mergedStack.push_back(std::move(board.m_board[row][index].back()));
                    board.m_board[row][index].pop_back();
                }
            board.m_board[0][index] = std::move(mergedStack);

            for (size_t row = 1; row < board.getSize(); ++row)
                board.m_board[row][index].clear();
        } else {
            for (size_t row = board.getSize() - 1; row < board.getSize(); --row)
                while (!board.m_board[row][index].empty()) {
                    mergedStack.push_back(std::move(board.m_board[row][index].back()));
                    board.m_board[row][index].pop_back();
                }

            board.m_board[board.getSize() - 1][index] = std::move(mergedStack);

            for (size_t row = 0; row < board.getSize() - 1; ++row)
                board.m_board[row][index].clear();
        }
    }

    return true;
}

bool Power::PowerAction::support(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    std::cout << "Support: Choose a card of value 1, 2 or 3 to increase its value by 1.\n";
    std::cout << "Enter the coordinates of the card: ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y)) return false;
    if (board.m_board[x][y].empty()) return false;

    if (_game.m_illusionsAllowed && board.m_board[x][y].back().isIllusion()) return false;

    Card &selectedCard = board.m_board[x][y].back();

    if (selectedCard.getColor() != _player.getColor()) return false;
    if (selectedCard.getValue() == Card::Value::Four || selectedCard.getValue() == Card::Value::Eter) return false;

    Power &power = Power::getInstance();
    power.setPlus({
                      static_cast<short>(x), static_cast<short>(y),
                      static_cast<short>(_game.getBoard().getBoard()[x][y].size() - 1)
                  }, _game);

    return true;
}

bool Power::PowerAction::earthquake(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;
    bool anyRemoved = false;

    for (size_t row = 0; row < board.m_board.size(); ++row)
        for (size_t col = 0; col < board.m_board.size(); ++col) {
            if (board.m_board[row][col].empty()) continue;

            if (board.m_board[row][col].back().getValue() == Card::Value::One && !board.m_board[row][col].back().
                isIllusion()) {
                auto card = std::move(board.m_board[row][col].back());
                board.m_board[row][col].pop_back();

                if (!board.checkBoardIntegrity()) {
                    board.m_board[row][col].push_back(std::move(card));
                    continue;
                }

                _game.m_eliminatedCards.push_back(std::move(card));
                anyRemoved = true;
            }
        }

    return anyRemoved;
}

bool Power::PowerAction::crumble(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    std::cout << "Crumble: Choose an opponent's card of value 2, 3, or 4 to decrease its value by 1.\n";
    std::cout << "Enter the coordinates of the card: ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y)) return false;
    if (board.m_board[x][y].empty()) return false;

    if (_game.m_illusionsAllowed && board.m_board[x][y].back().isIllusion()) return false;

    Card &selectedCard = board.m_board[x][y].back();

    if (selectedCard.getColor() == _player.getColor()) return false;
    if (selectedCard.getValue() == Card::Value::One || selectedCard.getValue() == Card::Value::Eter) return false;

    Power &power = Power::getInstance();
    power.setMinus({
                       static_cast<short>(x), static_cast<short>(y),
                       static_cast<short>(_game.getBoard().getBoard()[x][y].size() - 1)
                   }, _game);

    return true;
}

bool Power::PowerAction::border(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    std::cout << "Border: Place a neutral card on the board to define at least one boundary.\n";
    std::cout << "Enter the coordinates for placing the neutral card: ";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y)) return false;
    if (!board.m_board[x][y].empty()) return false;

    if (x != 0 && x != board.m_board.size() - 1 && y != 0 && y != board.m_board.size() - 1) return false;

    short minRow = board.getSize();
    short minCol = board.getSize();
    short maxRow = -1;
    short maxCol = -1;

    for (short i = 0; i < board.getSize(); ++i) {
        for (short j = 0; j < board.getSize(); ++j) {
            if (!board.m_board[i][j].empty()) {
                minRow = std::min(minRow, i);
                maxRow = std::max(maxRow, i);
                minCol = std::min(minCol, j);
                maxCol = std::max(maxCol, j);
            }
        }
    }

    const short height = maxRow - minRow + 1;
    const short width = maxCol - minCol + 1;

    if (height != board.getSize() - 1 && width != board.getSize() - 1) return false;

    if (height == board.getSize() - 1)
        if (minRow == x || maxRow == x)
            return false;

    if (width == board.getSize() - 1)
        if (minCol == y || maxCol == y)
            return false;

    board.m_board[x][y].emplace_back(Card::Value::Border, Card::Color::Undefined);

    std::cout << "Now play a card!\n";
    if (!_player.playCard(_game)) {
        board.m_board[x][y].pop_back();
        return false;
    }

    return true;
}

bool Power::PowerAction::avalanche(Player &_player, Game &_game, const bool _check) {
    size_t x1, y1, x2, y2;
    Board &board = _game.m_board;

    std::cout << "Avalanche: Choose two adjacent stacks on the board.\n";
    std::cout << "Enter the coordinates for the first stack (the 2 stacks will move in the direction of this stack): ";
    std::cin >> x1 >> y1;
    std::cout << "Enter the coordinates for the second stack: ";
    std::cin >> x2 >> y2;

    if (!board.isAPile(x1, y1) || !board.isAPile(x2, y2)) return false;
    if (!board.checkIndexes(x1, y1) && !board.checkIndexes(x2, y2)) return false;

    if (!(x1 == x2 && abs(static_cast<int>(y1) - static_cast<int>(y2)) == 1) && !(
            y1 == y2 && abs(static_cast<int>(x1) - static_cast<int>(x2)) == 1)) return false;

    if (x1 == x2) {
        if (y1 < y2 && y1 > 0 && board.m_board[x1][y1 - 1].empty()) {
            board.m_board[x1][y1 - 1] = std::move(board.m_board[x1][y1]);
            board.m_board[x1][y1] = std::move(board.m_board[x1][y2]);
            board.m_board[x1][y2].clear();

            if (!board.checkBoardIntegrity()) {
                board.m_board[x1][y2] = std::move(board.m_board[x1][y1]);
                board.m_board[x1][y1] = std::move(board.m_board[x1][y1 - 1]);
                board.m_board[x1][y1 - 1].clear();

                return false;
            }

        } else if (y1 > y2 && y1 < board.getSize() - 1 && board.m_board[x1][y1 + 1].empty()) {
            board.m_board[x1][y1 + 1] = std::move(board.m_board[x1][y1]);
            board.m_board[x1][y1] = std::move(board.m_board[x1][y2]);
            board.m_board[x1][y2].clear();

            if (!board.checkBoardIntegrity()) {
                board.m_board[x1][y2] = std::move(board.m_board[x1][y1]);
                board.m_board[x1][y1] = std::move(board.m_board[x1][y1 + 1]);
                board.m_board[x1][y1 + 1].clear();

                return false;
            }
        }

        else return false;

    } else if (y1 == y2) {
        if (x1 < x2 && x1 > 0 && board.m_board[x1 - 1][y1].empty()) {
            board.m_board[x1 - 1][y1] = std::move(board.m_board[x1][y1]);
            board.m_board[x1][y1] = std::move(board.m_board[x2][y1]);
            board.m_board[x2][y1].clear();

            if (!board.checkBoardIntegrity()) {
                board.m_board[x2][y1] = std::move(board.m_board[x1][y1]);
                board.m_board[x1][y1] = std::move(board.m_board[x1 - 1][y1]);
                board.m_board[x1 - 1][y1].clear();

                return false;
            }

        } else if (x1 > x2 && x1 < board.getSize() - 1 && board.m_board[x1 + 1][y1].empty()) {
            board.m_board[x1 + 1][y1] = std::move(board.m_board[x1][y1]);
            board.m_board[x1][y1] = std::move(board.m_board[x2][y1]);
            board.m_board[x2][y1].clear();

            if (!board.checkBoardIntegrity()) {
                board.m_board[x2][y1] = std::move(board.m_board[x1][y1]);
                board.m_board[x1][y1] = std::move(board.m_board[x1 + 1][y1]);
                board.m_board[x1 + 1][y1].clear();

                return false;
            }

        }

        else return false;
    }

    else return false;
    return true;
}

bool Power::PowerAction::rock(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    if (!_game.m_illusionsAllowed) return false;

    std::cout << "Rock: Cover any illusion with a card (from your hand) without flipping the illusion face up.\n";
    std::cout << "Enter the coordinates for the illusion to cover:\n";
    std::cin >> x >> y;

    if (!board.checkIndexes(x, y)) return false;
    if (board.m_board[x][y].empty()) return false;
    if (!board.m_board[x][y].back().isIllusion()) return false;

    std::cout << "Choose a card value to cover the illusion: ";
    int cardValue;
    std::cin >> cardValue;

    auto selectedCard = _player.useCard(static_cast<Card::Value>(cardValue));

    if (!selectedCard) return false;

    board.placeCard(x, y, std::move(*selectedCard));

    return true;
}
