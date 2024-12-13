#ifndef VEHICULE_HPP
#define VEHICULE_HPP

#include <SFML/Graphics.hpp>
#include "Feucircu.hpp" 
#include "Movable.hpp"
#include <vector>
#include <map>

struct Path {
    std::vector<sf::Vector2f> points;
};

class Vehicule : public Movable {
public:
    Vehicule(int spawn, int direction, int type, sf::Texture& skin);

    int getVehiculeType();

    bool CanTurnLeft(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab);
    bool CanTurnRight(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab);
    bool CanGoForward(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab);
    bool isOutOfBounds(int windowWidth, int windowHeight)const;

    sf::FloatRect getExpandedBounds(float extraLength);

    void drawBoundingBox(sf::RenderWindow& window);
    void drawDetectionSquare(sf::RenderWindow& window, std::vector<Vehicule>& Vehicules);
	void move(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab, float deltaTime);
    void SpeedUp();
    void SpeedDown();
    void TurnLeft();
	void TurnRight();
    void setTexture(const sf::Texture& texture);
    void setDirections(const std::vector<int>& directions);
    void updateDirection(const sf::Vector2f& target);
    void updateSpriteRotation();

private:
    int _VehiculeType; // 1:Voiture 2:Bus 3:Vélo

    std::map<int, Path> _paths;
    std::vector<Path> _currentPath; // Chemin actuel à suivre
    std::vector<int> _directions; // Séquence de directions à suivre
    size_t _currentPathIndex; // Index du segment de chemin actuel
    size_t _currentDirectionIndex; // Index de la direction actuelle

    void determinePath(int direction);
};

#endif 