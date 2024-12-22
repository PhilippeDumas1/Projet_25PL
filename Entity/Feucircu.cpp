#include "Feucircu.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

const sf::Color Orange(255, 165, 0);

std::vector<Traffic_light*> FeuTab;
std::mutex traffic_light_mutex;
std::vector<std::vector<Traffic_light*>> phases;

using namespace std::chrono_literals;

Traffic_light::Traffic_light(const Traffic_color& traffic_color, sf::Vector2f position) : traffic_color_(traffic_color), position_(position) {}

void Traffic_light::operator++() {
    switch (traffic_color_)
    {
    case Traffic_color::red:
        traffic_color_ = Traffic_color::green;
        break;
    case Traffic_color::green:
        traffic_color_ = Traffic_color::orange;
        break;
    case Traffic_color::orange:
        traffic_color_ = Traffic_color::red;
        break;
    }
}

void Traffic_light::set_traffic_color(const Traffic_color& traffic_color){
	traffic_color_ = traffic_color;
}

void Traffic_light::drawBoundingBox(sf::RenderWindow& window) {
    sf::FloatRect bounds = getExpandedBounds(50);
    sf::RectangleShape boundingBox;
    boundingBox.setPosition(bounds.left, bounds.top);
    boundingBox.setSize(sf::Vector2f(bounds.width, bounds.height));
    boundingBox.setFillColor(sf::Color::Transparent);
    boundingBox.setOutlineColor(sf::Color::Red);
    boundingBox.setOutlineThickness(1.0f);
    window.draw(boundingBox);
}

sf::FloatRect Traffic_light::getExpandedBounds(float extraLength) {
    sf::FloatRect bounds(position_.x - extraLength / 4, position_.y - extraLength / 4, extraLength / 2, extraLength / 2);
    return bounds;
}


sf::FloatRect Traffic_light::getGlobalBounds() const {
    return sf::FloatRect(position_.x - 10.0f, position_.y - 10.0f, 20.0f, 20.0f); // Example of a smaller size 20x20
}

const Traffic_color& Traffic_light::get_traffic_color() const{
	return traffic_color_;
}

const sf::Vector2f& Traffic_light::get_position() const {
    return position_;
}

const sf::Color& get_SFML_color(const Traffic_light& traffic_light) {
    switch (traffic_light.get_traffic_color()) {
    case Traffic_color::green:
        return sf::Color::Green;
    case Traffic_color::red:
        return sf::Color::Red;
    case Traffic_color::orange:
        return Orange;
    }
    return sf::Color::Black; // Default color if none match
}

std::ostream& operator<<(std::ostream& os, const Traffic_light& traffic_light){
    std::string ptr;
    switch (traffic_light.get_traffic_color())
    {
    case Traffic_color::red:
        ptr = "Red";
        break;
    case Traffic_color::green:
        ptr = "Green";
        break;
    case Traffic_color::orange:
        ptr = "Orange";
    }
    os << ptr;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Traffic_color& color) {
    switch (color) {
    case Traffic_color::green:
        os << "Green";
        break;
    case Traffic_color::orange:
        os << "Orange";
        break;
    case Traffic_color::red:
        os << "Red";
        break;
    default:
        os << "unknown";
        break;
    }
    return os;
}

void print_traffic_light(Traffic_light& traffic_light_master, Traffic_light& traffic_light_slave, std::stop_token stop_token){
    while (!stop_token.stop_requested())
    {
        std::this_thread::sleep_for(1s);
        std::cout << "Taffic light master : " << traffic_light_master << " Taffic light slave : " << traffic_light_slave << std::endl;
    }
}


void run_traffic_light_multiple(std::vector<Traffic_light*>& FeuTab, std::mutex& traffic_light_mutex, std::stop_token stop_token) {
    const int time_waiting = 10; // Temps vert
    const int time_transit = 2;  // Temps orange

    size_t currentIndex = 0;
    size_t numLights = FeuTab.size();

    if (FeuTab.size() < 4) {
        std::cerr << "Erreur : FeuTab doit contenir au moins 4 feux pour les phases." << std::endl;
        return;
    }

    while (!stop_token.stop_requested()) {
        {
            std::lock_guard<std::mutex> lock(traffic_light_mutex);
            for (auto& light : FeuTab) {
                light->set_traffic_color(Traffic_color::red);
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(time_waiting));

        // Passer les feux de la phase actuelle à l'orange
        {
            std::lock_guard<std::mutex> lock(traffic_light_mutex);
            if (currentIndex < phases.size()) {
                for (auto& light : phases[currentIndex]) {
                    ++(*light); // Passe de vert à orange
                }
            }
            else {
                std::cerr << "Erreur : currentIndex hors limites." << std::endl;
                return;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(time_transit));

        // Passer les feux de la phase actuelle au rouge et passer à la phase suivante
        {
            std::lock_guard<std::mutex> lock(traffic_light_mutex);
            if (currentIndex < phases.size()) {
                for (auto& light : phases[currentIndex]) {
                    ++(*light); // Passe d'orange à rouge
                }
                currentIndex = (currentIndex + 1) % phases.size();
            }
            else {
                std::cerr << "Erreur : currentIndex hors limites." << std::endl;
                return;
            }
        }
    }
}

/*
void run_traffic_light_multiple(std::vector<Traffic_light*>& FeuTab, std::mutex& traffic_light_mutex, std::stop_token stop_token) {
    const int time_waiting = 10; // Temps d'attente avant de changer de couleur (en secondes)
    const int time_transit = 2;  // Temps de transition pour le feu orange (en secondes)

    size_t currentIndex = 0;
    size_t numLights = FeuTab.size();

    while (!stop_token.stop_requested()) {
        // Mettre tous les feux au rouge
        {
            std::lock_guard<std::mutex> lock(traffic_light_mutex);
            for (auto& light : FeuTab) {
                light->set_traffic_color(Traffic_color::red);
            }
        }

        // Mettre le feu courant au vert
        {
            std::lock_guard<std::mutex> lock(traffic_light_mutex);
            FeuTab[currentIndex]->set_traffic_color(Traffic_color::green);
        }

        // Attendre pendant le temps de passage
        std::this_thread::sleep_for(std::chrono::seconds(time_waiting));

        // Passer le feu courant à l'orange
        {
            std::lock_guard<std::mutex> lock(traffic_light_mutex);
            ++(*FeuTab[currentIndex]); // Change la couleur du feu
        }

        // Attendre le temps de transition
        std::this_thread::sleep_for(std::chrono::seconds(time_transit));

        // Passer le feu courant au rouge et passer au suivant
        {
            std::lock_guard<std::mutex> lock(traffic_light_mutex);
            FeuTab[currentIndex]->set_traffic_color(Traffic_color::red);
            currentIndex = (currentIndex + 1) % numLights;
        }
    }
}
*/