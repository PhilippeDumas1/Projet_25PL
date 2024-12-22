#ifndef VEHICULE_HPP
#define VEHICULE_HPP

#include <SFML/Graphics.hpp>
#include "Feucircu.hpp" 
#include "Movable.hpp"
#include <vector>
#include <map>
#include <mutex>

struct Path {
    std::vector<sf::Vector2f> points;
};

enum VehicleState {
    MOVING,
    SLOWING_DOWN,
    STOPPED,
    SPEEDING_UP
};

class Vehicule : public Movable {
public:
    Vehicule(int spawn, int direction, int type, sf::Texture& skin);
    ~Vehicule();

    int getVehiculeType();

    bool CanTurnLeft(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab);
    bool CanTurnRight(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab);
    bool CanGoForward(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab, std::mutex& traffic_light_mutex);
    bool isOutOfBounds(int windowWidth, int windowHeight)const;

    sf::FloatRect getExpandedBounds(float extraLength);

    void drawBoundingBox(sf::RenderWindow& window);
    void drawDetectionSquare(sf::RenderWindow& window, std::vector<Vehicule>& Vehicules);

	void move(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab, std::mutex& traffic_light_mutex, float deltaTime);
    void SpeedUp();
    void SpeedDown();
    void TurnLeft();
	void TurnRight();
    void setTexture(const sf::Texture& texture);
    void setDirections(const std::vector<int>& directions);
    void updateDirection(const sf::Vector2f& target);
    void updateSpriteRotation();

    bool operator==(const Vehicule& other) const; // D�claration de l'op�rateur ==

private:
    int _VehiculeType; // 1:Voiture 2:Bus 3:V�lo
    VehicleState _state;

    std::map<int, Path> _paths;
    std::vector<Path> _currentPath; // Chemin actuel � suivre
    std::vector<int> _directions; // S�quence de directions � suivre
    size_t _currentPathIndex; // Index du segment de chemin actuel
    size_t _currentDirectionIndex; // Index de la direction actuelle

    void determinePath(int direction);
};


class VehicleGenerator {
public:
    VehicleGenerator(sf::Texture& carTexture, std::vector<Path>& paths)
        : carTexture(carTexture), paths(paths) {
    }

    void generateVehicles(std::vector<Vehicule>& vehicules, float deltaTime) {
        spawnTimer += deltaTime;
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0.0f;
            int spawnPoint = rand() % 4; // Choisir un point de spawn al�atoire
            Vehicule newCar(spawnPoint, 0, 1, carTexture);
            newCar.setDirections({ 0, 1, 2, 3 }); // Exemple de directions � suivre
            vehicules.push_back(newCar);
        }
    }

private:
    sf::Texture& carTexture;
    std::vector<Path>& paths;
    float spawnTimer = 0.0f;
    const float spawnInterval = 5.0f; // Intervalle de temps entre les spawns (en secondes)
};

#endif 