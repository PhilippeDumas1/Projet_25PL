#ifndef VOITURE_HPP
#define VOITURE_HPP

class Voiture
{
private:
    sf::RectangleShape shape_;
    float speed_;
    sf::Vector2f position_;

public:
    Voiture(float x, float y, float speed = 5.0f)
        : speed_(speed)
    {
        shape_.setSize(sf::Vector2f(50.0f, 25.0f));  // Taille de la voiture
        shape_.setFillColor(sf::Color::Blue);        // Couleur de la voiture
        shape_.setPosition(x, y);
    }

    void move()
    {
        position_.x += speed_;
        shape_.setPosition(position_);
    }

    void render(sf::RenderWindow& window)
    {
        window.draw(shape_);
    }

    void setPosition(float x, float y)
    {
        position_.x = x;
        position_.y = y;
        shape_.setPosition(position_);
    }

    void run_car(sf::CircleShape& car, Traffic_light& traffic_light, std::stop_token stop_token)
    {
        float speed = 2.0f;
        while (!stop_token.stop_requested())
        {
            if (traffic_light.get_traffic_color() == Traffic_color::green)
            {
                car.move(speed, 0); // Move the car horizontally
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Control the update rate
        }
    }
};

#endif VOITURE_HPP