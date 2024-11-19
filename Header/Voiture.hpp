#ifndef VOITURE_HPP
#define VOITURE_HPP

#include <iostream>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Voiture {
private:
    int id_; // Identifiant unique pour chaque voiture
    float position_x_; // Position de la voiture sur l'axe x
    float position_y_; // Position de la voiture sur l'axe y
    float speed_; // Vitesse de la voiture
    bool stopped_; // État de la voiture 

public:
    Voiture(int id, float x, float y, float speed);

    void move();

    void checkTrafficLight(Traffic_light& traffic_light);

    float getPositionX() const;

    float getPositionY() const;

    void runCar(Voiture& voiture, Traffic_light& traffic_light, std::stop_token stop_token);
};

#endif // VOITURE_HPP