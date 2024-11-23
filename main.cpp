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
    Traffic_light traffic_light_slave{ Traffic_color::green };
    std::jthread thread_traffic_light(run_traffic_light, std::ref(traffic_light_master), std::ref(traffic_light_slave), stopping.get_token());
    
    std::jthread write_traffic_light(print_traffic_light, std::ref(traffic_light_master), std::ref(traffic_light_slave), stopping.get_token());
    
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Traffic Simulation");
    
    float d1 = 400, d2 = 350, d3 = 300, size = 1000, radius = 10;
    sf::Vertex line1[] = { sf::Vertex(sf::Vector2f(0, d3)) , sf::Vertex(sf::Vector2f(size, d3)) };
    sf::Vertex line2[] = { sf::Vertex(sf::Vector2f(0, d3 + d1)), sf::Vertex(sf::Vector2f(size, d3 + d1)) };
    sf::Vertex line3[] = { sf::Vertex(sf::Vector2f(d2, 0)), sf::Vertex(sf::Vector2f(d2, size)) };
    sf::Vertex line4[] = { sf::Vertex(sf::Vector2f(d3+d2, 0)), sf::Vertex(sf::Vector2f(d3+d2, size)) };

	sf::Vertex line5[] = { sf::Vertex(sf::Vector2f(0, d3 + (d1 / 2))), sf::Vertex(sf::Vector2f(size, d3 + (d1 / 2))) };
    sf::Vertex line6[] = { sf::Vertex(sf::Vector2f(d2 + (d3/2), 0)), sf::Vertex(sf::Vector2f(d2 + (d3/2), size))};

    sf::Vertex lineV1[] = { sf::Vertex(sf::Vector2f(0, d3 + (d1 / 2) + (d1/4)), sf::Color::Magenta), sf::Vertex(sf::Vector2f(size, d3 + (d1 / 2) + (d1/4)), sf::Color::Magenta) };


    sf::CircleShape car(15);
    car.setFillColor(sf::Color::Blue);
    car.setPosition(0, d3 + (d1 / 2) + (d1 / 4));

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
		window.draw(line5, 2, sf::Lines);
		window.draw(line6, 2, sf::Lines);
		window.draw(lineV1, 2, sf::Lines);
        
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
            circle2A.setPosition(d2 + (d3/2), d3 + d1);

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

            window.draw(car);

            window.display();
        }

        return 0;


}