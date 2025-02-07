#include "../Actions/Wizard.h"
#include "../GameMechanics/Board.h"
#include "../GameMechanics/Game.h"

bool Wizard::WizardActions::eliminateCard(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;

    bool ok;
    x = static_cast<size_t>(QInputDialog::getInt(nullptr, "Eliminate Card",
        "Enter row coordinate (0-3):", 0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;
    y = static_cast<size_t> (QInputDialog::getInt(nullptr, "Eliminate Card",
        "Enter column coordinate (0-3):", 0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

   /* std::cout << "Eliminate an opponent's card that covers your own.\n";
    std::cout << "Enter (x, y) coordinates (0-indexed): ";
    std::cin >> x >> y;*/

    if (!board.checkIndexes(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move", "Invalid coordinates!");
        return false;
    }

    if (board.m_board[x][y].empty() || !board.isAPile(x, y)){
        QMessageBox::warning(nullptr, "Invalid Move", "No cards or not a pile at this position!");
        return false;
    }

    if (board.m_board[x][y].back().getColor() == _player.getColor()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Cannot eliminate your own top card!");
        return false;
    }

    size_t secondLastCardIndex = board.m_board[x][y].size() - 2;
    if (board.m_board[x][y][secondLastCardIndex].getColor() != _player.getColor()){
        QMessageBox::warning(nullptr, "Invalid Move",
            "Your card must be directly under the opponent's card!");
        return false;
    }
       

    Card eliminatedCard = std::move(board.m_board[x][y].back());
    board.m_board[x][y].pop_back();

    _game.m_eliminatedCards.push_back(std::move(eliminatedCard));

    QMessageBox::information(nullptr, "Success",
        "Card eliminated successfully!");
    return true;
}

bool Wizard::WizardActions::eliminateRow(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;
    const size_t boardSize = board.m_board.size();

    QStringList choices;
    choices << "Row" << "Column";
    bool ok;
    QString choice = QInputDialog::getItem(nullptr,
        "Eliminate Row/Column",
        "Choose to eliminate a row or column:",
        choices,
        0, false, &ok);

    if (!ok || choice.isEmpty())
        return false;


    size_t index = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Choose Index",
        QString("Enter %1 index (0-%2):").arg(choice.toLower()).arg(boardSize - 1),
        0, 0, boardSize - 1, 1, &ok));

    if (!ok || index >= boardSize) {
        QMessageBox::warning(nullptr, "Invalid Input", "Invalid index selected!");
        return false;
    }

    /*std::cout << "Eliminate an entire row or column of stacks.\n";
    std::cout << "Enter 'r' for row or 'c' for column: ";

    do {
        std::cin >> choice;
    }
    while (tolower(choice) != 'r' && towlower(choice) != 'c');*/

    /*std::cout << "Enter the index (0-indexed): ";
    std::cin >> index;

    if (index >= boardSize)
        return false;*/

    size_t ownVisibleCards = 0;
    size_t nonEmptyStacks = 0;

    if (choice == "Row") {
        const auto &row = board.m_board[index];
        for (const auto &stack: row) {
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
            const auto &stack = board.m_board[i][index];
            if (!stack.empty()) {
                nonEmptyStacks++;
                const Card &topCard = stack.back();
                if (topCard.getColor() == _player.getColor() && !topCard.isIllusion())
                    ownVisibleCards++;
            }
        }
    }

    if (nonEmptyStacks < 3) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "There must be at least 3 cards in the selected row/column!");
        return false;
    }

    if (ownVisibleCards == 0) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "You must have at least one visible card in the selected row/column!");
        return false;
    }

    std::vector<std::deque<Card>> savedSection;
    if (choice == "Row") {
        savedSection = board.m_board[index];
        for (auto &stack: board.m_board[index])
            while (!stack.empty()) {
                _game.m_eliminatedCards.push_back(std::move(stack.back()));
                stack.pop_back();
            }
    }

    else {
        savedSection.resize(boardSize);
        for (size_t i = 0; i < boardSize; ++i) {
            savedSection[i] = std::move(board.m_board[i][index]);
            while (!board.m_board[i][index].empty()) {
                _game.m_eliminatedCards.push_back(std::move(board.m_board[i][index].back()));
                board.m_board[i][index].pop_back();
            }
        }
    }

    if (!board.checkBoardIntegrity()) {
        if (choice =="Row") {
            board.m_board[index] = std::move(savedSection);
        } else {
            for (size_t i = 0; i < boardSize; ++i) {
                board.m_board[i][index] = std::move(savedSection[i]);
            }
        }
        _game.m_eliminatedCards.erase(_game.m_eliminatedCards.end() - nonEmptyStacks,
                                   _game.m_eliminatedCards.end());
        QMessageBox::warning(nullptr, "Invalid Move",
            "This move would break the board's integrity!");
        return false;
    }

    QMessageBox::information(nullptr, "Success",
        QString("Successfully eliminated %1 %2!").arg(choice.toLower()).arg(index));

    return true;
}

bool Wizard::WizardActions::coverCard(Player &_player, Game &_game, const bool _check) {
    size_t x, y, cardValue;
    Board &board = _game.m_board;

    /*std::cout << "Cover an opponent's card with one of your own of strictly lower value.\n";
    std::cout << "Enter (x, y) coordinates for the target card (0-indexed) and the card value: ";
    std::cin >> x >> y >> cardValue;*/

    bool ok;
    x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Cover Card",
        "Enter row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Cover Card",
        "Enter column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Invalid coordinates!");
        return false;
    }

    auto &targetStack = board.m_board[x][y];
    if (targetStack.empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Selected position is empty!");
        return false;
    }

    const Card &target = targetStack.back();

    if (target.isIllusion()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Cannot cover an illusion card!");
        return false;
    }

    if (target.getColor() == _player.getColor()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Cannot cover your own card!");
        return false;
    }

     cardValue = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Cover Card",
        "Enter card value (must be lower than target card):",
        1, 1, 4, 1, &ok));
    if (!ok) return false;

    if (cardValue >= static_cast<int>(target.getValue())) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Card value must be lower than the target card!");
        return false;
    }

    auto selectedCard = _player.useCard(static_cast<Card::Value>(cardValue));
    if (!selectedCard) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "You don't have a card with this value!");
        return false;
    }

    targetStack.push_back(std::move(*selectedCard));

    QMessageBox::information(nullptr, "Success",
        "Card successfully covered!");

    return true;
}

bool Wizard::WizardActions::sinkHole(Player &_player, Game &_game, const bool _check) {
    Board &board = _game.m_board;

    /*std::cout << "Transform an empty space on the board into a sinkhole.\n";
    std::cout << "Enter (x, y) coordinates for the empty space (0-indexed): ";
    std::cin >> x >> y;*/
    bool ok;

    size_t x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Create Sinkhole",
        "Enter row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    size_t y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Create Sinkhole",
        "Enter column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Invalid coordinates!");
        return false;
    }

    if (!board.m_board[y][x].empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Selected position is not empty!");
        return false;
    }

    if (board.checkHole(y, x)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "A sinkhole already exists at this position!");
        return false;
    }

    getInstance().setHole(std::make_pair(y, x));

    QMessageBox::information(nullptr, "Success",
        "Sinkhole created successfully!");

    return true;
}

bool Wizard::WizardActions::moveStackOwn(Player &_player, Game &_game, const bool _check) {
    size_t startX, startY, endX, endY;
    Board &board = _game.m_board;
    bool ok;
    /*std::cout << "Move a stack with your own card on top to an empty position.\n";
    std::cout << "Enter the coordinates of the stack:\n";
    std::cin >> startX >> startY;*/

    startX = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Move Stack",
        "Enter source row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    startY = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Move Stack",
        "Enter source column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(startX, startY)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Invalid source coordinates!");
        return false;
    }

    if (board.m_board[startX][startY].empty() || board.m_board[startX][startY].size() < 2) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Selected position is empty or contains only one card!");
        return false;
    }

    if (board.m_board[startX][startY].back().getColor() != _player.getColor()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Top card must be your own!");
        return false;
    }


    /*std::cout << "Enter coordinates for the stack destination:\n";
    std::cin >> endX >> endY;*/

    endX = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Move Stack",
        "Enter destination row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    endY = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Move Stack",
        "Enter destination column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(endX,endY)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Invalid coordinates!");
        return false;
    }

    if (!board.m_board[endX][endY].empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Destination position must be empty!");
        return false;
    }

    board.m_board[endX][endY] = std::move(board.m_board[startX][startY]);
    board.m_board[startX][startY].clear();

    if (board.checkBoardIntegrity()) {
        QMessageBox::information(nullptr, "Success",
            "Stack moved successfully!");
        return true;
    }

    board.m_board[startX][startY] = std::move(board.m_board[endX][endY]);
    board.m_board[endX][endY].clear();

    QMessageBox::warning(nullptr, "Invalid Move",
        "Move would break board integrity!");

    return false;
}

bool Wizard::WizardActions::extraEter(Player &_player, Game &_game, const bool _check) {
    size_t x, y;
    Board &board = _game.m_board;
    bool ok;
    /*std::cout << "You received an extra Eter card. Place now!\n";
    std::cout << "Enter (x, y) coordinates for extra Eter:\n";
    std::cin >> x >> y;*/

    x = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Place Extra Eter",
        "Enter row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    y = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Place Extra Eter",
        "Enter column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(x, y)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Invalid coordinates!");
        return false;
    }

    if (!board.m_board[x][y].empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Selected position must be empty!");
        return false;
    }

    board.m_board[x][y].emplace_back(Card::Value::Eter, _player.getColor());
    QMessageBox::information(nullptr, "Success",
        "Extra Eter card placed successfully!");
    return true;
}

bool Wizard::WizardActions::moveStackOpponent(Player &_player, Game &_game, const bool _check) {
    size_t startX, startY, endX, endY;
    Board &board = _game.m_board;
    bool ok;
    /*std::cout << "Move an opponent's stack with your card on top to an empty position.\n";
    std::cout << "Enter coordinates of the stack:\n";
    std::cin >> startX >> startY;*/

    startX = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Move Opponent Stack",
        "Enter source row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    startY = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Move Opponent Stack",
        "Enter source column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(startX,startY)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Invalid source coordinates!");
        return false;
    }

    if (board.m_board[startX][startY].empty() || board.m_board[startX][startY].size() < 2){
        QMessageBox::warning(nullptr, "Invalid Move",
            "Selected position is empty or not a stack!");
        return false;
    }


    if (board.m_board[startX][startY].back().getColor() == _player.getColor()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Must select opponent's stack!");
        return false;
    }

    /*std::cout << "Enter coordinates for the destination of the stack:\n";
    std::cin >> endX >> endY;*/

    endX = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Move Opponent Stack",
        "Enter destination row coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    endY = static_cast<size_t>(QInputDialog::getInt(nullptr,
        "Move Opponent Stack",
        "Enter destination column coordinate (0-3):",
        0, 0, board.getSize() - 1, 1, &ok));
    if (!ok) return false;

    if (!board.checkIndexes(endX,endY)) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Invalid destination coordinates!");
        return false;
    }

    if (!board.m_board[endX][endY].empty()) {
        QMessageBox::warning(nullptr, "Invalid Move",
            "Destination position must be empty!");
        return false;
    }


    board.m_board[endX][endY] = std::move(board.m_board[startX][startY]);
    board.m_board[startX][startY].clear();

    if (board.checkBoardIntegrity()) {
        QMessageBox::information(nullptr, "Success",
            "Stack moved successfully!");
        return true;
    }

    board.m_board[startX][startY] = std::move(board.m_board[endX][endY]);
    board.m_board[endX][endY].clear();

    QMessageBox::warning(nullptr, "Invalid Move",
        "Move would break board integrity!");

    return false;
}

bool Wizard::WizardActions::moveEdge(Player& _player, Game& _game, const bool _check) {
    Board& board = _game.m_board; 
    /*std::cout << "Move edge of the playing field and move it to a different edge\n";
    char choice;
    std::cout << "Choose edge (w: up, a: left, s: down, d: right): ";
    std::cin >> choice;*/

    QStringList choices;
    choices << "Up" << "Left" << "Down" << "Right";
    bool ok;
    QString choice = QInputDialog::getItem(nullptr,
        "Move Edge",
        "Choose edge to move:",
        choices,
        0, false, &ok);

    /*if (choice != 'w' && choice != 'a' && choice != 's' && choice != 'd')
        return false;*/

    if (!ok || choice.isEmpty()) return false;

    if (board.m_board.empty()) {
        QMessageBox::warning(nullptr, "Invalid Move", "Board is empty!");
        return false;
    }

    auto countOccupiedInRow = [](const std::vector<std::deque<Card>>& row) -> int {
        int count = 0;
        for (const auto& card : row) {
            if (!card.empty()) {
                count++;
            }
        }
        return count;
    };

    auto countOccupiedInColumn = [](const std::vector<std::vector<std::deque<Card>>>& board, size_t colIndex) -> int {
        int count = 0;
        for (const auto& row : board) {
            if (!row[colIndex].empty()) {
                count++;
            }
        }
        return count;
    };

    std::vector<std::deque<Card>> movedLine;
    char moveDirection;
    if (choice == "Up") moveDirection = 'w';
    else if (choice == "Left") moveDirection = 'a';
    else if (choice == "Down") moveDirection = 's';
    else if (choice == "Right") moveDirection = 'd';
    else return false;

    switch (moveDirection) {
    case 'w': {
        if (countOccupiedInRow(board.m_board[0]) < 3) return false;

        movedLine = std::move(board.m_board[0]);
        board.m_board[0].resize(board.getSize());
        board.circularShiftUp();
        board.m_board[board.m_board.size() - 1] = std::move(movedLine);
        break;
    }
    case 'a': {
        if (countOccupiedInColumn(board.m_board, 0) < 3) return false;

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
        if (countOccupiedInRow(board.m_board[board.m_board.size() - 1]) < 3) return false;

        movedLine = std::move(board.m_board[board.m_board.size() - 1]);
        board.m_board[board.m_board.size() - 1].resize(board.getSize());
        board.circularShiftDown();
        board.m_board[0] = std::move(movedLine);
        break;
    }
    case 'd': {
        if (countOccupiedInColumn(board.m_board, board.m_board.size() - 1) < 3) return false;

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

    if (board.checkBoardIntegrity()) {
        QMessageBox::information(nullptr, "Success", "Edge moved successfully!");
        return true;
    }

    movedLine.clear();

    switch (moveDirection) {
        case 's': {
            movedLine = std::move(board.m_board[0]);
            board.m_board[0].resize(board.getSize());
            board.circularShiftUp();
            board.m_board[board.m_board.size() - 1] = std::move(movedLine);
            break;
        }
        case 'd': {
            for (auto& row : board.m_board) {
                movedLine.emplace_back(std::move(row[0]));
            }
            board.circularShiftLeft();
            for (size_t i = 0; i < board.m_board.size(); ++i) {
                board.m_board[i][board.m_board[i].size() - 1] = std::move(movedLine[i]);
            }
            break;
        }
        case 'w': {
            movedLine = std::move(board.m_board[board.m_board.size() - 1]);
            board.m_board[board.m_board.size() - 1].resize(board.getSize());
            board.circularShiftDown();
            board.m_board[0] = std::move(movedLine);
            break;
        }
        case 'a': {
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
            break;
        }
    }
    QMessageBox::warning(nullptr, "Invalid Move", "Move would break board integrity!");
    return false;
}
