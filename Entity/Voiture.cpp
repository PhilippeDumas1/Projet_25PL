#include "Voiture.hpp"

Voiture::Voiture(float x, float y, float speed)
    : speed_(speed)
{
    shape_.setSize(sf::Vector2f(50.0f, 25.0f));  // Taille de la voiture
    shape_.setFillColor(sf::Color::Blue);        // Couleur de la voiture
    shape_.setPosition(x, y);
}

// Déplace la voiture
void Voiture::move()
{
    position_.x += speed_;
    shape_.setPosition(position_);
}

// Affiche la voiture dans la fenêtre
void Voiture::render(sf::RenderWindow& window)
{
    window.draw(shape_);
}

// Définit la position de la voiture
void Voiture::setPosition(float x, float y)
{
    position_.x = x;
    position_.y = y;
    shape_.setPosition(position_);
}

// Récupère la position de la voiture
sf::Vector2f Voiture::getPosition() const
{
    return position_;
}

void Voiture::run_car(sf::CircleShape& car, Traffic_light& traffic_light, std::stop_token stop_token)
{
    float speed = 2.0f;
    while (!stop_token.stop_requested())
    {
        if (traffic_light.get_traffic_color() == Traffic_color::green)
        {
            car.move(speed, 0); // Move the car horizontally
        }
        std::this_thread::sleep_for(50ms); // Control the update rate
    }
}#include "Voiture.hpp"

// Constructeur
Voiture::Voiture(float x, float y, float speed)
    : speed_(speed)
{
    shape_.setSize(sf::Vector2f(50.0f, 25.0f));  // Taille de la voiture
    shape_.setFillColor(sf::Color::Blue);        // Couleur de la voiture
    shape_.setPosition(x, y);
}

// Déplace la voiture
void Voiture::move()
{
    position_.x += speed_;
    shape_.setPosition(position_);
}

// Affiche la voiture dans la fenêtre
void Voiture::render(sf::RenderWindow& window)
{
    window.draw(shape_);
}

// Définit la position de la voiture
void Voiture::setPosition(float x, float y)
{
    position_.x = x;
    position_.y = y;
    shape_.setPosition(position_);
}

// Récupère la position de la voiture
sf::Vector2f Voiture::getPosition() const
{
    return position_;
}

void Voiture::run_car(sf::CircleShape& car, Traffic_light& traffic_light, std::stop_token stop_token)
{
    float speed = 2.0f;
    while (!stop_token.stop_requested())
    {
        if (traffic_light.get_traffic_color() == Traffic_color::green)
        {
            car.move(speed, 0); // Move the car horizontally
        }
        std::this_thread::sleep_for(50ms); // Control the update rate
    }
}