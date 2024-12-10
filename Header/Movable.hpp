#ifndef MOVABLE_HPP
#define MOVABLE_HPP

#include <SFML/Graphics.hpp>

class Movable {
public:
    virtual float getX() { return _x; }
    virtual float getY() { return _y; }
    virtual sf::Vector2f getPos() { return sf::Vector2f(_x, _y); }
    virtual float getAngle() { return _angle; }
    virtual int getDirection() const { return _direction; }
    virtual float getPatience() { return _Patience; }
    virtual sf::Vector2f getDirectionPos() { return _directionPos; }
    virtual sf::Sprite getSprite() { return _Sprite; }

    virtual void setX(float x) { _x = x; }
    virtual void setY(float y) { _y = y; }
    virtual void setAngle(float angle) { _angle = angle; }
    virtual void setSkin(sf::Texture& Skin) { _Sprite.setTexture(Skin); }
    virtual void setSprite(sf::Sprite Sprite) { _Sprite = Sprite; }
    virtual void setPos(float x, float y) {
        _x = x;
        _y = y;
        _Sprite.setPosition(x, y); // Mise à jour de la position du sprite
    }

protected:
    float _x, _y;
    float _angle;
    float _speed;
    int _direction;
    float _Patience;
    sf::Vector2f _directionPos;
    sf::Sprite _Sprite;
};

#endif // MOVABLE_HPP
