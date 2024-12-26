    #include "Vehicule.hpp"
    #include <cmath>
    #include <iomanip>
    #include <chrono>
    #include <mutex>

    const int WINDOW_SIZE_HORIZ = 1000; // ou toute autre valeur appropriée
    const int WINDOW_SIZE_VERTI = 1000;
    const double M_PI = 3.14;
    const bool DEBUG_MODE = false;
    const float MAX_SPEED = 45.0F;
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

    Vehicule::Vehicule(int spawn, int direction, int type, sf::Texture& Skin)
        : _VehiculeType(type), _currentPathIndex(0), _currentDirectionIndex(0), _state(MOVING) {
        setTexture(Skin);

        _paths = {
            {0, {{sf::Vector2f(0, 575), sf::Vector2f(1050, 575)}}},//Chemin voiture Gauche
            {1, {{sf::Vector2f(0, 575), sf::Vector2f(425, 575), sf::Vector2f(425, 1050)}}},
            {2, {{sf::Vector2f(0, 575), sf::Vector2f(475, 575), sf::Vector2f(475, -50)}}},
            {3, {{sf::Vector2f(425, 0), sf::Vector2f(425, 1050)}}},//Chemin voitue Haut
            {4, {{sf::Vector2f(425, 0), sf::Vector2f(425, 475), sf::Vector2f(-50, 475)}}},
            {5, {{sf::Vector2f(0, 575), sf::Vector2f(475, 575), sf::Vector2f(475, -50)}}},
            {6, {{sf::Vector2f(1000, 525), sf::Vector2f(-50, 525)}}},//Chemin Voiture Droite
            {7, {{sf::Vector2f(1000, 525), sf::Vector2f(425, 525), sf::Vector2f(425, 1050)}}},
            {8, {{sf::Vector2f(1000, 450), sf::Vector2f(0, 450)}}},
            {9, {{sf::Vector2f(450, 1000), sf::Vector2f(450, 0)}}},
            {10, {{sf::Vector2f(450, 1000), sf::Vector2f(450, 0)}}},
            {11, {{sf::Vector2f(450, 1000), sf::Vector2f(450, 0)}}}
        };

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
            case 1: _x = 1000; _y = 525; break; // Voiture
            case 2: _x = 1000; _y = 500; break; // Bus
            case 3: _x = 1000; _y = 475; break; // Vélo
            default: _x = 0; _y = 0; break;
            }
            _angle = 180; // Déplacement vers la gauche
            break;
        case DB:
            switch (_VehiculeType) {
            case 1: _x = 500; _y = 1000; break; // Voiture
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
        _directionPos = sf::Vector2f(
            static_cast<float>(-WINDOW_SIZE_VERTI / 2 * sin(M_PI / 2 * _direction) * cos(M_PI / 2 * _direction)),                         
            static_cast<float>(WINDOW_SIZE_HORIZ / 2 * cos(M_PI / 2 * _direction) * sin(M_PI / 2 * _direction)));

        if (DEBUG_MODE) {
            std::cout << "[NEW]\tVehicule : x:" << _x << " y:" << _y
                << " angle:" << _angle << " spawn:" << spawn
                << " direction:" << _direction << std::endl;
        }

        _Sprite.setScale(sf::Vector2f(0.5, 0.5));
        setPos(_x, _y);
        setAngle(_angle);
        _Sprite.setPosition(_x, _y);
        _Sprite.setRotation(_angle);

        sf::FloatRect bounds = _Sprite.getLocalBounds();
        _Sprite.setOrigin(bounds.width / 2, bounds.height / 2);

        determinePath(_direction);
        updateSpriteRotation();
    }

    Vehicule::~Vehicule() {
	    if (DEBUG_MODE) {
		    std::cout << "[DEL]\tVehicule : x:" << _x << " y:" << _y
			    << " angle:" << _angle << " direction:" << _direction << std::endl;
	    }
    }

    //----------------------------------------------------------------

    int Vehicule::getVehiculeType() {
	    return _VehiculeType;
    }

    //----------------------------------------------------------------

    //Fonction utilisé nul part
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

    //Fonction utilisé nul part
    bool Vehicule::CanTurnRight(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab) {
	    return false;
    }

    //extern std::mutex traffic_light_mutex; // Assurez-vous que le mutex est accessible ici

    bool Vehicule::CanGoForward(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab, std::mutex& traffic_light_mutex) {
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

        {
            std::lock_guard<std::mutex> lock(traffic_light_mutex);

            for (auto& feu : FeuTab) {
                if (expandedBounds.intersects(feu->getGlobalBounds())) {
                    //std::cout << "Feu détecté: " << feu->get_traffic_color() << " à la position (" << feu->get_position().x << ", " << feu->get_position().y << ")" << std::endl;
                    if (feu->get_traffic_color() == Traffic_color::red) {
                        return false;
                    }else if (feu->get_traffic_color() == Traffic_color::orange) {
                        SpeedDown();
                        return true;
                    }else if (feu->get_traffic_color() == Traffic_color::green) {
                        return true;
                    }
                }
            }
        }
        return true; // Aucun obstacle détecté
    }

    bool Vehicule::isOutOfBounds(int windowWidth, int windowHeight) const {
        sf::FloatRect bounds = _Sprite.getGlobalBounds();
        return (bounds.left + bounds.width < 0 || bounds.left > windowWidth ||
            bounds.top + bounds.height < 0 || bounds.top > windowHeight);
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

    //Fonction utilisé nul part
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

    //Fonction utilisé nul part
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

    //----------------------------------------------------------------

    void Vehicule::move(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab, std::mutex& traffic_light_mutex, float deltaTime) {
        if (CanGoForward(Vehicules, FeuTab, traffic_light_mutex)) {
            if (_state == STOPPED || _state == SLOWING_DOWN) {
                _state = SPEEDING_UP;
            }

            if (_state == SPEEDING_UP) {
                SpeedUp();
                if (_speed >= MIN_SPEED) {
                    _state = MOVING;
                }
            }

            if (_currentDirectionIndex < _currentPath.size() && _currentPathIndex < _currentPath[_currentDirectionIndex].points.size()) {
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

				if (std::abs(_x - target.x) < 5.0f && std::abs(_y - target.y) < 5.0f) {//Vérifie si le véhicule est proche de la cible
                    _currentPathIndex++;  //le véhicule atteint la fin d'un segment de chemin, il passe au segment suivant

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
                std::cout << "x :" << _x << "y :" << _y << std::endl;
            }

            else {

                if (_currentDirectionIndex + 1 < _directions.size()) {
                    std::cout << "A" << std::endl;
                    _currentDirectionIndex++;
                    determinePath(_directions[_currentDirectionIndex]);
                }
                else {
                    if (isOutOfBounds(WINDOW_SIZE_HORIZ, WINDOW_SIZE_VERTI)) {
                        std::cout << "Véhicule OOB" << std::endl;
                        auto it = std::find(Vehicules.begin(), Vehicules.end(), *this);
                        if (it != Vehicules.end()) {
                            Vehicules.erase(it);
                            std::cout << "Véhicule supprimé" << std::endl;
                        }
                        return; // Sortir de la fonction pour éviter d'accéder à un objet supprimé
                    }
                }
            }
        }
        else {
            SpeedDown();
        }
    }


    //----------------------------------------------------------------

    void Vehicule::SpeedUp() {
        if (_speed == 0) {
            _speed = MIN_SPEED; // Reprendre à la vitesse minimale de base
        }

	    if (_speed < MAX_SPEED) {
		    _speed += 0.5f;
	    }
        else {
            _speed = MAX_SPEED;
        }
    }

    void Vehicule::SpeedDown() {
        if (_speed > 0) {
            _speed -= 0.5f;
            if (_speed <= 0) {
                _speed = 0;
                _state = STOPPED;
                std::cout << "Véhicule arrêté." << std::endl;
            }
            else {
                _state = SLOWING_DOWN;
            }
        }
        else {
            _speed = 0;
            _state = STOPPED;
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
        std::cout << "Determining path for direction: " << direction << std::endl;
        if (_paths.find(direction) != _paths.end()) {
            _currentPath.clear();
            _currentPath.push_back(_paths[direction]);
            _currentPathIndex = 0;
        }
        else {
            std::cerr << "Error: Invalid direction " << direction << std::endl;
        }
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

    //----------------------------------------------------------------

    bool Vehicule::operator==(const Vehicule& other) const {
        return this == &other;
	    //return _x == other._x && _y == other._y && _angle == other._angle && _direction == other._direction;
    }