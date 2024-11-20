#include <iostream>
#include <vector>
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Feucircu.hpp"
#include "Voiture.hpp"

void run_car(sf::CircleShape & car, Traffic_light & traffic_light, std::stop_token stop_token){
    float speed = 2.0f;
    while (!stop_token.stop_requested()){
        if (traffic_light.get_traffic_color() == Traffic_color::green)
        {
            if (traffic_light.get_traffic_color() == Traffic_color::orange) {
				car.move(speed, 5);
            }
            car.move(speed, 0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main(){
    std::stop_source stopping;
    Traffic_light traffic_light_master{ Traffic_color::red };
    Traffic_light traffic_light_slave{ Traffic_color::red };
    std::jthread thread_traffic_light(run_traffic_light, std::ref(traffic_light_master), std::ref(traffic_light_slave), stopping.get_token());
    
    std::jthread write_traffic_light(print_traffic_light, std::ref(traffic_light_master), std::ref(traffic_light_slave), stopping.get_token());
    
    sf::RenderWindow window(sf::VideoMode(800, 800), "Traffic Simulation");
    
    float l1 = 350, l2 = 430, size = 800, radius = 10;
    sf::Vertex line1[] = { sf::Vertex(sf::Vector2f(0, l1)), sf::Vertex(sf::Vector2f(size, l1)) };
    sf::Vertex line2[] = { sf::Vertex(sf::Vector2f(0, l2)), sf::Vertex(sf::Vector2f(size, l2)) };
    sf::Vertex line3[] = { sf::Vertex(sf::Vector2f(l1, 0)), sf::Vertex(sf::Vector2f(l1, size)) };
    sf::Vertex line4[] = { sf::Vertex(sf::Vector2f(l2, 0)), sf::Vertex(sf::Vector2f(l2, size)) };

    sf::CircleShape car(15);
    car.setFillColor(sf::Color::Blue);
    car.setPosition(0, l1 - 20);

    std::jthread thread_car(run_car, std::ref(car), std::ref(traffic_light_master), stopping.get_token());
    
    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                stopping.request_stop();
                window.close();
                return 0;
            }
        }
        window.clear(sf::Color::Black);
        

        window.draw(line1, 2, sf::Lines);
        window.draw(line2, 2, sf::Lines);
        window.draw(line3, 2, sf::Lines);
        window.draw(line4, 2, sf::Lines);
        
        //Update traffic lights
            sf::CircleShape circle1(radius);
            circle1.setFillColor(get_SFML_color(traffic_light_slave));
            circle1.setOrigin(circle1.getRadius() / 2, circle1.getRadius() / 2);
            circle1.setPosition(l2 + radius / 2, l2 + radius / 2);

            sf::CircleShape circle2(radius);
            circle2.setFillColor(get_SFML_color(traffic_light_master));
            circle2.setOrigin(circle2.getRadius() / 2, circle2.getRadius() / 2);
            circle2.setPosition(l2 + radius / 2, l1 - radius);

            sf::CircleShape circle3(radius);
            circle3.setFillColor(get_SFML_color(traffic_light_slave));
            circle3.setOrigin(circle3.getRadius() / 2, circle3.getRadius() / 2);
            circle3.setPosition(l1 - radius/2 , l2 + radius / 2);

			sf::CircleShape circle4(radius);
			circle4.setFillColor(get_SFML_color(traffic_light_master));
			circle4.setOrigin(circle4.getRadius() / 2, circle4.getRadius() / 2);
			circle4.setPosition(l1 - radius / 2, l1 - radius / 2);

            window.draw(circle1);
            window.draw(circle2);
			window.draw(circle3);
			window.draw(circle4);

            window.draw(car);

            window.display();
        }

        return 0;


}