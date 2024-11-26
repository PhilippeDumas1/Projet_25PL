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
    float d1 = 400, d2 = 350, d3 = 300, size = 1000, radius = 10;

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
    std::stop_source stopping;
    std::jthread thread_traffic_light(run_traffic_light,std::ref(traffic_light_master),std::ref(traffic_light_slave),stopping.get_token());

    std::jthread write_traffic_light(print_traffic_light, std::ref(traffic_light_master), std::ref(traffic_light_slave), stopping.get_token());

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

        //Update traffic lights
        sf::CircleShape circle1A(radius);
        circle1A.setFillColor(get_SFML_color(traffic_light_master));
        circle1A.setOrigin(circle1A.getRadius() / 2, circle1A.getRadius() / 2);
        circle1A.setPosition(d2, d3 + (d1 / 2));

        sf::CircleShape circle1B(radius);
        circle1B.setFillColor(get_SFML_color(traffic_light_master));
        circle1B.setOrigin(circle1B.getRadius() / 2, circle1B.getRadius() / 2);
        circle1B.setPosition(d2, d3 + d1);


        sf::CircleShape circle2A(radius);
        circle2A.setFillColor(get_SFML_color(traffic_light_slave));
        circle2A.setOrigin(circle2A.getRadius() / 2, circle2A.getRadius() / 2);
        circle2A.setPosition(d2 + (d3 / 2), d3 + d1);

        sf::CircleShape circle2B(radius);
        circle2B.setFillColor(get_SFML_color(traffic_light_slave));
        circle2B.setOrigin(circle2B.getRadius() / 2, circle2B.getRadius() / 2);
        circle2B.setPosition(d2 + d3, d3 + d1);


        sf::CircleShape circle1C(radius);
        circle1C.setFillColor(get_SFML_color(traffic_light_master));
        circle1C.setOrigin(circle1C.getRadius() / 2, circle1C.getRadius() / 2);
        circle1C.setPosition(d2 + d3, d3 + (d1 / 2));

        sf::CircleShape circle2C(radius);
        circle2C.setFillColor(get_SFML_color(traffic_light_master));
        circle2C.setOrigin(circle2C.getRadius() / 2, circle2C.getRadius() / 2);
        circle2C.setPosition(d2 + d3, d3);

        sf::CircleShape circle1D(radius);
        circle1D.setFillColor(get_SFML_color(traffic_light_slave));
        circle1D.setOrigin(circle1D.getRadius() / 2, circle1D.getRadius() / 2);
        circle1D.setPosition(d2, d3);

        sf::CircleShape circle2D(radius);
        circle2D.setFillColor(get_SFML_color(traffic_light_slave));
        circle2D.setOrigin(circle2D.getRadius() / 2, circle2D.getRadius() / 2);
        circle2D.setPosition(d2 + (d3 / 2), d3);

        window.draw(circle1A);
        window.draw(circle1B);

        window.draw(circle2A);
        window.draw(circle2B);

        window.draw(circle1C);
        window.draw(circle2C);

        window.draw(circle1D);
        window.draw(circle2D);

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