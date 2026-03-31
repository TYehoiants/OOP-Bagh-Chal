// Game_Logic.cpp
#include "Game.hpp" 
#include <iostream> 
#include <cmath>    
#include <algorithm> 
#include <string>    

sf::Vector2f Game::getScreenPosition(int boardX, int boardY) const {
    // Проверяем, что запрошенные логические координаты находятся в допустимом диапазоне (0-4).
    if (boardX < 0 || boardX >= 5 || boardY < 0 || boardY >= 5) {
        // Если координаты недействительны (например, для фигуры, находящейся "вне доски"),
        // возвращаем безопасную, но невидимую позицию.
        return sf::Vector2f(-100.f, -100.f); // Вне видимой части экрана
    }

    // Преобразуем 2D-координаты (x,y) в 1D-индекс для доступа к массиву.
    int index = boardY * 5 + boardX;

    
    return boardNodePositions[index];
}


void Game::updatePiecePositions() {
    // Обновляем позиции тигров.
    for (auto& tiger : tigers) {
        // Получаем координаты центра узла, где находится тигр.
        sf::Vector2f screenPos = getScreenPosition(tiger.getBoardX(), tiger.getBoardY());

        // Получаем текущие размеры спрайта тигра (после масштабирования).
        float pieceWidth = tiger.getSprite().getGlobalBounds().width;
        float pieceHeight = tiger.getSprite().getGlobalBounds().height;

        
        tiger.setScreenPosition(screenPos.x - pieceWidth / 2.f, screenPos.y - pieceHeight / 2.f);
    }

    
    for (auto& goat : goats) {
        // Проверяем, что у козы действительные логические координаты (не -1,-1).
        if (goat.getBoardX() != -1 && goat.getBoardY() != -1) {
            sf::Vector2f screenPos = getScreenPosition(goat.getBoardX(), goat.getBoardY());
            float pieceWidth = goat.getSprite().getGlobalBounds().width;
            float pieceHeight = goat.getSprite().getGlobalBounds().height;
            goat.setScreenPosition(screenPos.x - pieceWidth / 2.f, screenPos.y - pieceHeight / 2.f);
        }
    }
}

// Определяет, над каким узлом доски находится курсор мыши.
// Возвращает 1D-индекс узла (0-24) или -1, если щелчок находится вне узла.
int Game::getNodeUnderMouse(int mouseX, int mouseY) const {
    // Проверяем, находится ли курсор мыши в общей области доски.
    if (mouseX < boardOffsetX || mouseX > boardOffsetX + boardSprite.getGlobalBounds().width ||
        mouseY < boardOffsetY || mouseY > boardOffsetY + boardSprite.getGlobalBounds().height) {
        return -1; // Курсор вне доски
    }

    // Вычисляем смещение курсора относительно НАЧАЛА СЕТКИ УЗЛОВ на доске.
    float boardPadding = 60.0f; 
    float relativeX = mouseX - (boardOffsetX + boardPadding);
    float relativeY = mouseY - (boardOffsetY + boardPadding);

    // Вычисляем приблизительные координаты делением на размер узла
    int boardX = static_cast<int>(std::round(relativeX / nodeSize));
    int boardY = static_cast<int>(std::round(relativeY / nodeSize));
    // Проверяем, что полученные координаты находятся в пределах доски 5x5.
    if (boardX >= 0 && boardX < 5 && boardY >= 0 && boardY < 5) {
        // Возвращаем 1D-индекс узла (0-24)
        return boardY * 5 + boardX;
    }
    return -1; // Щелчок не на узле (например, между узлами или за пределами сетки 5x5)
}

// Проверяет, занят ли данный узел на доске какой-либо фигурой.
bool Game::isNodeOccupied(int boardX, int boardY) const {
    // Проверяем, занят ли узел тигром
    for (const auto& tiger : tigers) {
        if (tiger.getBoardX() == boardX && tiger.getBoardY() == boardY) {
            return true;
        }
    }
    // Проверяем, занят ли узел козой (только козами на доске, а не захваченными)
    for (const auto& goat : goats) {
        if (goat.getBoardX() == boardX && goat.getBoardY() == boardY && goat.getBoardX() != -1) {
            return true;
        }
    }
    return false;
}

// Проверяет, можно ли разместить козу на данном узле.
bool Game::isValidGoatPlacement(int boardX, int boardY) const {
    
    return !isNodeOccupied(boardX, boardY);
}



bool Game::isValidMove(int fromX, int fromY, int toX, int toY, PieceType pieceType, int& jumpedGoatIndex) const {
    jumpedGoatIndex = -1; 

    // Проверка на выход за границы доски
    if (toX < 0 || toX >= 5 || toY < 0 || toY >= 5) {
        return false;
    }

   
    if (isNodeOccupied(toX, toY)) {
        return false;
    }

    int dx = toX - fromX; 
    int dy = toY - fromY; 

    // Проверяем, является ли ход на одну клетку (обычный шаг)
    bool isRegularStep = (std::abs(dx) <= 1 && std::abs(dy) <= 1 && (dx != 0 || dy != 0));
    // Проверяем, является ли ход на две клетки (потенциальный прыжок)
    bool isJump = ((std::abs(dx) == 2 && std::abs(dy) == 0) || // Горизонтальный прыжок
        (std::abs(dx) == 0 && std::abs(dy) == 2) || // Вертикальный прыжок
        (std::abs(dx) == 2 && std::abs(dy) == 2)); // Диагональный прыжок


    //Общая логика для проверки, следует ли ход линиям доски 
    auto checkLineValidity = [](int x1, int y1, int x2, int y2) {
        int tempDx = x2 - x1;
        int tempDy = y2 - y1;

        // Прямые ходы всегда допустимы на сетке
        if (std::abs(tempDx) == 0 || std::abs(tempDy) == 0) {
            return true;
        }

        // Диагональные ходы (dx=1, dy=1 или dx=2, dy=2)
        if (std::abs(tempDx) == std::abs(tempDy)) {
          
            bool sameSumParity = ((x1 + y1) % 2 == (x2 + y2) % 2);
            bool sameDiffParity = ((x1 - y1) % 2 == (x2 - y2) % 2);

            if (sameSumParity || sameDiffParity) {
                return true;
            }
        }
        return false;
        };
    


    // Правила для тигра:
    if (pieceType == PieceType::Tiger) {
        // 1. Обычный ход на соседний пустой узел (на 1 клетку)
        if (isRegularStep) {
            if (checkLineValidity(fromX, fromY, toX, toY)) {
                return true;
            }
        }

        // 2. Прыжок через козу (на 2 клетки)
        if (isJump) {
            int middleX = fromX + dx / 2;
            int middleY = fromY + dy / 2;

            // Проверяем, находится ли промежуточный узел в пределах доски
            if (middleX < 0 || middleX >= 5 || middleY < 0 || middleY >= 5) {
                return false;
            }

            // Проверяем, следует ли прыжок линиям доски
            if (!checkLineValidity(fromX, fromY, toX, toY)) { // Проверяем допустимость линии для всего прыжка
                return false;
            }
            // Также проверяем допустимость линии для отрезка fromX,fromY до middleX,middleY, если прыжок диагональный
            if (std::abs(dx) == 2 && std::abs(dy) == 2) { 
                if (!checkLineValidity(fromX, fromY, middleX, middleY)) {
                    return false;
                }
            }


            // Проверяем, занят ли промежуточный узел козой
            for (size_t i = 0; i < goats.size(); ++i) {
                if (goats[i].getBoardX() == middleX && goats[i].getBoardY() == middleY && goats[i].getBoardX() != -1) {
                    jumpedGoatIndex = static_cast<int>(i); // Найдена коза, через которую прыгают
                    return true; // Допустимый прыжок
                }
            }
        }
        return false; // Ни обычный ход, ни прыжок не допустимы
    }
    // Правила для козы:
    else if (pieceType == PieceType::Goat) {
        
        if (isRegularStep) {
            if (checkLineValidity(fromX, fromY, toX, toY)) {
                return true;
            }
        }
        return false;
    }
    return false; 
}

// Обработка кликов мышью
void Game::handleClick(int mouseX, int mouseY) {
    if (currentGameState == GameState::GameOver) {
        std::cout << "Game Over. Click to restart or close." << std::endl;
        // Можно добавить логику перезапуска игры здесь
        return;
    }

    // Получаем индекс узла доски, по которому был сделан клик.
    // Если клик не на узле, getNodeUnderMouse вернет -1.
    int clickedNodeIndex = getNodeUnderMouse(mouseX, mouseY);
    int clickedBoardX = -1, clickedBoardY = -1;
    if (clickedNodeIndex != -1) {
        clickedBoardX = clickedNodeIndex % 5;
        clickedBoardY = clickedNodeIndex / 5;
    }

    // --- ЛОГИКА ХОДА ИГРОКА-КОЗЫ ---
    if (currentPlayerTurn == PlayerTurn::GoatPlayer) {
        if (currentGameState == GameState::WaitingForGoatPlacement) {
            // Размещение козы
            if (goatsToPlaceCount > 0 && clickedNodeIndex != -1) { // Если есть козы для размещения и клик сделан на узле доски
                if (isValidGoatPlacement(clickedBoardX, clickedBoardY)) {
                    for (auto& goat : goats) {
                        if (goat.getBoardX() == -1 && goat.getBoardY() == -1) { 
                            goat.setBoardPosition(clickedBoardX, clickedBoardY); 
                            goatsToPlaceCount--; 
                            updatePiecePositions(); 
                            std::cout << "Goat placed at (" << clickedBoardX << ", " << clickedBoardY << "). Goats remaining: " << goatsToPlaceCount << std::endl;

                            // Переключаем ход на Тигра
                            currentPlayerTurn = PlayerTurn::TigerPlayer;
                            currentGameState = GameState::WaitingForTigerSelection;
                            std::cout << "Tiger player's turn: Select a tiger to move." << std::endl;

                            
                            if (checkGoatWinCondition()) {
                                currentGameState = GameState::GameOver;
                                gameOverText.setString("Goat Player Wins! Tigers are blocked.");
                                std::cout << "Goat Player Wins! Tigers are blocked." << std::endl;
                            }
                            

                            break; 
                        }
                    }
                }
                else {
                    std::cout << "Cannot place goat at (" << clickedBoardX << ", " << clickedBoardY << "). Node is occupied." << std::endl;
                }
            }
            else {
                std::cout << "Click outside board or no goats to place." << std::endl;
            }
        }
        else if (currentGameState == GameState::WaitingForGoatMoveSelection) {
            // Выбор козы для перемещения (происходит после размещения всех 20)
          
            for (size_t i = 0; i < goats.size(); ++i) {
                // Убедиться, что коза находится на доске (не захвачена)
                if (goats[i].getBoardX() == clickedBoardX && goats[i].getBoardY() == clickedBoardY &&
                    goats[i].getBoardX() != -1) {
                    selectedPieceIndex = static_cast<int>(i); 
                    selectedPieceType = PieceType::Goat;
                    isPieceSelected = true; 
                  
                    highlightRect.setPosition(goats[i].getSprite().getPosition());
                    highlightRect.setSize(goats[i].getSprite().getGlobalBounds().getSize());
                    currentGameState = GameState::WaitingForGoatMoveDestination;
                    std::cout << "Goat selected at (" << clickedBoardX << ", " << clickedBoardY << "). Select a destination." << std::endl;
                    return; // Выходим, так как фигура выбрана
                }
            }
            std::cout << "Click not on your goat. Select a goat to move." << std::endl;
        }
        else if (currentGameState == GameState::WaitingForGoatMoveDestination) {
            // Перемещение выбранной козы
            if (selectedPieceIndex != -1 && selectedPieceType == PieceType::Goat && clickedNodeIndex != -1) {
                int fromX = goats[selectedPieceIndex].getBoardX();
                int fromY = goats[selectedPieceIndex].getBoardY();
                int jumpedGoatIndex = -1; 
                
                if (isValidMove(fromX, fromY, clickedBoardX, clickedBoardY, PieceType::Goat, jumpedGoatIndex)) {
                    goats[selectedPieceIndex].setBoardPosition(clickedBoardX, clickedBoardY);
                    updatePiecePositions();
                    std::cout << "Goat moved from (" << fromX << ", " << fromY << ") to ("
                        << clickedBoardX << ", " << clickedBoardY << ")." << std::endl;

                    // Сброс выбора и переключение хода
                    selectedPieceIndex = -1;
                    selectedPieceType = PieceType::None;
                    isPieceSelected = false; 
                    currentPlayerTurn = PlayerTurn::TigerPlayer;
                    currentGameState = GameState::WaitingForTigerSelection;
                    std::cout << "Tiger player's turn: Select a tiger to move." << std::endl;

                    //Проверяем условие победы коз после перемещения козы, если тигры заблокированы 
                    if (checkGoatWinCondition()) {
                        currentGameState = GameState::GameOver;
                        gameOverText.setString("Goat Player Wins! Tigers are blocked.");
                        std::cout << "Goat Player Wins! Tigers are blocked." << std::endl;
                    }
                   

                }
                else {
                    std::cout << "Invalid move for goat from (" << fromX << ", " << fromY << ") to ("
                        << clickedBoardX << ", " << clickedBoardY << ")." << std::endl;
       
                    selectedPieceIndex = -1;
                    selectedPieceType = PieceType::None;
                    isPieceSelected = false;
                    currentGameState = GameState::WaitingForGoatMoveSelection; 
                }
            }
            else {
                std::cout << "Invalid click or no goat selected. Resetting selection." << std::endl;
                selectedPieceIndex = -1;
                selectedPieceType = PieceType::None;
                isPieceSelected = false;
                currentGameState = GameState::WaitingForGoatMoveSelection; 
            }
        }
    }
    // --- ЛОГИКА ХОДА ИГРОКА-ТИГРА ---
    else if (currentPlayerTurn == PlayerTurn::TigerPlayer) {
        if (currentGameState == GameState::WaitingForTigerSelection) {
            
            for (size_t i = 0; i < tigers.size(); ++i) {
                if (tigers[i].getBoardX() == clickedBoardX && tigers[i].getBoardY() == clickedBoardY) {
                    selectedPieceIndex = static_cast<int>(i); 
                    selectedPieceType = PieceType::Tiger;
                    isPieceSelected = true; 
                    
                    highlightRect.setPosition(tigers[i].getSprite().getPosition());
                    highlightRect.setSize(tigers[i].getSprite().getGlobalBounds().getSize());
                    currentGameState = GameState::WaitingForTigerMoveDestination;
                    std::cout << "Tiger selected at (" << clickedBoardX << ", " << clickedBoardY << "). Select a destination." << std::endl;
                    return; 
                }
            }
            std::cout << "Click not on a tiger. Select a tiger to move." << std::endl;
        }
        else if (currentGameState == GameState::WaitingForTigerMoveDestination) {
            
            if (selectedPieceIndex != -1 && selectedPieceType == PieceType::Tiger && clickedNodeIndex != -1) {
                int fromX = tigers[selectedPieceIndex].getBoardX();
                int fromY = tigers[selectedPieceIndex].getBoardY();
                int jumpedGoatIndex = -1; 

                if (isValidMove(fromX, fromY, clickedBoardX, clickedBoardY, PieceType::Tiger, jumpedGoatIndex)) {
                    tigers[selectedPieceIndex].setBoardPosition(clickedBoardX, clickedBoardY);
                    updatePiecePositions();

                    if (jumpedGoatIndex != -1) { 
                        captureGoat(jumpedGoatIndex); 
                        std::cout << "Tiger captured a goat! Captured goats: " << capturedGoatsCount << std::endl;
                        capturedGoatsText.setString("Captured Goats: " + std::to_string(capturedGoatsCount) + "/5"); 
                    }

                    std::cout << "Tiger moved from (" << fromX << ", " << fromY << ") to ("
                        << clickedBoardX << ", " << clickedBoardY << ")." << std::endl;

                    // Сброс выбора
                    selectedPieceIndex = -1;
                    selectedPieceType = PieceType::None;
                    isPieceSelected = false; 

                    // Проверка условия победы тигра
                    if (checkTigerWinCondition()) {
                        currentGameState = GameState::GameOver;
                        gameOverText.setString("Tiger Player Wins! (5 goats captured)");
                        std::cout << "Tiger Player Wins! (5 goats captured)" << std::endl;
                    }
                    else {
                        // Переключаем ход
                        if (goatsToPlaceCount > 0) {
                            currentPlayerTurn = PlayerTurn::GoatPlayer;
                            currentGameState = GameState::WaitingForGoatPlacement;
                            std::cout << "Goat player's turn: Place a goat." << std::endl;
                        }
                        else {
                            // Если все козы размещены, козы начинают двигаться
                            currentPlayerTurn = PlayerTurn::GoatPlayer;
                            currentGameState = GameState::WaitingForGoatMoveSelection;
                            std::cout << "Goat player's turn: Select a goat to move." << std::endl;
                        }

                       
                        if (checkGoatWinCondition()) {
                            currentGameState = GameState::GameOver;
                            gameOverText.setString("Goat Player Wins! Tigers are blocked.");
                            std::cout << "Goat Player Wins! Tigers are blocked." << std::endl;
                        }
                        
                    }

                }
                else {
                    std::cout << "Invalid move for tiger from (" << fromX << ", " << fromY << ") to ("
                        << clickedBoardX << ", " << clickedBoardY << ")." << std::endl;
                    
                    selectedPieceIndex = -1;
                    selectedPieceType = PieceType::None;
                    isPieceSelected = false;
                    currentGameState = GameState::WaitingForTigerSelection; 
                }
            }
            else {
                std::cout << "Invalid click or no tiger selected. Resetting selection." << std::endl;
                selectedPieceIndex = -1;
                selectedPieceType = PieceType::None;
                isPieceSelected = false;
                currentGameState = GameState::WaitingForTigerSelection;
            }
        }
    }
}

// Захватывает козу, перемещая ее с доски.
void Game::captureGoat(int goatIndex) {
    if (goatIndex >= 0 && goatIndex < goats.size()) {
        goats[goatIndex].setBoardPosition(-1, -1); 
        capturedGoatsCount++; 
        updatePiecePositions(); 
    }
}


bool Game::checkTigerWinCondition() const {
    return capturedGoatsCount >= 5;
}


bool Game::checkGoatWinCondition() const {
    return areTigersBlocked(); 
}

// Вспомогательный метод: Проверяет, заблокированы ли тигры
bool Game::areTigersBlocked() const {
    // Если есть хотя бы один тигр, который может сделать валидный ход, тигры не заблокированы
    for (const auto& tiger : tigers) {
        int currentX = tiger.getBoardX();
        int currentY = tiger.getBoardY();

        // Проверяем все возможные соседние и "прыжковые" позиции
        // Тигр может перемещаться на 1 или 2 клетки (для прыжка)
        // Обходим 5x5 матрицу возможных целевых позиций относительно тигра
        // Это покрывает все 8 соседних узлов и 8 узлов для прыжка
        for (int dy = -2; dy <= 2; ++dy) {
            for (int dx = -2; dx <= 2; ++dx) {
                
                if (dx == 0 && dy == 0) {
                    continue;
                }

                int targetX = currentX + dx;
                int targetY = currentY + dy;
                int tempJumpedGoatIndex = -1; 

                
                if (isValidMove(currentX, currentY, targetX, targetY, PieceType::Tiger, tempJumpedGoatIndex)) {
                    
                    return false;
                }
            }
        }
    }
    
    return true;
}