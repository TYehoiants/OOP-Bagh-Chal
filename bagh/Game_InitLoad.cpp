// Game_InitLoad.cpp
#include "Game.hpp" 
#include <iostream> 

// Загружает все необходимые текстуры из файлов.

bool Game::loadTextures() {
    bool success = true; 
    
    
    if (!boardTexture.loadFromFile("assets/board.png")) { 
        std::cerr << "Failed to load board texture: assets/board.png" << std::endl; 
        success = false; 
    }
    if (!goatTexture.loadFromFile("assets/goat.png")) { 
        std::cerr << "Failed to load goat texture: assets/goat.png" << std::endl; 
        success = false; 
    }
    if (!tigerTexture.loadFromFile("assets/tiger.png")) { 
        std::cerr << "Failed to load tiger texture: assets/tiger.png" << std::endl; 
        success = false; 
    }
    return success; 
}


void Game::initializePieces() { // 
    // Тигры всегда начинают игру на четырех угловых узлах доски 5x5.
    tigers.emplace_back(PieceType::Tiger, tigerTexture, 0, 0); // Верхний левый угол 
    tigers.emplace_back(PieceType::Tiger, tigerTexture, 0, 4); // Верхний правый угол 
    tigers.emplace_back(PieceType::Tiger, tigerTexture, 4, 0); // Нижний левый угол 
    tigers.emplace_back(PieceType::Tiger, tigerTexture, 4, 4); // Нижний правый угол 

    // Козы изначально "вне доски" или "в резерве".
    // Их логические координаты (-1, -1) означают, что они еще не размещены на игровом поле.
    for (int i = 0; i < 20; ++i) { // 
        goats.emplace_back(PieceType::Goat, goatTexture, -1, -1); // 
    }

    
    updatePiecePositions(); 
}