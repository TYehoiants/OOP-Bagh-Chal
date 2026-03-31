#include "Piece.hpp" 


Piece::Piece()
    : type(PieceType::None), boardX(-1), boardY(-1) {
    
}

// Основной конструктор
Piece::Piece(PieceType type, const sf::Texture& texture, int initialBoardX, int initialBoardY)
    : type(type), boardX(initialBoardX), boardY(initialBoardY) {
    sprite.setTexture(texture); 
}

// Устанавливает позицию спрайта на экране.
void Piece::setScreenPosition(float x, float y) {
    sprite.setPosition(x, y);
}

// Возвращает спрайт фигуры.
const sf::Sprite& Piece::getSprite() const {
    return sprite;
}

// Возвращает логическую координату X на доске.
int Piece::getBoardX() const {
    return boardX;
}

// Возвращает логическую координату Y на доске.
int Piece::getBoardY() const {
    return boardY;
}

// Устанавливает логические координаты на доске.
void Piece::setBoardPosition(int x, int y) {
    boardX = x;
    boardY = y;
}

// Возвращает тип фигуры.
PieceType Piece::getType() const {
    return type;
}