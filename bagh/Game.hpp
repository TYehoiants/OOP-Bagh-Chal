// Game.hpp
#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "Piece.hpp"


enum class PlayerTurn {
    GoatPlayer, 
    TigerPlayer 
};


enum class GameState {
    WaitingForGoatPlacement,    // Игрок-коза размещает козу
    WaitingForTigerSelection,   // Игрок-тигр выбирает тигра для хода
    WaitingForTigerMoveDestination, // Игрок-тигр выбирает место для хода/прыжка
    WaitingForGoatMoveSelection, // Игрок-коза выбирает козу для хода (после размещения всех 20)
    WaitingForGoatMoveDestination, // Игрок-коза выбирает место для хода
    GameOver 
};


class Game {
public:
    Game(int width, int height, const std::string& title);
    void run();

private:
    sf::RenderWindow window;
    sf::Texture boardTexture;
    sf::Texture goatTexture;
    sf::Texture tigerTexture;
    sf::Sprite boardSprite;

    std::vector<Piece> goats;
    std::vector<Piece> tigers;

    float boardOffsetX;
    float boardOffsetY;
    float nodeSize;
    std::vector<sf::Vector2f> boardNodePositions; 

    
    PlayerTurn currentPlayerTurn; 
    GameState currentGameState; 

    int selectedPieceIndex;     
    PieceType selectedPieceType; 
    bool isPieceSelected;       

    int goatsToPlaceCount;      // Счетчик коз, которые еще нужно разместить
    int capturedGoatsCount;     // Счетчик съеденных коз (для победы тигра)

    sf::Vector2f goatPoolStartPos; // Начальная позиция для отрисовки пула коз
    float goatPoolSpacing;          // Расстояние между козами в пуле
    int goatsPerRow;                // Сколько коз в одном ряду пула

    sf::RectangleShape highlightRect; // Прямоугольник для выделения

   
    sf::Font font; 
    sf::Text capturedGoatsText; 
    sf::Text gameOverText;      

    

    bool loadTextures();
    void initializePieces();
    sf::Vector2f getScreenPosition(int boardX, int boardY) const;
    void updatePiecePositions();

    void processEvents();
    void handleClick(int mouseX, int mouseY);
    int getNodeUnderMouse(int mouseX, int mouseY) const;
    bool isNodeOccupied(int boardX, int boardY) const;

   
    bool isValidGoatPlacement(int boardX, int boardY) const; 
    bool isValidMove(int fromX, int fromY, int toX, int toY, PieceType pieceType, int& jumpedGoatIndex) const;
    void captureGoat(int goatIndex); 

  
    bool checkTigerWinCondition() const;
    bool checkGoatWinCondition() const;
    bool areTigersBlocked() const; 

    void update();
    void render();
};

#endif 