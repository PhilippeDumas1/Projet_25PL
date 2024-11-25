#include "Feucircu.hpp"
#include <iostream>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std::chrono_literals;

Traffic_color operator++(Traffic_color& traffic_color)
{
    switch (traffic_color)
    {
    case Traffic_color::red:
        traffic_color = Traffic_color::green;
        break;

    case Traffic_color::green:
        std::this_thread::sleep_for(std::chrono::seconds(time_transit));
        traffic_color = Traffic_color::orange;
        std::this_thread::sleep_for(std::chrono::seconds(time_transit));
        traffic_color = Traffic_color::red;
    }
    return traffic_color;
}

Traffic_light::Traffic_light(const Traffic_color& traffic_color, sf::Vector2f position)
    : traffic_color_(traffic_color), position_(position) {}

void Traffic_light::operator++()
{
	switch (traffic_color_)
	{
	case Traffic_color::red:
		traffic_color_ = Traffic_color::green;
		break;
	case Traffic_color::green:
		std::this_thread::sleep_for(std::chrono::seconds(time_transit));
		traffic_color_ = Traffic_color::orange;
		std::this_thread::sleep_for(std::chrono::seconds(time_transit));
		traffic_color_ = Traffic_color::red;
	}
}

void Traffic_light::set_traffic_color(const Traffic_color& traffic_color)
{
	traffic_color_ = traffic_color;
}

const Traffic_color& Traffic_light::get_traffic_color() const
{
	return traffic_color_;
}

const sf::Vector2f& Traffic_light::get_position() const {
    return position_;
}

const sf::Color& get_SFML_color(const Traffic_light& traffic_light)
{
    switch (traffic_light.get_traffic_color())
    {
    case Traffic_color::green:
        return sf::Color::Green;
    case Traffic_color::red:
        return sf::Color::Red;
    }
    return Orange;
}

std::ostream& operator<<(std::ostream& os, const Traffic_light& traffic_light)
{
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

void run_traffic_light(Traffic_light& traffic_light_master, Traffic_light& traffic_light_slave, std::stop_token stop_token)
{
    traffic_light_master.set_traffic_color(Traffic_color::green);
    traffic_light_slave.set_traffic_color(Traffic_color::red);
    while (!stop_token.stop_requested())
    {

        std::this_thread::sleep_for(std::chrono::seconds(time_waiting));
        if (traffic_light_master.get_traffic_color() == Traffic_color::green)
        {
            ++traffic_light_master;
            ++traffic_light_slave;
        }
        else
        {
            ++traffic_light_slave;
            ++traffic_light_master;
        }
    }
}

void print_traffic_light(Traffic_light& traffic_light_master, Traffic_light& traffic_light_slave, std::stop_token stop_token)
{
    while (!stop_token.stop_requested())
    {
        std::this_thread::sleep_for(1s);
        std::cout << "Taffic light master : " << traffic_light_master << " Taffic light slave : " << traffic_light_slave << std::endl;
    }
}
