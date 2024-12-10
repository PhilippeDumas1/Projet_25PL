#include "Vehicule.hpp"
#include <cmath>
#include <iomanip>
#include <chrono>

const int WINDOW_SIZE_HORIZ = 1000; // ou toute autre valeur appropriée
const int WINDOW_SIZE_VERTI = 1000;
const double M_PI = 3.14;
const bool DEBUG_MODE = false;

enum SpawnPoint {
    DVG, // Exemple de point de spawn
    // Ajoutez d'autres points de spawn ici
};

Vehicule::Vehicule(int spawn, int direction, int type, sf::Texture& Skin) : _VehiculeType(type) {
    setTexture(Skin); // Remplacez setSkin par setTexture
    int voie = (_VehiculeType == 1) ? 25 : 75; // 25 pour voiture, 75 pour bus et vélo

    switch (spawn) {
    case DVG:
        switch (_VehiculeType) {
        case 1: setPos(0, 575); break;
        case 2: // Bus
            _x = 0;
            _y = 625;
            break;
        case 3: // Vélo
            _x = 0;
            _y = 675;
            break;
        default:
            _x = 0;
            _y = 0; // Position par défaut
            break;
        }
        break;
    }

    _angle = 90 * spawn;
    _speed = 1.0F; // Valeur de base 5.0F
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

bool Vehicule::CanGoForward(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab) {
    // Exemple de logique pour vérifier les collisions avec d'autres véhicules
    for (auto& vehicule : Vehicules) {
        if (this != &vehicule && this->getSprite().getGlobalBounds().intersects(vehicule.getSprite().getGlobalBounds())) {
            return false; // Collision détectée
        }
    }

    // Exemple de logique pour vérifier les feux de signalisation
    for (auto& feu : FeuTab) {
        if (this->getSprite().getGlobalBounds().intersects(feu->getGlobalBounds())) {
            // Logique pour déterminer si le feu est rouge
            // if (feu->isRed()) {
            //     return false;
            // }
        }
    }

    return true; // Aucun obstacle détecté
}


sf::FloatRect Vehicule::getExpandedBounds(float extraLength) {
	return sf::FloatRect();
}

void Vehicule::drawBoundingBox(sf::RenderWindow& window) {
    sf::FloatRect bounds = _Sprite.getGlobalBounds();
    sf::RectangleShape boundingBox;
    boundingBox.setPosition(bounds.left, bounds.top);
    boundingBox.setSize(sf::Vector2f(bounds.width, bounds.height));
    boundingBox.setFillColor(sf::Color::Transparent);
    boundingBox.setOutlineColor(sf::Color::Red);
    boundingBox.setOutlineThickness(1.0f);
    window.draw(boundingBox);
}

void Vehicule::drawDetectionSquare(sf::RenderWindow& window, std::vector<Vehicule>& Vehicules) {
    // Définir la taille du carré de détection (par exemple, 100x100)
    float detectionSize = 100.0f;

    // Créer un carré pour représenter la zone de détection
    sf::RectangleShape detectionSquare;
    detectionSquare.setPosition(_x - detectionSize / 2, _y - detectionSize / 2); // Positionner le carré autour du véhicule
    detectionSquare.setSize(sf::Vector2f(detectionSize, detectionSize)); // Définir la taille du carré
    detectionSquare.setFillColor(sf::Color::Transparent); // Rendre le carré transparent
    detectionSquare.setOutlineColor(sf::Color::Red); // Définir la couleur de la bordure en rouge
    detectionSquare.setOutlineThickness(1.0f); // Définir l'épaisseur de la bordure

    // Dessiner le carré sur la fenêtre
    window.draw(detectionSquare);
}

void Vehicule::move(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab) {
    // Exemple de mise à jour de la position
    _x += _speed * cos(_angle);
    _y += _speed * sin(_angle);
    setPos(_x, _y);
    _Sprite.setPosition(_x, _y); // Mise à jour de la position du sprite
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}


void Vehicule::move(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab) {
    if (CanGoForward(Vehicules, FeuTab)) {
        _x += _speed * cos(_angle * M_PI / 180);
        _y += _speed * sin(_angle * M_PI / 180);
        setPos(_x, _y);
        _Sprite.setPosition(_x, _y);
    }
    else {
        SpeedDown();
    }
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