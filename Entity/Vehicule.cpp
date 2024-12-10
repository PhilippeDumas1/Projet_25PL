#include "Vehicule.hpp"
#include <cmath>
#include <iomanip>

const int WINDOW_SIZE_HORIZ = 1000; // ou toute autre valeur appropri�e
const int WINDOW_SIZE_VERTI = 1000;
const double M_PI = 3.14;
const bool DEBUG_MODE = false;

enum SpawnPoint {
    DVG, // Exemple de point de spawn
    // Ajoutez d'autres points de spawn ici
};

Vehicule::Vehicule(int spawn, int direction, int type, sf::Texture& Skin) : _VehiculeType(type) {
    setTexture(Skin); // Remplacez setSkin par setTexture
    int voie = (_VehiculeType == 1) ? 25 : 75; // 25 pour voiture, 75 pour bus et v�lo

    switch (spawn) {
    case DVG:
        switch (_VehiculeType) {
        case 1: // Voiture
            _x = 0;
            _y = 575;
            break;
        case 2: // Bus
            _x = 0;
            _y = 625;
            break;
        case 3: // V�lo
            _x = 0;
            _y = 675;
            break;
        default:
            _x = 0;
            _y = 0; // Position par d�faut
            break;
        }
        break;
    }

    _angle = 90 * spawn;
    _speed = 5.0F; // Valeur de base 5.0F
    _Patience = 0;
    _direction = (direction % 4);
    _directionPos = sf::Vector2f(-WINDOW_SIZE_VERTI / 2 * sin(M_PI / 2 * _direction) - voie * cos(M_PI / 2 * _direction), WINDOW_SIZE_HORIZ / 2 * cos(M_PI / 2 * _direction) - voie * sin(M_PI / 2 * _direction));

    switch (_VehiculeType) {
    case 1:
        if (DEBUG_MODE) std::cout << "[NEW]\tCar :";
        _Sprite.setOrigin(0, 575);
        break;
    case 2:
        if (DEBUG_MODE) std::cout << "[NEW]\tBus :";
        _Sprite.setOrigin(0, 625);
        break;
    case 3:
        if (DEBUG_MODE) std::cout << "[NEW]\tBike :";
        _Sprite.setOrigin(60, 120);
        break;
    default:
        if (DEBUG_MODE) std::cerr << "ERREUR" << std::endl;
        break;
    }

    if (DEBUG_MODE) std::cout << "\tx:" << _x << "\ty:" << _y << "\ta:" << std::setfill('0') << std::setw(2) << _angle << "\ts:" << spawn << "\td:" << _direction << std::endl;
    _Sprite.setScale(sf::Vector2f(0.2, 0.2));
    setPos(_x, _y);
    setAngle(_angle);

    _Sprite.setPosition(_x, _y);
}

int Vehicule::getVehiculeType() {
	return _VehiculeType;
}

bool Vehicule::HasToTurnLeft() {
	return false;
}

bool Vehicule::HasToTurnRight() {
	return false;
}

bool Vehicule::CanGoForward(std::vector<Vehicule>& Vehicules, std::vector<sf::CircleShape*>& FeuTab) {
    // Exemple de logique pour v�rifier les collisions avec d'autres v�hicules
    for (auto& vehicule : Vehicules) {
        if (this != &vehicule && this->getSprite().getGlobalBounds().intersects(vehicule.getSprite().getGlobalBounds())) {
            return false; // Collision d�tect�e
        }
    }

    // Exemple de logique pour v�rifier les feux de signalisation
    for (auto& feu : FeuTab) {
        if (this->getSprite().getGlobalBounds().intersects(feu->getGlobalBounds())) {
            // Logique pour d�terminer si le feu est rouge
            // if (feu->isRed()) {
            //     return false;
            // }
        }
    }

    return true; // Aucun obstacle d�tect�
}


sf::FloatRect Vehicule::getExpandedBounds(float extraLength) {
	return sf::FloatRect();
}

void Vehicule::drawBoundingBox(sf::RenderWindow& window) {
}

void Vehicule::drawDetectionSquare(sf::RenderWindow& window, std::vector<Vehicule>& Vehicules) {
}

void Vehicule::move(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab) {
    _x += _speed * cos(_angle * M_PI / 180);
    _y += _speed * sin(_angle * M_PI / 180);
    setPos(_x, _y);

}

void Vehicule::SpeedUp() {
    _speed += 1.0F;
}

void Vehicule::SpeedDown() {
    _speed -= 1.0F;
}

Vehicule NewCar(int spawn, int direction, sf::Texture& Skin) {
    return Vehicule(spawn, direction, 1, Skin);
}

void Vehicule::setTexture(const sf::Texture& texture) {
	_Sprite.setTexture(texture);
}