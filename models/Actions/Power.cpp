﻿#include "../Actions/Power.h"
#include "../GameMechanics/Board.h"
#include "../GameMechanics/Game.h"

CardPosition Power::getMinus(const Board &_board) const {
    return _board.findCardIndexes(m_minus);
}

void Power::setMinus(const CardPosition _position, Game &_game) {
    m_minus = &_game.getBoard().getBoard()[_position.x][_position.y][_position.z];
}

CardPosition Power::getPlus(const Board &_board) const {
    return _board.findCardIndexes(m_plus);
}

void Power::setPlus(const CardPosition _position, Game &_game) {
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

//neverificata
bool Power::PowerAction::controlledExplosion(Player &_player, Game &_game, const bool _check) {
    if (!_game.m_explosionAllowed) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Explosions are not allowed in this game mode!");
        return false;
    };

    Board &board = _game.m_board;

    if (_game.m_playedExplosion) Explosion::getInstance().generateExplosion(board.m_board.size());
    
    size_t returnedCount = 0;
    size_t eliminatedCount = 0;
    
    board.useExplosion(_game.m_returnedCards, _game.m_eliminatedCards);
    _game.m_playedExplosion = true;

    returnedCount = _game.m_returnedCards.size();
    eliminatedCount = _game.m_eliminatedCards.size();

    QMessageBox::information(nullptr, "Explosion Results",
        QString("Controlled explosion executed successfully!\n"
            "Cards returned to hands: %1\n"
            "Cards eliminated: %2")
        .arg(returnedCount)
        .arg(eliminatedCount));

    return true;
}

bool Power::PowerAction::destruction(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;

    auto &opponent = _player.getColor() == Card::Color::Red ? _game.m_player2 : _game.m_player1;
    
    if (!opponent.getLastPlacedCard() ) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Opponent hasn't placed any card yet!");
        return false;
    }

    auto [lastRow, lastCol, cardHeight] = board.findCardIndexes(opponent.getLastPlacedCard());

    if (lastRow >= board.getSize() || lastCol >= board.getSize() ||
        board.m_board[lastRow][lastCol].empty() ||
        cardHeight >= board.m_board[lastRow][lastCol].size()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Cannot find opponent's last placed card!");
        return false;
    }


    auto &stack = board.m_board[lastRow][lastCol];
    Card affectedCard = std::move(stack[cardHeight]);
    stack.erase(stack.begin() + cardHeight);

    bool boardEmpty = true;
    for (size_t i = 0; i < board.getSize(); ++i) {
        for (size_t j = 0; j < board.getSize(); ++j) {
            if (!board.m_board[i][j].empty()) {
                boardEmpty = false;
                break;
            }
        }
        if (!boardEmpty) break;
    }

    if (!board.checkBoardIntegrity()) {
        stack.insert(stack.begin() + cardHeight, std::move(affectedCard));
        QMessageBox::warning(nullptr, "Invalid Move",
            "This move would break board integrity!");
        return false;
    }

    _game.m_eliminatedCards.push_back(std::move(affectedCard));

    if (boardEmpty) {
        board.m_firstCardPlayed = false;
    }

    QMessageBox::information(nullptr, "Success",
        "Card successfully destroyed!");
    return true;
}

bool Power::PowerAction::flame(Player &_player, Game &_game, const bool _check) {
    if (!_game.m_illusionsAllowed) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Illusions are not allowed in this game mode!");
        return false;
    }

    Board &board = _game.m_board;

    bool illusionFound = false;
    size_t foundRow = 0, foundCol = 0;

    Card::Color opponentColor = _player.getColor() == Card::Color::Red ?
        Card::Color::Blue : Card::Color::Red;

    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            if (board.checkIllusion(row, col, _player.getColor() == Card::Color::Red
                                                  ? Card::Color::Blue
                                                  : Card::Color::Red)) {
                board.m_board[row][col].back().resetIllusion();
                Card& illusionCard = board.m_board[row][col].back();
                QString colorPrefix = opponentColor == Card::Color::Red ? "R" : "B";
                QString valSuffix = QString::number(static_cast<int>(illusionCard.getValue()));
                QString newImagePath = QCoreApplication::applicationDirPath() +
                    "/cards/" + colorPrefix + valSuffix + ".png";

                foundRow = row;
                foundCol = col;
                illusionFound = true;
                break;
            }
        }

        if (illusionFound) break;
    }

    if (!illusionFound){
        QMessageBox::warning(nullptr, "Invalid Move",
            "No opponent illusions found on the board!");
        return false;
    }
    //board.printBoard();

   /* std::cout << "Now place a card on any position on the board.\n";*/

    QMessageBox::information(nullptr, "Success",
        QString("Opponent's illusion at position (%1, %2) has been revealed!\n"
            "Now you can place a card anywhere on the board.")
        .arg(foundRow).arg(foundCol));

    return true;
   // return _player.playCard(_game); // TODO: no checks maybe? check power desc
}

bool Power::PowerAction::lava(Player &_player, Game &_game, const bool _check) {
    //size_t chosenValue;

    /*std::cout << "Lava: Select a value with the condition that at least 2 cards of that value are visible.\n";
    std::cin >> chosenValue;*/

    bool ok;
    QString selectedValue = QInputDialog::getItem(nullptr,
        "Lava Power",
        "Select a card value (must have at least 2 visible cards of this value):",
        QStringList() << "1" << "2" << "3" << "4",
        0, false, &ok);

    if (!ok) return false;
    size_t chosenValue = static_cast<size_t>(selectedValue.toInt());
    auto targetValue = static_cast<Card::Value>(chosenValue);

    //if (chosenValue > 4) return false;

    //auto targetValue = static_cast<Card::Value>(chosenValue);

    size_t count = 0;
    Board &board = _game.m_board;
    for (size_t row = 0; row < board.getSize(); ++row)
        for (size_t col = 0; col < board.getSize(); ++col)
            if (!board.m_board[row][col].empty() &&
                board.m_board[row][col].back().getValue() == targetValue &&
                !board.m_board[row][col].back().isIllusion()) { ++count; }

    if (count < 2) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "There must be at least 2 visible cards with the selected value!");
        return false;
    }

    for (size_t row = 0; row < board.getSize(); ++row)
        for (size_t col = 0; col < board.getSize(); ++col)
            if (!board.m_board[row][col].empty()) {
                Card &topCard = board.m_board[row][col].back();

                if (topCard.getValue() == targetValue && !topCard.isIllusion()) {
                    _game.m_returnedCards.push_back(std::move(topCard));
                    board.m_board[row][col].pop_back();
                }
            }

    QMessageBox::information(nullptr, "Success",
        QString("Successfully removed all visible cards with value %1").arg(chosenValue));
    return true;
}

bool Power::PowerAction::ash(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;

    auto &eliminatedCards = _game.m_eliminatedCards;
    if (eliminatedCards.empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "There are no eliminated cards to bring back!");
        return false;
    }

    QStringList cardOptions;
    std::vector<size_t> validIndices;

    for (size_t i = 0; i < eliminatedCards.size(); ++i) {
        if (eliminatedCards[i].getColor() == _player.getColor()) {
            QString cardDesc = QString("Card %1 (Value: %2)").arg(i + 1)
                .arg(static_cast<int>(eliminatedCards[i].getValue()));
            cardOptions.append(cardDesc);
            validIndices.push_back(i);
        }
    }

    if (cardOptions.isEmpty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "You have no eliminated cards to bring back!");
        return false;
    }


    bool ok;
    QString selectedCard = QInputDialog::getItem(nullptr,
        "Ash Power",
        "Select a card to bring back:",
        cardOptions, 0, false, &ok);

    if (!ok) return false;

    int selectedIndex = cardOptions.indexOf(selectedCard);
    if (selectedIndex == -1) return false;

    Card& chosenCard = eliminatedCards[validIndices[selectedIndex]];

    /*size_t cardIndex;
    std::cout << "Select the card index: ";
    std::cin >> cardIndex;

    if (cardIndex < 1 || cardIndex > eliminatedCards.size()) return false;

    Card &chosenCard = eliminatedCards[cardIndex - 1];

    if (chosenCard.getColor() != _player.getColor()) return false;

    size_t x, y;
    std::cout << "Enter (x, y) coordinates to place the card (0-indexed): ";
    std::cin >> x >> y;*/

    int x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Ash Power",
        "Enter X coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    int y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Ash Power",
        "Enter Y coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;


    if (!board.checkPartial(x, y, static_cast<size_t>(chosenCard.getValue()))) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Cannot place the card at this position!");
        return false;
    }

    _game.m_eliminatedCards.erase(_game.m_eliminatedCards.begin() + validIndices[selectedIndex]);

    board.m_board[y][x].push_back(std::move(chosenCard));

    if (!board.checkBoardIntegrity()) {
        chosenCard = std::move(board.m_board[x][y].back());
        board.m_board[x][y].pop_back();
        QMessageBox::warning(nullptr, "Invalid Move",
            "This move would break board integrity!");
        return false;
    }

    _player.setLastPlacedCard(_game.getBoard().getBoard()[x][y].back());
    QMessageBox::information(nullptr, "Success",
        "Card successfully brought back to the board!");
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

    if (coveredCards.empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "No covered cards found that can be moved!");
        return false;
    }

    /*for (size_t i = 0; i < coveredCards.size(); ++i) {
        auto [row, col, card] = coveredCards[i];
        std::cout << i + 1 << ": " << card << "at position (" << row << ", " << col << ")\n";
    }*/

    QStringList cardOptions;
    for (const auto& [row, col, card] : coveredCards) {
        cardOptions << QString("Card value %1 at position (%2, %3)")
            .arg(static_cast<int>(card.getValue()))
            .arg(row)
            .arg(col);
    }


    /*size_t choice;
    std::cout << "Select the card index: ";
    std::cin >> choice;

    if (choice < 1 || choice > coveredCards.size()) return false;

    auto [origRow, origCol, chosenCard] = coveredCards[choice - 1];*/


    bool ok;
    QString selectedCard = QInputDialog::getItem(nullptr,
        "Spark Power",
        "Select a covered card to move:",
        cardOptions, 0, false, &ok);

    if (!ok) return false;

    int choice = cardOptions.indexOf(selectedCard);
    if (choice == -1) return false;

    auto [origRow, origCol, chosenCard] = coveredCards[choice];

    /*size_t newRow, newCol;
    std::cout << "Enter the new position (x, y) to place the card (0-indexed): ";
    std::cin >> newRow >> newCol;*/

    size_t newRow = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Spark Power",
        "Enter new row position (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    size_t newCol = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Spark Power",
        "Enter new column position (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));

    if (!ok) return false;

    if (!board.checkPartial(newRow, newCol, static_cast<size_t>(chosenCard.getValue()))) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Cannot place the card at this position!");
        return false;
    }

    auto &originalStack = board.m_board[origRow][origCol];
    originalStack.erase(originalStack.end() - 2);
    board.m_board[newRow][newCol].push_back(std::move(chosenCard));

    if (!board.checkBoardIntegrity()) {
        auto card = std::move(board.m_board[newRow][newCol].back());
        board.m_board[newRow][newCol].pop_back();

        board.m_board[origRow][origCol].push_back(std::move(card));

        QMessageBox::warning(nullptr, "Invalid Move",
            "This move would break board integrity!");
        return false;
    }

    _player.setLastPlacedCard(_game.getBoard().getBoard()[newRow][newCol].back());
    QMessageBox::information(nullptr, "Success",
        QString("Card successfully moved from (%1, %2) to (%3, %4)!")
        .arg(origRow).arg(origCol)
        .arg(newRow).arg(newCol));
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

    if (visibleOpponentCards.empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "No opponent cards available to return!");
        return false;
    }

    /*std::cout << "Squall: Choose a card to return to opponent's hand:\n";
    for (size_t i = 0; i < visibleOpponentCards.size(); ++i) {
        auto [row, col, card] = visibleOpponentCards[i];
        std::cout << i + 1 << ": " << card << " at position (" << row << ", " << col << ")\n";
    }*/

    QStringList cardOptions;
    for (const auto& [row, col, card] : visibleOpponentCards) {
        cardOptions << QString("Card value %1 at position (%2, %3)")
            .arg(static_cast<int>(card.getValue()))
            .arg(row)
            .arg(col);
    }

    /*size_t choice;
    std::cout << "Select the card index: ";
    std::cin >> choice;

    if (choice < 1 || choice > visibleOpponentCards.size()) return false;*/

    bool ok;
    QString selectedOption = QInputDialog::getItem(nullptr,
        "Squall Power",
        "Choose a card to return to opponent's hand:",
        cardOptions, 0, false, &ok);

    if (!ok) return false;

    size_t choice = static_cast<size_t>(cardOptions.indexOf(selectedOption));
    if (choice == -1) return false;

    auto [selectedRow, selectedCol, selectedCard] = visibleOpponentCards[choice];

    auto card = std::move(board.m_board[selectedRow][selectedCol].back());
    board.m_board[selectedRow][selectedCol].pop_back();

    if (!board.checkBoardIntegrity()) {
        board.m_board[selectedRow][selectedCol].push_back(std::move(card));
        QMessageBox::warning(nullptr, "Invalid Move",
            "Removing this card would break board integrity!");
        return false;
    }

    _game.m_returnedCards.push_back(std::move(card));
    QMessageBox::information(nullptr, "Success",
        QString("Card at position (%1, %2) has been returned to opponent's hand!")
        .arg(selectedRow)
        .arg(selectedCol));
    return true;
}

bool Power::PowerAction::gale(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;

    bool hasCoveredCards = false;
    for (size_t row = 0; row < board.getSize(); ++row) {
        for (size_t col = 0; col < board.getSize(); ++col) {
            if (board.m_board[row][col].size() > 1) {
                hasCoveredCards = true;
                break;
            }
        }
        if (hasCoveredCards) break;
    }

    if (!hasCoveredCards) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "There are no covered cards on the board!");
        return false;
    }


    QMessageBox::StandardButton reply = QMessageBox::question(nullptr,
        "Gale Power",
        "This will return all covered cards to their owners' hands.\nContinue?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return false;
    }

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

    QMessageBox::information(nullptr, "Success",
        QString("Gale power executed successfully!\n"
            "Covered cards have been returned to their owners."));

    return true;
}

bool Power::PowerAction::hurricane(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;
    //std::cout << "Hurricane: Shift a full row or column in the desired direction.\n";

    /*char typeChoice, directionChoice;
    size_t index;*/

    /*std::cout << "Choose type to shift (r: row, c: column): ";
    while (std::cin >> typeChoice && tolower(typeChoice) != 'r' && tolower(typeChoice != 'c')) {
        std::cout << "Invalid choice! Try again: ";
    }*/


    QStringList typeOptions = { "Row", "Column" };
    bool ok;
    QString typeChoice = QInputDialog::getItem(nullptr,
        "Hurricane Power",
        "Choose what to shift:",
        typeOptions, 0, false, &ok);

    if (!ok) return false;
    bool isRow = (typeChoice == "Row");

    size_t maxIndex = board.m_board.size() - 1;
    size_t index = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Hurricane Power",
        QString("Choose %1 index (0-%2):").arg(isRow ? "row" : "column").arg(maxIndex),
        0, 0, maxIndex, 1, &ok));

    if (!ok) return false;
    //typeChoice = tolower(typeChoice);

    bool hasEmpty = false;
    if (isRow) {
        for (const auto& stack : board.m_board[index])
            if (stack.empty()) hasEmpty = true;
    }
    else {
        for (auto& row : board.m_board)
            if (row[index].empty()) hasEmpty = true;
    }

    if (hasEmpty) {
        QMessageBox::warning(nullptr, "Invalid Move",
            QString("The selected %1 must be completely filled!")
            .arg(isRow ? "row" : "column"));
        return false;
    }

    QStringList directionOptions;
    if (isRow) {
        directionOptions = { "Left", "Right" };
    }
    else {
        directionOptions = { "Up", "Down" };
    }

    QString direction = QInputDialog::getItem(nullptr,
        "Hurricane Power",
        "Choose direction to shift:",
        directionOptions, 0, false, &ok);

    if (!ok) return false;

    /*if (typeChoice == 'r') {
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
    }*/

    /*do {
        std::cout << "Choose direction (w: up, a: left, s: down, d: right): ";
        std::cin >> directionChoice;
        directionChoice = tolower(directionChoice);

        if ((typeChoice == 'r' && (directionChoice != 'a' && directionChoice != 'd')) ||
            (typeChoice == 'c' && (directionChoice != 'w' && directionChoice != 's'))) {
            std::cout << "Invalid direction for the chosen type! Try again.\n";
        } else {
            break;
        }
    } while (true);*/

    if (isRow) {
        auto &row = board.m_board[index];

        if (direction == "Left") {
            auto outStack = std::move(row[0]);

            for (size_t i = 0; i < row.size() - 1; ++i)
                row[i] = std::move(row[i + 1]);

            row[row.size() - 1].clear();

            for (auto &card: outStack)
                _game.m_returnedCards.push_back(std::move(card));
        } else if (direction == "Right") {
            auto outStack = std::move(row[row.size() - 1]);

            for (size_t i = row.size() - 1; i > 0; --i)
                row[i] = std::move(row[i - 1]);

            row[0].clear();

            for (auto &card: outStack)
                _game.m_returnedCards.push_back(std::move(card));
        }
    } else {
        if (direction == "Up") {
            auto outStack = std::move(board.m_board[0][index]);

            for (size_t i = 0; i < board.m_board.size() - 1; ++i)
                board.m_board[i][index] = std::move(board.m_board[i + 1][index]);

            board.m_board[board.m_board.size() - 1][index].clear();

            for (auto &card: outStack)
                _game.m_returnedCards.push_back(std::move(card));
        } else if (direction == "Down") {
            auto outStack = std::move(board.m_board[board.m_board.size() - 1][index]);

            for (size_t i = board.m_board.size() - 1; i > 0; --i)
                board.m_board[i][index] = std::move(board.m_board[i - 1][index]);

            board.m_board[0][index].clear();

            for (auto &card: outStack)
                _game.m_returnedCards.push_back(std::move(card));
        }
    }

    QMessageBox::information(nullptr, "Success",
        QString("%1 %2 has been shifted %3!")
        .arg(isRow ? "Row" : "Column")
        .arg(index)
        .arg(direction.toLower()));

    return true;
}

bool Power::PowerAction::gust(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    /*std::cout <<
            "Gust: Move horizontally or vertically any visible card on the board to a position adjacent to a card with a lower number.\n";
    std::cout << "Enter the coordinates for the card that you want to move: ";
    std::cin >> x >> y;*/

    bool ok;
     x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Gust Power",
        "Enter row coordinate of the card to move (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

     y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Gust Power",
        "Enter column coordinate of the card to move (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;


    if (!board.checkIndexes(x, y) || board.m_board[x][y].empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "There is no card at the selected position!");
        return false;
    }


    QStringList directions = { "Up", "Down", "Left", "Right" };
    QString direction = QInputDialog::getItem(nullptr,
        "Gust Power",
        "Choose direction to move:",
        directions, 0, false, &ok);
    if (!ok) return false;

    std::map<QString, std::pair<int, int> > directionMap = {
        {"Up", {-1, 0}},
        {"Down", {1, 0}},
        {"Left", {0, -1}},
        {"Right", {0, 1}}
    };

    auto offset = directionMap[direction];
    int newX = x + offset.first;
    int newY = y + offset.second;

    // Verifică dacă noua poziție e validă
    if (!board.checkIndexes(newX, newY)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Cannot move card outside the board!");
        return false;
    }

   /* std::cout << "Choose a direction (w = up, a = left, s = down, d = right): ";
    char direction;
    std::cin >> direction;
    direction = tolower(direction);*/

    /*if (directionMap.find(direction) == directionMap.end()) return false;

    auto offset = directionMap[direction];
    size_t newX = x + offset.first, newY = y + offset.second;

    if (!board.checkIndexes(newX, newY)) return false;*/

    if (board.m_board[newX][newY].empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Must move card adjacent to another card!");
        return false;
    }

    auto &currentCard = board.m_board[x][y].back();
    auto &targetCard = board.m_board[newX][newY].back();

    if (currentCard.getValue() <= targetCard.getValue()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Card must be moved adjacent to a card with lower value!");
        return false;
    }

    board.m_board[newX][newY].push_back(std::move(currentCard));
    board.m_board[x][y].pop_back();

    if (!board.checkBoardIntegrity()) {
        board.m_board[x][y].push_back(std::move(currentCard));
        board.m_board[newX][newY].pop_back();
        QMessageBox::warning(nullptr, "Invalid Move",
            "This move would break board integrity!");
        return false;
    }

    QMessageBox::information(nullptr, "Success",
        QString("Card successfully moved from (%1, %2) to (%3, %4)!")
        .arg(x).arg(y).arg(newX).arg(newY));

    return true;
}

//nu merge
bool Power::PowerAction::mirage(Player &_player, Game &_game, const bool _check) {
    size_t x = -1, y = -1;
    Board &board = _game.m_board;

    if (!_game.m_illusionsAllowed) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Illusions are not allowed in this game mode!");
        return false;
    }

    if (!_player.wasIllusionPlayed()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "You haven't played any illusion yet!");
        return false;
    }

    for (size_t i = 0; i < board.m_board.size(); i++) {
        for (size_t j = 0; j < board.m_board[i].size(); j++) {
            if (board.m_board[i][j].empty()) continue;

            if (auto &illusion = board.m_board[i][j].back();
                illusion.isIllusion() && illusion.getColor() == _player.getColor()) {
                x = i, y = j;
                break;
            }
        }
        if (x != -1) break;
    }

    if (!board.checkIndexes(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "No illusion found to replace!");
        return false;
    }

    /*std::cout << "Mirage: Replace your own placed illusion with another illusion.\n";

    size_t value;
    std::cout << "Please select new card value: ";
    std::cin >> value;

    if (value > static_cast<size_t>(Card::Value::Four)) return false;*/

    bool ok;
    QString selectedValue = QInputDialog::getItem(nullptr,
        "Mirage Power",
        "Select new illusion value:",
        QStringList() << "1" << "2" << "3" << "4",
        0, false, &ok);

    if (!ok) return false;
    size_t value = static_cast<size_t>(selectedValue.toInt());

    auto playedCard = _player.useCard(static_cast<Card::Value>(value));

    if (!playedCard) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "You don't have a card with this value!");
        return false;
    }

    auto &topCard = board.m_board[x][y].back();
    _player.returnCard(std::move(topCard));
    topCard.resetJustReturned();
    board.m_board[x][y].pop_back();

    playedCard->setIllusion();
    board.m_board[x][y].push_back(std::move(*playedCard));
    _player.setLastPlacedCard(_game.getBoard().getBoard()[x][y].back());

    QMessageBox::information(nullptr, "Success",
        QString("Successfully replaced illusion at position (%1, %2)").arg(x).arg(y));

    return true;
}

bool Power::PowerAction::storm(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    /*std::cout << "Storm: Remove any pile of cards containing 2 or more cards.\n";
    std::cout << "Enter coordinates for pile: ";
    std::cin >> x >> y;*/

    bool ok;
    x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Storm Power",
        "Enter row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Storm Power",
        "Enter column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Invalid coordinates!");
        return false;
    }

    if (!board.isAPile(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Selected position must contain at least 2 cards!");
        return false;
    }

    auto stack = std::move(board.m_board[x][y]);
    board.m_board[x][y].clear();

    if (!board.checkBoardIntegrity()) {
        board.m_board[x][y] = std::move(stack);
        QMessageBox::warning(nullptr, "Invalid Move",
            "This move would break board integrity!");
        return false;
    }

    while (!stack.empty()) {
        _game.m_eliminatedCards.push_back(std::move(stack.back()));
        stack.pop_back();
    }

    bool boardEmpty = true;
    for (size_t i = 0; i < board.getSize(); ++i) {
        for (size_t j = 0; j < board.getSize(); ++j) {
            if (!board.m_board[i][j].empty()) {
                boardEmpty = false;
                break;
            }
        }
        if (!boardEmpty) break;
    }

    if (boardEmpty) {
        board.m_firstCardPlayed = false;
    }

    QMessageBox::information(nullptr, "Success",
        QString("Successfully eliminated pile at position (%1, %2)!")
        .arg(x).arg(y));

    return true;
}

bool Power::PowerAction::tide(Player &_player, Game &_game, const bool _check) {
    size_t x, y, v, w;
    Board &board = _game.m_board;

    /*std::cout << "Tide: Switch 2 piles of cards.\n";
    std::cout << "Enter coordinates for the first pile (0-indexed): ";
    std::cin >> x >> y;
    std::cout << "Enter coordinates for the second pile (0-indexed): ";
    std::cin >> v >> w;*/

    bool ok;
    x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Tide Power",
        "Enter row coordinate for first pile (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Tide Power",
        "Enter column coordinate for first pile (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    // Coordonatele pentru a doua grămadă
    v = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Tide Power",
        "Enter row coordinate for second pile (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    w = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Tide Power",
        "Enter column coordinate for second pile (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;


    if (!board.checkIndexes(x, y) || !board.checkIndexes(v, w)){
            QMessageBox::warning(nullptr, "Invalid Move",
                "Invalid coordinates!");
            return false;
    }

    if (!board.isAPile(x, y) || !board.isAPile(v, w)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Both positions must contain at least 2 cards!");
        return false;
    }

    std::swap(board.m_board[x][y], board.m_board[v][w]);

    QMessageBox::information(nullptr, "Success",
        QString("Successfully swapped piles at positions (%1, %2) and (%3, %4)!")
        .arg(x).arg(y).arg(v).arg(w));

    return true;
}

bool Power::PowerAction::mist(Player &_player, Game &_game, const bool _check) {
    size_t x, y, value;
    Board &board = _game.m_board;

    for (auto &row: board.m_board)
        for (auto &col: row)
            if (!col.empty() && col.back().isIllusion() && col.back().getColor() == _player.getColor()) {
                QMessageBox::warning(nullptr, "Invalid Move",
                    "You already have an illusion on the board!");
                return false;
            }

    /*std::cout << "Mist: Play an extra illusion.\n";
    std::cout << "Enter the coordinates and value for the new illusion\n";
    std::cin >> x >> y >> value;*/

    bool ok;
    QString selectedValue = QInputDialog::getItem(nullptr,
        "Mist Power",
        "Select value for the new illusion:",
        QStringList() << "1" << "2" << "3" << "4",
        0, false, &ok);

    if (!ok) return false;
   value = static_cast<size_t>(selectedValue.toInt());

    x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Mist Power",
        "Enter row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Mist Power",
        "Enter column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Invalid coordinates!");
        return false;
    }
    if (value > static_cast<size_t>(Card::Value::Four)) return false;

    auto playedCard = _player.useCard(static_cast<Card::Value>(value));

    if (!playedCard) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "You don't have a card with this value!");
        return false;
    }

    playedCard->setIllusion();
    board.m_board[x][y].push_back(std::move(*playedCard));
    _player.setLastPlacedCard(_game.getBoard().getBoard()[x][y].back());
    _player.setPlayedIllusion(true);

    QMessageBox::information(nullptr, "Success",
        QString("Successfully placed illusion at position (%1, %2)!")
        .arg(x).arg(y));
    return true;
}

bool Power::PowerAction::wave(Player &_player, Game &_game, const bool _check) {
    //size_t x, y;
    Board &board = _game.m_board;

    /*std::cout << "Wave: Move a pile to an adjacent empty position. Play a card on the new empty position.\n";
    std::cout << "Enter the coordinates for pile (0-indexed): ";
    std::cin >> x >> y;*/

    bool ok;
    size_t x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Wave Power",
        "Enter row coordinate for pile (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    size_t y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Wave Power",
        "Enter column coordinate for pile (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Invalid coordinates!");
        return false;
    }
    if (!board.isAPile(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Selected position must contain at least two cards!");
        return false;
    }


    QStringList directions = { "Up", "Down", "Left", "Right" };
    QString direction = QInputDialog::getItem(nullptr,
        "Wave Power",
        "Choose direction to move:",
        directions, 0, false, &ok);
    if (!ok) return false;

    std::map<QString, std::pair<int, int> > directionMap = {
        {"Up", {-1, 0}},
        {"Down", {1, 0}},
        {"Left", {0, -1}},
        {"Right", {0, 1}}
    };

    /*char direction;
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
    }*/

    auto offset = directionMap[direction];
    int newX = static_cast<int>(x) + offset.first;
    int newY = static_cast<int>(y) + offset.second;

    if (!board.checkIndexes(newX, newY)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Cannot move pile outside the board!");
        return false;
    }
    if (!board.m_board[newX][newY].empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Target position must be empty!");
        return false;
    }

    QString selectedValue = QInputDialog::getItem(nullptr,
        "Wave Power",
        "Select value for the new card:",
        QStringList() << "1" << "2" << "3" << "4",
        0, false, &ok);
    if (!ok) return false;
    size_t value = static_cast<size_t>(selectedValue.toInt());


    /*size_t int_value;
    std::cout << "Enter value for the new card: ";
    std::cin >> int_value;*/

   /* auto playedCard = _player.playCardCheck(_game, x, y, value);*/
    auto playedCard = _player.useCard(static_cast<Card::Value>(value));
    if (!playedCard) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "You don't have a card with this value!");
        return false;
    }


    board.m_board[newX][newY] = std::move(board.m_board[x][y]);
    board.m_board[x][y].clear();

    board.m_board[x][y].push_back(std::move(*playedCard));
    _player.setLastPlacedCard(_game.getBoard().getBoard()[x][y].back());

    QMessageBox::information(nullptr, "Success",
        QString("Successfully moved pile to (%1, %2) and placed new card at (%3, %4)!")
        .arg(newX).arg(newY).arg(x).arg(y));

    return true;
}

bool Power::PowerAction::whirlpool(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    /*std::cout <<
            "Whirlpool: Move 2 cards from the same row or column, but separated by an empty space, onto that empty space. The card with the higher number is placed on top, and in case of a tie, the player chooses.\n";

    char choice;
    std::cout << "Choose row ('r') or column ('c'): ";
    std::cin >> choice;
    choice = tolower(choice);*/

    bool ok;
    QString choice = QInputDialog::getItem(nullptr,
        "Whirlpool Power",
        "Choose line type:",
        QStringList() << "Row" << "Column", 0, false, &ok);
    if (!ok) return false;
    bool isRow = (choice == "Row");

    size_t lineIndex = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Whirlpool Power",
        QString("Enter %1 index (0-%2):").arg(isRow ? "row" : "column").arg(board.getSize() - 1),
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

   /* if (choice != 'r' && choice != 'c') return false;*/

    /*std::cout << "Enter the index for the empty spot on the row/column (0-indexed): ";
    std::cin >> x >> y;*/


    size_t emptyPos = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Whirlpool Power",
        QString("Enter position of empty space in %1 %2 (0-%3):")
        .arg(isRow ? "row" : "column")
        .arg(lineIndex)
        .arg(board.getSize() - 1),
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

     x = isRow ? lineIndex : emptyPos;
     y = isRow ? emptyPos : lineIndex;

    if (!board.checkIndexes(x, y) || !board.m_board[x][y].empty()) {
        QMessageBox::warning(nullptr, "Invalid Move", "Selected position must be empty!");
        return false;
    }

    /*size_t firstIndex = (choice == 'c' ? x : y) - 1;
    size_t secondIndex = (choice == 'c' ? x : y) + 1;*/
    size_t firstIndex = emptyPos - 1;
    size_t secondIndex = emptyPos + 1;


    if (firstIndex > static_cast<size_t>(Card::Value::Four) ||
        secondIndex > static_cast<size_t>(Card::Value::Four))
        return false;

    if (firstIndex >= board.getSize() || secondIndex >= board.getSize()) {
        QMessageBox::warning(nullptr, "Invalid Move", "Invalid adjacent positions!");
        return false;
    }

    if (isRow) {
        if (board.m_board[x][firstIndex].empty() || board.m_board[x][secondIndex].empty()) {
            QMessageBox::warning(nullptr, "Invalid Move", "Adjacent positions must contain cards!");
            return false;
        }
    }
    else {
        if (board.m_board[firstIndex][y].empty() || board.m_board[secondIndex][y].empty()) {
            QMessageBox::warning(nullptr, "Invalid Move", "Adjacent positions must contain cards!");
            return false;
        }
    }

   /* if (choice == 'r') {
        if (board.m_board[x][firstIndex].empty() || board.m_board[x][secondIndex].empty()) return false;
    } else {
        if (board.m_board[firstIndex][x].empty() || board.m_board[secondIndex][x].empty()) return false;
    }*/

    std::unique_ptr<Card> firstCard = std::make_unique<Card>(
        isRow  ? board.m_board[x][firstIndex].back() : board.m_board[firstIndex][y].back());
    std::unique_ptr<Card> secondCard = std::make_unique<Card>(
        isRow  ? board.m_board[x][secondIndex].back() : board.m_board[secondIndex][y].back());

    const auto firstCardValue = firstCard->getValue();
    const auto secondCardValue = secondCard->getValue();

    //bool switchCards;

    /*if (firstCardValue < secondCardValue) std::swap(firstCard, secondCard);

    if (firstCardValue == secondCardValue) {
        std::cout << "Cards have the same value. Choose which card goes on top:\n";
        std::cout << "0. " << *firstCard << "\n1. " << *secondCard << "\n";
        std::cin >> switchCards;

        if (!switchCards) std::swap(firstCard, secondCard);
    }*/

    if (firstCardValue == secondCardValue) {
        QStringList options = {
            QString("Card at position %1").arg(firstIndex),
            QString("Card at position %1").arg(secondIndex)
        };
        QString selected = QInputDialog::getItem(nullptr,
            "Whirlpool Power",
            "Cards have equal value. Choose which card goes on top:",
            options, 0, false, &ok);
        if (!ok) return false;
        if (options.indexOf(selected) == 1) {
            std::swap(firstCard, secondCard);
        }
    }
    else if (firstCardValue < secondCardValue) {
        std::swap(firstCard, secondCard);
    }

    board.m_board[x][y].push_back(std::move(*firstCard));
    board.m_board[x][y].push_back(std::move(*secondCard));

    /*board.m_board[choice == 'r' ? x : firstIndex][choice == 'r' ? firstIndex : y].pop_back();
    board.m_board[choice == 'r' ? x : secondIndex][choice == 'r' ? secondIndex : y].pop_back();*/

    if (isRow) {
        board.m_board[x][firstIndex].pop_back();
        board.m_board[x][secondIndex].pop_back();
    }
    else {
        board.m_board[firstIndex][y].pop_back();
        board.m_board[secondIndex][y].pop_back();
    }

    if (!board.checkBoardIntegrity()) {
        if (isRow) {
            board.m_board[x][secondIndex].push_back(std::move(board.m_board[x][y].back()));
            board.m_board[x][y].pop_back();
            board.m_board[x][firstIndex].push_back(std::move(board.m_board[x][y].back()));
            board.m_board[x][y].pop_back();
        }
        else {
            board.m_board[secondIndex][y].push_back(std::move(board.m_board[x][y].back()));
            board.m_board[x][y].pop_back();
            board.m_board[firstIndex][y].push_back(std::move(board.m_board[x][y].back()));
            board.m_board[x][y].pop_back();
        }
        QMessageBox::warning(nullptr, "Invalid Move", "This move would break board integrity!");
        return false;

       /* board.m_board[choice == 'r' ? x : secondIndex][choice == 'r' ? secondIndex : y].push_back(
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
            );*/

    }
    QMessageBox::information(nullptr, "Success",
        "Successfully combined cards into new stack!");

    return true;
}

bool Power::PowerAction::tsunami(Player &_player, Game &_game, const bool _check) {
    //char line;
    Board &board = _game.m_board;
    /*std::cout << "Tsunami: Choose a row ('r') ora column ('c') to restrict: ";
    std::cin >> line;
    line = tolower(line);

    if (line != 'r' && line != 'c') return false;

    size_t index;
    std::cout << "Enter the index of the row/column to restrict: ";
    std::cin >> index;*/

    bool ok;
    QString lineType = QInputDialog::getItem(nullptr,
        "Tsunami Power",
        "Choose line type to restrict:",
        QStringList() << "Row" << "Column",
        0, false, &ok);
    if (!ok) return false;

    bool isRow = (lineType == "Row");

    size_t index = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Tsunami Power",
        QString("Choose %1 index (0-%2):").arg(isRow ? "row" : "column").arg(board.getSize() - 1),
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (index >= board.getSize()) return false;

    bool hasFreeSpace = false;
    for (size_t i = 0; i < board.getSize(); ++i) {
        for (size_t j = 0; j < board.getSize(); ++j) {
            if (board.m_board[i][j].empty() &&
                !((isRow && i == index) || (!isRow && j == index))) {
                hasFreeSpace = true;
                break;
            }
        }
        if (hasFreeSpace) break;
    }

    if (!hasFreeSpace) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Must have at least one free space outside restricted line!");
        return false;
    }

    Power &power = getInstance();

    if (isRow) power.setRestrictedRow(index);
    else power.setRestrictedCol(index);

    power.setJustBlocked(true);
    QMessageBox::information(nullptr, "Success",
        QString("%1 %2 has been restricted!").arg(lineType).arg(index));
    return true;
}

bool Power::PowerAction::waterfall(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;
    /*char choice;
    size_t index;
    char direction;

    std::cout << "Waterfall: Choose a row ('r') ora column ('c'): ";
    std::cin >> choice;
    choice = tolower(choice);

    if (choice != 'r' && choice != 'c') return false;

    std::cout << "Enter the index of the row/column with at least 3 occupied positions: ";
    std::cin >> index;

    if ((choice == 'r' && index >= board.getSize()) || (choice == 'c' && index >= board.getSize())) return false;*/

    bool ok;
    QString lineType = QInputDialog::getItem(nullptr,
        "Waterfall Power",
        "Choose line type:",
        QStringList() << "Row" << "Column",
        0, false, &ok);
    if (!ok) return false;
    bool isRow = (lineType == "Row");

    size_t index = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Waterfall Power",
        QString("Enter %1 index (0-%2):").arg(isRow ? "row" : "column").arg(board.getSize() - 1),
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    int occupiedPositions = 0;
    if (isRow) {
        for (size_t col = 0; col < board.getSize(); ++col)
            if (!board.m_board[index][col].empty())
                ++occupiedPositions;
    } else {
        for (size_t row = 0; row < board.getSize(); ++row)
            if (!board.m_board[row][index].empty())
                ++occupiedPositions;
    }

    if (occupiedPositions < 3) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Selected line must have at least 3 cards!");
        return false;
    }

    /*if (choice == 'r') std::cout << "Choose the direction of the cascade (a for left, d for right): ";
    else std::cout << "Choose the direction of the cascade (w for up, s for down): ";
    std::cin >> direction;
    direction = tolower(direction);

    if ((choice == 'r' && direction != 'a' && direction != 'd') ||
        (choice == 'c' && direction != 'w' && direction != 's'))
        return false;*/


    QStringList directions;
    if (isRow) {
        directions << "Left" << "Right";
    }
    else {
        directions << "Up" << "Down";
    }

    QString direction = QInputDialog::getItem(nullptr,
        "Waterfall Power",
        "Choose cascade direction:",
        directions, 0, false, &ok);
    if (!ok) return false;


    std::deque<Card> mergedStack;

    if (isRow) {
        if (direction == "Left") {
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
        if (direction == "Up") {
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

    QMessageBox::information(nullptr, "Success",
        QString("Successfully created waterfall in %1 %2!").arg(lineType.toLower()).arg(index));
    return true;
}

bool Power::PowerAction::support(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    /*std::cout << "Support: Choose a card of value 1, 2 or 3 to increase its value by 1.\n";
    std::cout << "Enter the coordinates of the card: ";
    std::cin >> x >> y;*/

    bool ok;
     x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Support Power",
        "Enter row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

     y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Support Power",
        "Enter column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move", "Invalid position!");
        return false;
    }

    if (board.m_board[x][y].empty()) {
        QMessageBox::warning(nullptr, "Invalid Move", "Empty space!");
        return false;
    }

    Card& selectedCard = board.m_board[x][y].back();

    if (_game.m_illusionsAllowed && selectedCard.isIllusion()) {
        QMessageBox::warning(nullptr, "Invalid Move", "Cannot support an illusion!");
        return false;
    }

    if (selectedCard.getColor() != _player.getColor()) {
        QMessageBox::warning(nullptr, "Invalid Move", "Can only support your own cards!");
        return false;
    }
  
    if (selectedCard.getValue() == Card::Value::Four || selectedCard.getValue() == Card::Value::Eter){
        QMessageBox::warning(nullptr, "Invalid Move", "Cannot support cards of value 4 or Eter!");
        return false;
     }
   


    Power &power = Power::getInstance();
    power.setPlus({
                      static_cast<short>(x), static_cast<short>(y),
                      static_cast<short>(_game.getBoard().getBoard()[x][y].size() - 1)
                  }, _game);

    QMessageBox::information(nullptr, "Success", "Card value increased by 1!In case of a tie, the card is worth +1 point.");
    return true;
}

bool Power::PowerAction::earthquake(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;
    bool anyRemoved = false;

    QMessageBox::information(nullptr, "Earthquake Power",
        "Removing all cards with value 1 from the board...");

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

    bool boardEmpty = true;
    if (!anyRemoved) {
        QMessageBox::warning(nullptr, "Earthquake Power",
            "No cards with value 1 could be removed!");
    }
    else {

        for (size_t i = 0; i < board.getSize(); ++i) {
            for (size_t j = 0; j < board.getSize(); ++j) {
                if (!board.m_board[i][j].empty()) {
                    boardEmpty = false;
                    break;
                }
            }
            if (!boardEmpty) break;
        }

        if (boardEmpty) {
            board.m_firstCardPlayed = false;
        }
        QMessageBox::information(nullptr, "Success",
            "Successfully removed all possible cards with value 1!");
    }

    return anyRemoved;
}

bool Power::PowerAction::crumble(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    /*std::cout << "Crumble: Choose an opponent's card of value 2, 3, or 4 to decrease its value by 1.\n";
    std::cout << "Enter the coordinates of the card: ";
    std::cin >> x >> y;*/

    bool ok;
    x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Support Power",
        "Enter row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Support Power",
        "Enter column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move", "Invalid position!");
        return false;
    }
    if (board.m_board[x][y].empty()) {
        QMessageBox::warning(nullptr, "Invalid Move", "Empty space!");
        return false;
    }

    if (_game.m_illusionsAllowed && board.m_board[x][y].back().isIllusion()) {
        QMessageBox::warning(nullptr, "Invalid Move", "You cannot target an illusion!");
        return false;
    }
  

    Card &selectedCard = board.m_board[x][y].back();

    if (selectedCard.getColor() == _player.getColor()) {
        QMessageBox::warning(nullptr, "Invalid Move", "You cannot target your own card!");
        return false;
    }
    if (selectedCard.getValue() == Card::Value::One || selectedCard.getValue() == Card::Value::Eter) {
        QMessageBox::warning(nullptr, "Invalid Move", "You cannot decrease Eter value or cards with value one !");
        return false;
    }

    Power &power = Power::getInstance();
    power.setMinus({
                       static_cast<short>(x), static_cast<short>(y),
                       static_cast<short>(_game.getBoard().getBoard()[x][y].size() - 1)
                   }, _game);

    QMessageBox::information(nullptr, "Crumble Activated",
        QString("The card at position (%1, %2) has been reduced by 1!").arg(x).arg(y));

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

    /*std::cout << "Avalanche: Choose two adjacent stacks on the board.\n";
    std::cout << "Enter the coordinates for the first stack (the 2 stacks will move in the direction of this stack): ";
    std::cin >> x1 >> y1;
    std::cout << "Enter the coordinates for the second stack: ";
    std::cin >> x2 >> y2;*/

    QMessageBox::information(nullptr, "Avalanche Power",
        "Choose two adjacent stacks. These stacks will move in the direction of the first stack.");


    bool ok;
    x1 = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Avalanche Power", "Enter row coordinate for the first stack (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    y1 = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Avalanche Power", "Enter column coordinate for the first stack (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;


    x2 = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Avalanche Power", "Enter row coordinate for the second stack (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    y2 = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Avalanche Power", "Enter column coordinate for the second stack (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.isAPile(x1, y1) || !board.isAPile(x2, y2)) {
        QMessageBox::warning(nullptr, "Invalid Move", "Both selected positions must contain stacks!");
        return false;
    }

    if (!board.checkIndexes(x1, y1) && !board.checkIndexes(x2, y2)){
        QMessageBox::warning(nullptr, "Invalid Move", "Invalid coordinates!");
        return false;
    }

    if (!(x1 == x2 && abs(static_cast<int>(y1) - static_cast<int>(y2)) == 1) && !(
        y1 == y2 && abs(static_cast<int>(x1) - static_cast<int>(x2)) == 1)) {
        QMessageBox::warning(nullptr, "Invalid Move", "Stacks must be adjacent!");
        return false;
        return false;
    }

    if (x1 == x2) {
        if (y1 < y2 && y1 > 0 && board.m_board[x1][y1 - 1].empty()) {
            board.m_board[x1][y1 - 1] = std::move(board.m_board[x1][y1]);
            board.m_board[x1][y1] = std::move(board.m_board[x1][y2]);
            board.m_board[x1][y2].clear();

            if (!board.checkBoardIntegrity()) {
                board.m_board[x1][y2] = std::move(board.m_board[x1][y1]);
                board.m_board[x1][y1] = std::move(board.m_board[x1][y1 - 1]);
                board.m_board[x1][y1 - 1].clear();

                QMessageBox::warning(nullptr, "Invalid Move",
                    "This move would break board integrity!");
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
                QMessageBox::warning(nullptr, "Invalid Move",
                    "This move would break board integrity!");
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
                QMessageBox::warning(nullptr, "Invalid Move",
                    "This move would break board integrity!");
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
                QMessageBox::warning(nullptr, "Invalid Move",
                    "This move would break board integrity!");
                return false;
            }

        }

        else return false;
    }

    else return false;

    QMessageBox::information(nullptr, "Avalanche Activated",
        QString("Stacks at (%1, %2) and (%3, %4) have been moved!")
        .arg(x1).arg(y1).arg(x2).arg(y2));
    return true;
}

bool Power::PowerAction::rock(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    bool hasIllusion = false;

   
    for (size_t i = 0; i < board.getSize(); i++) {
        for (size_t j = 0; j < board.m_board[i].size(); j++) {
            if (!board.m_board[i][j].empty() && board.m_board[i][j].back().isIllusion()) {
                hasIllusion = true;
                break;
            }
        }
        if (hasIllusion) break;
    }

    if (!hasIllusion) {
        QMessageBox::warning(nullptr, "Invalid Action", "There are no illusions to cover!");
        return false;
    }

    if (!_game.m_illusionsAllowed) return false;

    /*std::cout << "Rock: Cover any illusion with a card (from your hand) without flipping the illusion face up.\n";
    std::cout << "Enter the coordinates for the illusion to cover:\n";
    std::cin >> x >> y;*/


    bool ok;
    x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Rock Power", "Enter row coordinate for the illusion you want to cover (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Rock Power", "Enter col coordinate for the illusion you want to cover (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(x, y)) return false;
    if (board.m_board[x][y].empty()) { 
        QMessageBox::warning(nullptr, "Invalid Position", "Selected position does not contain an illusion!");
        return false; 
    }
    if (!board.m_board[x][y].back().isIllusion()) {
        QMessageBox::warning(nullptr, "Invalid Position", "Selected position does not contain an illusion!");
        return false;
    }

   /* std::cout << "Choose a card value to cover the illusion: ";
    int cardValue;
    std::cin >> cardValue;*/

    /*QString selectedValue = QInputDialog::getItem(nullptr,
        "Rock Power",
        "Choose card value to cover illusion:",
        QStringList() << "1" << "2" << "3" << "4",
        0, false, &ok);

    if (!ok) return false;*/

    QString cardValue = QInputDialog::getItem(nullptr, "Select Card",
        "Choose card value to cover illusion:",
        QStringList() << "1" << "2" << "3" << "4", 0, false, &ok);
    if (!ok) return false;

    size_t value = static_cast<size_t>(cardValue.toInt());
    auto selectedCard = _player.useCard(static_cast<Card::Value>(value));

    if (!selectedCard) {
        QMessageBox::warning(nullptr, "Invalid Card", "You don't have this card in your hand!");
        return false;
    }


    board.m_board[x][y].push_back(std::move(*selectedCard));

    return true;
}
