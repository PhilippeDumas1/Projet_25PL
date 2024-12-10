#ifndef TRAFFIC_LIGHT_HPP
#define TRAFFIC_LIGHT_HPP

#include <iostream>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

static const sf::Color Orange(255, 165, 0);

const auto time_transit = 3;
const auto time_waiting = 8;

enum class Traffic_color
{
    green = 0,
    orange = 1,
    red = 2
};

class Traffic_light
{
private:
    Traffic_color traffic_color_;
    sf::Vector2f position_;

public:
    explicit Traffic_light(const Traffic_color& traffic_color, sf::Vector2f position);

    void operator++();

    void set_traffic_color(const Traffic_color& traffic_color);

    const Traffic_color& get_traffic_color() const;

    const sf::Vector2f& get_position() const;

    sf::FloatRect getExpandedBounds(float extraLength);

    void drawBoundingBox(sf::RenderWindow& window);

    sf::FloatRect getGlobalBounds() const;
};

const sf::Color& get_SFML_color(const Traffic_light& traffic_light);

std::ostream& operator<<(std::ostream& os, const Traffic_light& traffic_light);

void run_traffic_light(Traffic_light& traffic_light_master, Traffic_light& traffic_light_slave, std::stop_token stop_token);

void print_traffic_light(Traffic_light& traffic_light_master, Traffic_light& traffic_light_slave, std::stop_token stop_token);

#endif // TRAFFIC_LIGHT_HPP