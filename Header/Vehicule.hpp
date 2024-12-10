#ifndef VEHICULE_HPP
#define VEHICULE_HPP

#include <SFML/Graphics.hpp>
#include "Feucircu.hpp" // Assurez-vous que cette inclusion correspond à votre implémentation des feux de circulation
#include "Movable.hpp"

class Vehicule : public Movable {
public:
    Vehicule(int spawn, int direction, int type, sf::Texture& skin);

    int getVehiculeType();

    bool HasToTurnLeft();
    bool HasToTurnRight();

    bool CanGoForward(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab);

	//bool CanTurnLeft(std::vector<Vehicule>& Vehicules, std::vector<sf::CircleShape*>& FeuTab);
	//bool CanTurnRight(std::vector<Vehicule>& Vehicules, std::vector<sf::CircleShape*>& FeuTab);

    sf::FloatRect getExpandedBounds(float extraLength);
    void drawBoundingBox(sf::RenderWindow& window);
    void drawDetectionSquare(sf::RenderWindow& window, std::vector<Vehicule>& Vehicules);

    void move(std::vector<Vehicule>& Vehicules, std::vector<Traffic_light*>& FeuTab);
    void SpeedUp();
    void SpeedDown();

    void setTexture(const sf::Texture& texture);

private:
    int _VehiculeType; // 1:Voiture 2:Bus 3:Vélo
};

#endif 