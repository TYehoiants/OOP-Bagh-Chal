// Game.cpp
#include "Game.hpp" 
#include <algorithm> 
#include <iostream>  
#include <cmath>     
#include <string>    


Game::Game(int width, int height, const std::string& title)
    : window(sf::VideoMode(width, height), title), 
    boardTexture(), goatTexture(), tigerTexture(), 
    boardSprite(), 
    boardOffsetX(0.f), boardOffsetY(0.f), 
    nodeSize(0.f),  
    currentPlayerTurn(PlayerTurn::GoatPlayer), 
    currentGameState(GameState::WaitingForGoatPlacement),  
    selectedPieceIndex(-1),  
    selectedPieceType(PieceType::None), 
    isPieceSelected(false),  
    goatsToPlaceCount(20), 
    capturedGoatsCount(0),  
    
    goatPoolStartPos(650.0f, 50.0f),  
    
    goatPoolSpacing(50.0f), 
    goatsPerRow(5), 
    
    highlightRect(sf::Vector2f(0, 0)) 
{
    
    window.setFramerateLimit(60);  

    
    if (!loadTextures()) { 
        std::cerr << "Error loading textures! Make sure image files exist and paths are correct." << std::endl; 
        
        exit(1); 
    }
    else {
        std::cout << "All textures loaded successfully." << std::endl; // 
        std::cout << "Board texture size: " << boardTexture.getSize().x << "x" << boardTexture.getSize().y << std::endl; 
        std::cout << "Goat texture size: " << goatTexture.getSize().x << "x" << goatTexture.getSize().y << std::endl;  
        std::cout << "Tiger texture size: " << tigerTexture.getSize().x << "x" << tigerTexture.getSize().y << std::endl;  
    }

   
    boardSprite.setTexture(boardTexture); 
    
    
    float desiredBoardDisplaySize = 600.0f; 
   
    float scale = desiredBoardDisplaySize / boardTexture.getSize().x;  
    boardSprite.setScale(scale, scale);  
   
    boardOffsetX = 20.0f;  
   
    boardOffsetY = (height - boardSprite.getGlobalBounds().height) / 2.f; 
    boardSprite.setPosition(boardOffsetX, boardOffsetY);  

    std::cout << "Displayed board size (sprite): " << boardSprite.getGlobalBounds().width << "x" << boardSprite.getGlobalBounds().height << std::endl; 
    std::cout << "Board position (OffsetX, OffsetY): " << boardOffsetX << ", " << boardOffsetY << std::endl; 
    
    float boardPadding = 60.0f; 
     
    
    nodeSize = (boardSprite.getGlobalBounds().width - 2 * boardPadding) / 4.0f;  
    std::cout << "Node size (nodeSize): " << nodeSize << std::endl;  
     
    boardNodePositions.resize(25);  
    
    for (int y = 0; y < 5; ++y) {  
        for (int x = 0; x < 5; ++x) {  
            
            
            float nodeScreenX = boardOffsetX + boardPadding + x * nodeSize; 
            
            float nodeScreenY = boardOffsetY + boardPadding + y * nodeSize;  
            
            int index = y * 5 + x; 
            boardNodePositions[index] = sf::Vector2f(nodeScreenX, nodeScreenY); 
        }
    }

     
    initializePieces(); //инициализация фишек 
    
    float pieceBaseSize = tigers[0].getSprite().getGlobalBounds().width;  
    
    highlightRect.setSize(sf::Vector2f(pieceBaseSize, pieceBaseSize));  
    highlightRect.setFillColor(sf::Color::Transparent); 
    highlightRect.setOutlineThickness(3);  
    
    highlightRect.setOutlineColor(sf::Color::Yellow);  

    
    if (!font.loadFromFile("assets/arial.ttf")) { 
        std::cerr << "Error loading font: assets/arial.ttf" << std::endl;  
        exit(1);  
    }
    else {
        std::cout << "Font loaded successfully." << std::endl; 
    }

    capturedGoatsText.setFont(font);  
    capturedGoatsText.setCharacterSize(24);  
    capturedGoatsText.setFillColor(sf::Color::White); 
    capturedGoatsText.setString("Captured Goats: 0/5"); // Изначальный текст 

    gameOverText.setFont(font);  
    gameOverText.setCharacterSize(60);  
    gameOverText.setFillColor(sf::Color::Red);  
    gameOverText.setString(""); // Изначально пустая строка 

    std::cout << "Bagh Chal game initialized." << std::endl;  
    std::cout << "Current turn: Goat Player (Placement)." << std::endl; 
    }


void Game::run() {
    while (window.isOpen()) {
        processEvents(); 
        update();       
        render(); 
    }
    std::cout << "Game finished." << std::endl;  
}

 
void Game::processEvents() {
    sf::Event event; 
    
    while (window.pollEvent(event)) {  
       
        if (event.type == sf::Event::Closed) {  
            window.close(); 
        }
       
        if (event.type == sf::Event::MouseButtonPressed) { 
            if (event.mouseButton.button == sf::Mouse::Left) { 
                handleClick(event.mouseButton.x, event.mouseButton.y); 
            }
        }
    }
}


void Game::update() {
    
}


void Game::render() {

    window.clear(sf::Color(100, 149, 237));

    //создание доски
    window.draw(boardSprite);


    for (const auto& tiger : tigers) {
        window.draw(tiger.getSprite());
    }

    // Нарисовать только тех коз которые на доске (Их логические коолдинаты не равны -1,-1). 
    for (const auto& goat : goats) {
        if (goat.getBoardX() != -1 && goat.getBoardY() != -1) {
            window.draw(goat.getSprite());
        }
    }


    if (isPieceSelected) {

        window.draw(highlightRect);
    }



    float currentX = goatPoolStartPos.x;
    float currentY = goatPoolStartPos.y;
    int placedInRow = 0;

    float scaledGoatDisplaySize = 40.0f;
    float scaleFactor = 1.0f;


    if (goatTexture.getSize().x > 0) {
        scaleFactor = scaledGoatDisplaySize / goatTexture.getSize().x;
    }
    else {

        std::cerr << "Warning: goatTexture.getSize().x is 0 when drawing goat pool. Check texture loading." << std::endl;

    }

    // Отображаем пул коз только если игра не окончена
    if (currentGameState != GameState::GameOver) {
        for (int i = 0; i < goatsToPlaceCount; ++i) {
            sf::Sprite poolGoatSprite;
            poolGoatSprite.setTexture(goatTexture);
            poolGoatSprite.setScale(scaleFactor, scaleFactor);


            poolGoatSprite.setPosition(currentX, currentY);
            window.draw(poolGoatSprite);

            currentX += goatPoolSpacing;
            placedInRow++;
            if (placedInRow >= goatsPerRow) {
                currentX = goatPoolStartPos.x;
                currentY += goatPoolSpacing;
                placedInRow = 0;
            }
        }

        // Отрисовка счетчика захваченных коз
        capturedGoatsText.setPosition(
            window.getSize().x - capturedGoatsText.getGlobalBounds().width - 20,
            window.getSize().y - capturedGoatsText.getGlobalBounds().height - 20
        );
        window.draw(capturedGoatsText);

        // Отрисовка текста о победе/поражении, если игра окончена
        if (currentGameState == GameState::GameOver) {
            gameOverText.setPosition(
                (window.getSize().x - gameOverText.getGlobalBounds().width) / 2.f,
                (window.getSize().y - gameOverText.getGlobalBounds().height) / 2.f
            );
            window.draw(gameOverText);
        }

        window.display();
    }
}