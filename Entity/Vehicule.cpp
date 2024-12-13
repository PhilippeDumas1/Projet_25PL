#include "Vehicule.hpp"
#include <cmath>
#include <iomanip>
#include <chrono>

const int WINDOW_SIZE_HORIZ = 1000; // ou toute autre valeur appropriée
const int WINDOW_SIZE_VERTI = 1000;
const double M_PI = 3.14;
const bool DEBUG_MODE = false;
const float MAX_SPEED = 35.0F;
const float MIN_SPEED = 25.0F;

enum SpawnPoint {
    DH, // Début haut
	DG, // Début gauche
	DD, // Début droite
	DB, // Début bas
};

enum VehiculeType {
	Car = 1,
	Bus = 2,
	Bike = 3
};

Vehicule::Vehicule(int spawn, int direction, int type, sf::Texture& Skin) : _VehiculeType(type), _currentPathIndex(0), _currentDirectionIndex(0) {
    setTexture(Skin);

    // Définir les chemins pour chaque direction
    //voiture Gauche
    _paths[0] = { {sf::Vector2f(0, 575), sf::Vector2f(1000, 575)} };
    _paths[1] = { {sf::Vector2f(0, 575), sf::Vector2f(425, 575), sf::Vector2f(425, 1000)} };
    _paths[2] = { {sf::Vector2f(0, 575), sf::Vector2f(475, 575), sf::Vector2f(475, 0)} };
	//----------------
	//voiture Droite
	_paths[2] = { {sf::Vector2f(1000, 450), sf::Vector2f(0, 450)} };

    switch (spawn) {
    case DG:
        switch (_VehiculeType) {
        case 1: _x = 0; _y = 575; break; //Voiture
        case 2: _x = 0; _y = 625; break; //Bus
		case 3: _x = 0; _y = 0;   break; //Vélo a faire
        default:_x = 0; _y = 0;   break;
        }
        _angle = 0;
        break;
    case DH:
        switch (_VehiculeType) {
        case 1: _x = 425; _y = 0; break; // Voiture
        case 2: _x = 500; _y = 0; break; // Bus
        case 3: _x = 550; _y = 0; break; // Vélo
        default: _x = 0; _y = 0; break;
        }
        _angle = 90; // Déplacement vers le bas
        break;
    case DD:
        switch (_VehiculeType) {
        case 1: _x = 1000; _y = 450; break; // Voiture
        case 2: _x = 1000; _y = 500; break; // Bus
        case 3: _x = 1000; _y = 550; break; // Vélo
        default: _x = 0; _y = 0; break;
        }
        _angle = 180; // Déplacement vers la gauche
        break;
    case DB:
        switch (_VehiculeType) {
        case 1: _x = 450; _y = 1000; break; // Voiture
        case 2: _x = 500; _y = 1000; break; // Bus
        case 3: _x = 550; _y = 1000; break; // Vélo
        default: _x = 0; _y = 0; break;
        }
        _angle = 270; // Déplacement vers le haut
        break;
    default:
        _x = 0; _y = 0; _angle = 0; break;
    }

    _speed = 25.0F;
    _Patience = 0;
    _direction = (direction % 4);
    _directionPos = sf::Vector2f(static_cast<float>(-WINDOW_SIZE_VERTI / 2 * sin(M_PI / 2 * _direction) * cos(M_PI / 2 * _direction)),
                                 static_cast<float>(WINDOW_SIZE_HORIZ / 2 * cos(M_PI / 2 * _direction) * sin(M_PI / 2 * _direction)));

    switch (_VehiculeType) {
    case 1:
        if (DEBUG_MODE) std::cout << "[NEW]\tCar :";
        _Sprite.setOrigin(0, 0);
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
    
    _Sprite.setScale(sf::Vector2f(0.5, 0.5));

    // Set the position
    setPos(_x, _y);
    setAngle(_angle);
    _Sprite.setPosition(_x, _y);
    _Sprite.setRotation(_angle);

    sf::FloatRect bounds = _Sprite.getLocalBounds();
    _Sprite.setOrigin(bounds.width / 2, bounds.height / 2);

    determinePath(_direction);
    updateSpriteRotation();
}

//----------------------------------------------------------------

int Vehicule::getVehiculeType() {
	return _VehiculeType;
}

//----------------------------------------------------------------

bool Vehicule::CanTurnLeft(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab) {
    for (auto& feu : FeuTab) {
        if (feu->get_traffic_color() == Traffic_color::red) {
            return false;
        }
    }

    // Vérifier s'il y a un autre véhicule dans la zone de détection
    sf::FloatRect expandedBounds = getExpandedBounds(10.0f);
    for (auto& vehicule : Vehicules) {
        if (this != &vehicule && expandedBounds.intersects(vehicule.getExpandedBounds(10.0f))) {
            return false; // Collision détectée
        }
    }

    // Si aucune des conditions ci-dessus n'est remplie, le véhicule peut tourner à gauche
    return true;
}

bool Vehicule::CanTurnRight(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab) {
	return false;
}

bool Vehicule::CanGoForward(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab) {
    sf::FloatRect expandedBounds = getExpandedBounds(10.0f);

    for (auto& vehicule : Vehicules) {
        if (this != &vehicule && expandedBounds.intersects(vehicule.getExpandedBounds(10.0f))) {
            if (_speed == 0 && vehicule._speed == 0) {
                if (_Patience > vehicule._Patience) {
                    vehicule.SpeedDown();
                    return true;
                }
                else {
                    SpeedDown();
                    return false;
                }
            }
            return false; // Collision détectée
        }
    }

    for (auto& feu : FeuTab) {
        if (expandedBounds.intersects(feu->getGlobalBounds())) {
            if (feu->get_traffic_color() == Traffic_color::red) {
                return false;
            }
			else if (feu->get_traffic_color() == Traffic_color::orange) {
				SpeedDown();
				return true;
			}
            else if (feu->get_traffic_color() == Traffic_color::green) {
                return true;
            }
        }
    }
    return true; // Aucun obstacle détecté
}

bool Vehicule::isOutOfBounds(int windowWidth, int windowHeight) const {
    return _x < 0 || _x > windowWidth || _y < 0 || _y > windowHeight;
}

//----------------------------------------------------------------

sf::FloatRect Vehicule::getExpandedBounds(float extraLength) {
    sf::FloatRect bounds = _Sprite.getGlobalBounds();
    bounds.left -= extraLength;
    bounds.top -= extraLength;
    bounds.width += 2 * extraLength;
    bounds.height += 2 * extraLength;
    return bounds;
}

//----------------------------------------------------------------

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

void Vehicule::move(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab, float deltaTime) {
    if (CanGoForward(Vehicules, FeuTab)) {
        if (_speed == 0) {
            SpeedUp();
        }

        if (_currentPathIndex < _currentPath[_currentDirectionIndex].points.size()) {
            sf::Vector2f target = _currentPath[_currentDirectionIndex].points[_currentPathIndex];
            sf::Vector2f direction = target - sf::Vector2f(_x, _y);
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            sf::Vector2f unitDirection = direction / length;

            if (length != 0) {
                _x += unitDirection.x * _speed * deltaTime;
                _y += unitDirection.y * _speed * deltaTime;
            }

            setPos(_x, _y);
            _Sprite.setPosition(_x, _y);

            // Vérifier si le véhicule a atteint la fin du segment de chemin actuel
            if (std::abs(_x - target.x) < 1.0f && std::abs(_y - target.y) < 1.0f) {
                _currentPathIndex++;
                
                if (_currentPathIndex < _currentPath[_currentDirectionIndex].points.size()) {
                    updateDirection(_currentPath[_currentDirectionIndex].points[_currentPathIndex]);
                    updateSpriteRotation();
                }
                else {
                    _currentPathIndex = 0;
                    _currentDirectionIndex++;
                    if (_currentDirectionIndex < _directions.size()) {
                        determinePath(_directions[_currentDirectionIndex]);
                        updateDirection(_currentPath[_currentDirectionIndex].points[_currentPathIndex]);
                        updateSpriteRotation();
                    }
                }
            }
        }
    }
    else {
        SpeedDown();
    }
}

void Vehicule::SpeedUp() {
    if (_speed == 0) {
        _speed = MIN_SPEED; // Reprendre à la vitesse minimale de base
    }
    else {
        _speed += 1.0F;
        if (_speed > MAX_SPEED) {
            _speed = MAX_SPEED;
        }
    }
}

void Vehicule::SpeedDown() {
    _speed -= 1.0F;
    if (_speed < 0) {
        _speed = 0;
    }
}

void Vehicule::TurnLeft() {
    _angle -= 90; // Ajustez l'angle pour tourner à gauche
    if (_angle < 0) {
        _angle += 360;
    }
    _Sprite.setRotation(_angle);
}

void Vehicule::TurnRight() {
    _angle += 90; // Ajustez l'angle pour tourner à droite
    if (_angle >= 360) {
        _angle -= 360;
    }
	_Sprite.setRotation(_angle);
}

void Vehicule::setTexture(const sf::Texture& texture) {
	_Sprite.setTexture(texture);
}

void Vehicule::determinePath(int direction) {
    _currentPath.clear();
    _currentPath.push_back(_paths[direction]);
    _currentPathIndex = 0;
}

void Vehicule::setDirections(const std::vector<int>& directions) {
    _directions = directions;
    _currentDirectionIndex = 0;
    if (!_directions.empty()) {
        determinePath(_directions[_currentDirectionIndex]);
    }
}

void Vehicule::updateDirection(const sf::Vector2f& target) {
    sf::Vector2f currentPos(_x, _y);
    sf::Vector2f direction = target - currentPos;

    if (std::abs(direction.x) > std::abs(direction.y)) {
        if (direction.x > 0) {
            _direction = 0; // Droite
        }
        else {
            _direction = 2; // Gauche
        }
    }
    else {
        if (direction.y > 0) {
            _direction = 1; // Bas
        }
        else {
            _direction = 3; // Haut
        }
    }
}

void Vehicule::updateSpriteRotation() {
    switch (_direction) {
    case 0: // Droite
        _angle = 0;
        break;
    case 1: // Bas
        _angle = 90;
        break;
    case 2: // Gauche
        _angle = 180;
        break;
    case 3: // Haut
        _angle = 270;
        break;
    default:
        _angle = 0;
        break;
    }
    _Sprite.setRotation(_angle);
}