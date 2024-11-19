#include "Voiture.hpp"
#include <iostream> // std::cout
#include <thread>   // std::thread, std::this_thread::yield
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include "Feucircu.cpp" 

class Voiture {
private:
    int id_; // Identifiant unique pour chaque voiture
    float position_x_; // Position de la voiture sur l'axe x
    float position_y_; // Position de la voiture sur l'axe y
    float speed_; // Vitesse de la voiture
    bool stopped_; // État de la voiture 

public:
    Voiture(int id, float x, float y, float speed)
        : id_{ id }, position_x_{ x }, position_y_{ y }, speed_{ speed }, stopped_{ false } {}

    void move() {
        if (!stopped_) {
            position_x_ += speed_;
            std::cout << "Voiture " << id_ << " avance à la position ("
                << position_x_ << ", " << position_y_ << ").\n";
        }
    }

    void checkTrafficLight(Traffic_light& traffic_light) {
        if (traffic_light.get_traffic_color() == Traffic_color::red) {
            stopped_ = true;
            std::cout << "Voiture " << id_ << " s'arrête au feu rouge.\n";
        }
        else {
            stopped_ = false;
            std::cout << "Voiture " << id_ << " traverse le carrefour.\n";
        }
    }

    float getPositionX() const {
        return position_x_;
    }
    float getPositionY() const {
        return position_y_;
    }

    void runCar(Voiture& voiture, Traffic_light& traffic_light, std::stop_token stop_token) {
        while (!stop_token.stop_requested()) {
            voiture.checkTrafficLight(traffic_light);
            voiture.move();
            std::this_thread::sleep_for(1s);
        }
    }
};
