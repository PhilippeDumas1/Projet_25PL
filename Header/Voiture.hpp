#ifndef VOITURE_HPP
#define VOITURE_HPP

#include <SFML/Graphics.hpp>
#include "Feucircu.hpp" // Assurez-vous que cette inclusion correspond à votre implémentation des feux de circulation

class Voiture
{
private:
    sf::RectangleShape shape_;
    float speed_;
    sf::Vector2f position_;
    bool has_passed_light_ = false; // Indique si la voiture a dépassé un feu

public:
    // Constructeur
    Voiture(float x, float y, float speed = 5.0f)
        : speed_(speed)
    {
        shape_.setSize(sf::Vector2f(50.0f, 25.0f));  // Taille de la voiture
        shape_.setFillColor(sf::Color::Blue);        // Couleur de la voiture
        position_ = sf::Vector2f(x, y);              // Initialiser la position
        shape_.setPosition(position_);
    }

    // Déplace la voiture
    void move(bool horizontal)
    {
        if (horizontal) {
            position_.x += speed_;
        }
        else {
            position_.y += speed_;
        }
        shape_.setPosition(position_);
    }

    // Affiche la voiture dans la fenêtre
    void render(sf::RenderWindow& window)
    {
        window.draw(shape_);
    }

    // Définit la position de la voiture
    void setPosition(float x, float y)
    {
        position_.x = x;
        position_.y = y;
        shape_.setPosition(position_);
    }

    // Récupère la position de la voiture
    sf::Vector2f getPosition() const
    {
        return position_;
    }

    // Vérifie si la voiture a dépassé un feu de circulation
    void check_traffic_light(const Traffic_light& traffic_light)
    {
        if (position_.x > traffic_light.get_position().x)
        {
            has_passed_light_ = true;
        }
    }

    // Logique pour faire rouler la voiture en fonction des feux
    void run_car(sf::CircleShape& car, Traffic_light& traffic_light, std::stop_token stop_token)
    {
        float speed = 2.0f;
        while (!stop_token.stop_requested())
        {
            if (!has_passed_light_ && car.getPosition().x < traffic_light.get_position().x)
            {
                if (traffic_light.get_traffic_color() == Traffic_color::green)
                {
                    car.move(speed, 0); // Déplacer la voiture horizontalement
                }
            }
            else
            {
                has_passed_light_ = true; // La voiture ne sera plus affectée par ce feu
                car.move(speed, 0);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Contrôle du taux de mise à jour
        }
    }
};

#endif 