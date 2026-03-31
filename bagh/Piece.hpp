#ifndef PIECE_HPP
#define PIECE_HPP

#include <SFML/Graphics.hpp> 
#include <string>           


enum class PieceType {
    None,
    Goat,
    Tiger
};

// Класс Piece представляет собой одну игровую фишку (козу или тигра).
class Piece {
public:
    
    Piece();

    
    Piece(PieceType type, const sf::Texture& texture, int initialBoardX, int initialBoardY);

    // Метод для установки позиции спрайта на экране.
    void setScreenPosition(float x, float y);

    // Метод для получения спрайта фигуры.
    const sf::Sprite& getSprite() const;

    // Методы для получения и установки логических координат на доске.
    int getBoardX() const;
    int getBoardY() const;
    void setBoardPosition(int x, int y);

    // Метод для получения типа фигуры.
    PieceType getType() const;

private:
    PieceType type;    // Тип фигуры (коза или тигр).
    sf::Sprite sprite; // Спрайт для отображения фигуры.
    int boardX, boardY; // Логические координаты фигуры на доске (например, 0-4 для сетки 5x5).
};

#endif 