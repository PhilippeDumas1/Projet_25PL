#include <iostream>
#include <vector>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "Feucircu.hpp"
#include "Voiture.hpp"

using namespace std;

bool are_cars_close(const sf::CircleShape& car1, const sf::CircleShape& car2, float threshold = 20.0f) {
    sf::Vector2f pos1 = car1.getPosition();
    sf::Vector2f pos2 = car2.getPosition();
    float distance = std::sqrt(std::pow(pos1.x - pos2.x, 2) + std::pow(pos1.y - pos2.y, 2));
    return distance < threshold;
}

void update_logic(vector<sf::CircleShape>& cars, vector<Traffic_light>& traffic_lights, bool& running) {
    float speed = 1.0f;

    while (running) {
        for (auto& car : cars) {
            sf::Vector2f car_position = car.getPosition();

            Traffic_light* closest_traffic_light = nullptr;
            float min_distance = std::numeric_limits<float>::max();

            for (auto& light : traffic_lights) {
                float distance = std::abs(light.get_position().x - car_position.x);
                if (distance < min_distance && car_position.x <= light.get_position().x) {
                    min_distance = distance;
                    closest_traffic_light = &light;
                }
            }

            if (closest_traffic_light) {
                if (closest_traffic_light->get_traffic_color() == Traffic_color::red &&
                    car_position.x + car.getRadius() > closest_traffic_light->get_position().x) {
                    continue;
                }
            }

            bool should_stop = false;
            for (auto& other_car : cars) {
                if (&car != &other_car && are_cars_close(car, other_car)) {
                    should_stop = true;
                    break;
                }
            }

            if (should_stop) {
                continue;
            }

            car.move(speed, 0);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    float d1 = 400, d2 = 350, d3 = 300, size = 1000;

    Traffic_light traffic_light_master{ Traffic_color::red, sf::Vector2f(d2, d3 + (d1 / 2)) };
    Traffic_light traffic_light_slave{ Traffic_color::green, sf::Vector2f(d2 + (d3 / 2), d3 + d1) };

    // Initialisation des feux
    vector<Traffic_light> traffic_lights;
    traffic_lights.emplace_back(Traffic_color::red, sf::Vector2f(d2, d3 + (d1 / 2)));
    traffic_lights.emplace_back(Traffic_color::red, sf::Vector2f(d2, d3 + d1));

    traffic_lights.emplace_back(Traffic_color::green, sf::Vector2f(d2 + (d3 / 2), d3 + d1));
    traffic_lights.emplace_back(Traffic_color::green, sf::Vector2f(d2 + d3, d3 + d1));

    traffic_lights.emplace_back(Traffic_color::red, sf::Vector2f(d2 + d3, d3 + (d1 / 2)));
    traffic_lights.emplace_back(Traffic_color::red, sf::Vector2f(d2 + d3, d3));

    traffic_lights.emplace_back(Traffic_color::green, sf::Vector2f(d2, d3));
    traffic_lights.emplace_back(Traffic_color::green, sf::Vector2f(d2 + (d3 / 2), d3));

    // Initialisation des voitures
    vector<sf::CircleShape> cars;
    sf::CircleShape car1(15), car2(15);
    car1.setFillColor(sf::Color::Blue);
    car1.setPosition(0, d3);
    car2.setFillColor(sf::Color::Red);
    car2.setPosition(50, d3);

    cars.push_back(car1);
    cars.push_back(car2);



    // Fenêtre SFML
    std::jthread thread_traffic_light(run_traffic_light, std::ref(traffic_light_master), std::ref(traffic_light_slave), stopping.get_token());

    //std::jthread write_traffic_light(print_traffic_light, std::ref(traffic_light_master), std::ref(traffic_light_slave), stopping.get_token());

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Traffic Simulation");


    sf::Vertex line1[] = { sf::Vertex(sf::Vector2f(0, d3)) , sf::Vertex(sf::Vector2f(size, d3)) };
    sf::Vertex line2[] = { sf::Vertex(sf::Vector2f(0, d3 + d1)), sf::Vertex(sf::Vector2f(size, d3 + d1)) };
    sf::Vertex line3[] = { sf::Vertex(sf::Vector2f(d2, 0)), sf::Vertex(sf::Vector2f(d2, size)) };
    sf::Vertex line4[] = { sf::Vertex(sf::Vector2f(d3 + d2, 0)), sf::Vertex(sf::Vector2f(d3 + d2, size)) };

    sf::Vertex line5[] = { sf::Vertex(sf::Vector2f(0, d3 + (d1 / 2))), sf::Vertex(sf::Vector2f(size, d3 + (d1 / 2))) };
    sf::Vertex line6[] = { sf::Vertex(sf::Vector2f(d2 + (d3 / 2), 0)), sf::Vertex(sf::Vector2f(d2 + (d3 / 2), size)) };

    sf::Vertex lineV1[] = { sf::Vertex(sf::Vector2f(0, d3 + (d1 / 2) + (d1 / 4)), sf::Color::Magenta), sf::Vertex(sf::Vector2f(size, d3 + (d1 / 2) + (d1 / 4)), sf::Color::Magenta) };

    sf::Vertex lineFGB[] = { sf::Vertex(sf::Vector2f(d2, d3 + (d1 / 2)), sf::Color::Yellow), sf::Vertex(sf::Vector2f(d2, d3 + d1), sf::Color::Yellow) };
    bool running = true;

    // Thread pour la logique
    std::thread logic_thread(update_logic, std::ref(cars), std::ref(traffic_lights), std::ref(running));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
                logic_thread.join();
                window.close();
                return 0;
            }
        }

        // Dessiner la fenêtre
        window.clear(sf::Color::Black);
        window.draw(line1, 2, sf::Lines);
        window.draw(line2, 2, sf::Lines);
        window.draw(line3, 2, sf::Lines);
        window.draw(line4, 2, sf::Lines);
        window.draw(line5, 2, sf::Lines);
        window.draw(line6, 2, sf::Lines);
        window.draw(lineV1, 2, sf::Lines);
        window.draw(lineFGB, 2, sf::Lines);

        // Dessiner les feux
        for (auto& light : traffic_lights) {
            sf::CircleShape light_circle(10);
            light_circle.setFillColor(get_SFML_color(light));
            light_circle.setPosition(light.get_position());
            window.draw(light_circle);
        }

        // Dessiner les voitures
        for (auto& car : cars) {
            window.draw(car);
        }

        window.display();
    }

    running = false;
    logic_thread.join();
    return 0;
}