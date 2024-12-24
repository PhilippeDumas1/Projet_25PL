#ifndef TRAFFIC_LIGHT_HPP
#define TRAFFIC_LIGHT_HPP

#include <iostream>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <mutex>

extern const sf::Color Orange;

const auto time_transit = 3;
const auto time_waiting = 5;

enum class Traffic_color
{
    green = 0,
    orange = 1,
    red = 2
};

class Traffic_light{
public:
    explicit Traffic_light(const Traffic_color& traffic_color, sf::Vector2f position);

    void operator++();
    void set_traffic_color(const Traffic_color& traffic_color);
    
    void drawBoundingBox(sf::RenderWindow& window);
    sf::FloatRect getExpandedBounds(float extraLength);
    sf::FloatRect getGlobalBounds() const;
    
    const Traffic_color& get_traffic_color() const;
    const sf::Vector2f& get_position() const;

private:
    Traffic_color traffic_color_;
    sf::Vector2f position_;
};

const sf::Color& get_SFML_color(const Traffic_light& traffic_light);

std::ostream& operator<<(std::ostream& os, const Traffic_light& traffic_light);
std::ostream& operator<<(std::ostream& os, const Traffic_color& color);

void print_traffic_light(Traffic_light& traffic_light_master, Traffic_light& traffic_light_slave, std::stop_token stop_token);
void run_traffic_light_multiple(std::vector<Traffic_light*>& FeuTab, std::vector<std::vector<Traffic_light*>>& phases, std::mutex& traffic_light_mutex, std::stop_token stop_token);

#endif // TRAFFIC_LIGHT_HPP