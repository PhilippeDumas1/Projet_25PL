#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Feucircu.hpp"
#include "Vehicule.hpp"
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

const int WINDOW_SIZE_HORIZ = 1000; // ou toute autre valeur appropriée
const int WINDOW_SIZE_VERTI = 1000;

float radius = 5;

enum SpawnPoint {
	DH, // Début haut
	DG, // Début gauche
	DD, // Début droite
	DB, // Début bas
};

std::vector<Path> carPaths = {
    { {sf::Vector2f(0, 575), sf::Vector2f(1000, 575)} },
    { {sf::Vector2f(0, 575), sf::Vector2f(425, 575), sf::Vector2f(425, 1000)} },
    { {sf::Vector2f(1000, 450), sf::Vector2f(0, 450)} },
    { {sf::Vector2f(450, 1000), sf::Vector2f(450, 0)} }
};

class VehicleGenerator {
public:
    VehicleGenerator(sf::Texture& carTexture, std::vector<Path>& paths)
        : carTexture(carTexture), paths(paths) {
    }

    void generateVehicles(std::vector<Vehicule>& vehicules, float deltaTime) {
        spawnTimer += deltaTime;
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0.0f;
            int spawnPoint = rand() % 4; // Choisir un point de spawn aléatoire
            Vehicule newCar(spawnPoint, 0, 1, carTexture);
            newCar.setDirections({ 0, 1, 2, 3 }); // Exemple de directions à suivre
            vehicules.push_back(newCar);
        }
    }

private:
    sf::Texture& carTexture;
    std::vector<Path>& paths;
    float spawnTimer = 0.0f;
    const float spawnInterval = 5.0f; // Intervalle de temps entre les spawns (en secondes)
};


int main() {
	// Initialisation des textures
	sf::Texture carTexture;
    if (!carTexture.loadFromFile("C:/Program Files/SFML/img/car.png")) {
        std::cerr << "Erreur: Impossible de charger la texture de la voiture." << std::endl;
        return -1; // Ou toute autre action appropriée en cas d'erreur
    }
    else{
		std::cout << "Texture de la voiture chargée avec succès." << std::endl;
    }

	// Initialisation des véhicules
    std::vector<Vehicule> vehicules;
    Vehicule newCar(DG, 0, 1, carTexture);
    newCar.setDirections({ 2 }); // Assigner la direction spécifique
    vehicules.push_back(newCar);

    //VehicleGenerator vehicleGenerator(carTexture, carPaths);
    /*
    for (auto& vehicule : vehicules) {
        sf::Sprite sprite = vehicule.getSprite();
        std::cout << "Position de la voiture: (" << sprite.getPosition().x << ", " << sprite.getPosition().y << ")" << std::endl;
        std::cout << "Taille de la voiture: (" << sprite.getGlobalBounds().width << ", " << sprite.getGlobalBounds().height << ")" << std::endl;
    }
    */

    // Initialisation des feux
    Traffic_light traffic_light_master{ Traffic_color::red, sf::Vector2f(350, 575) };
    Traffic_light traffic_light_slave{ Traffic_color::green, sf::Vector2f(475, 650) };

    std::vector<Traffic_light*> traffic_lights;
    traffic_lights.push_back(&traffic_light_master);
    traffic_lights.push_back(&traffic_light_slave);

    traffic_lights.push_back(new Traffic_light(Traffic_color::red, sf::Vector2f(350, 575)));
    traffic_lights.push_back(new Traffic_light(Traffic_color::red, sf::Vector2f(350, 625)));
    traffic_lights.push_back(new Traffic_light(Traffic_color::green, sf::Vector2f(475, 650)));
    traffic_lights.push_back(new Traffic_light(Traffic_color::green, sf::Vector2f(525, 650)));
    traffic_lights.push_back(new Traffic_light(Traffic_color::red, sf::Vector2f(550, 525)));
    traffic_lights.push_back(new Traffic_light(Traffic_color::red, sf::Vector2f(550, 475)));
    traffic_lights.push_back(new Traffic_light(Traffic_color::green, sf::Vector2f(425, 450)));
    traffic_lights.push_back(new Traffic_light(Traffic_color::green, sf::Vector2f(375, 450)));

    // Fenêtre SFML
    std::stop_source stopping;

    std::jthread thread_traffic_light(run_traffic_light,std::ref(traffic_light_master),std::ref(traffic_light_slave),stopping.get_token());
    std::jthread write_traffic_light(print_traffic_light, std::ref(traffic_light_master), std::ref(traffic_light_slave), stopping.get_token());

    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE_HORIZ, WINDOW_SIZE_VERTI), "Traffic Simulation");

    sf::Vertex MG[] = { sf::Vertex(sf::Vector2f(0, 550)), sf::Vertex(sf::Vector2f(350, 550)) };
	sf::Vertex CBG1[] = { sf::Vertex(sf::Vector2f(0, 650)), sf::Vertex(sf::Vector2f(350, 650)) };
    sf::Vertex CBG2[] = { sf::Vertex(sf::Vector2f(350, 650)), sf::Vertex(sf::Vector2f(350, 1000)) };

    sf::Vertex DVG[] = { sf::Vertex(sf::Vector2f(0, 575), sf::Color::Magenta), sf::Vertex(sf::Vector2f(425, 575), sf::Color::Magenta) };
    sf::Vertex VGC1[] = { sf::Vertex(sf::Vector2f(425, 575), sf::Color::Magenta), sf::Vertex(sf::Vector2f(1000, 575), sf::Color::Magenta) };
    sf::Vertex VGC2[] = { sf::Vertex(sf::Vector2f(425, 575), sf::Color::Magenta), sf::Vertex(sf::Vector2f(425, 1000), sf::Color::Magenta) };
    sf::Vertex VGC3[] = { sf::Vertex(sf::Vector2f(475, 575), sf::Color::Magenta), sf::Vertex(sf::Vector2f(475, 0), sf::Color::Magenta) };

    sf::Vertex DBG[] = { sf::Vertex(sf::Vector2f(0, 625), sf::Color::Cyan), sf::Vertex(sf::Vector2f(375, 625), sf::Color::Cyan) };

	sf::Vertex MB[] = { sf::Vertex(sf::Vector2f(450, 650)), sf::Vertex(sf::Vector2f(450, 1000)) };
    sf::Vertex CBD1[] = { sf::Vertex(sf::Vector2f(550, 650)), sf::Vertex(sf::Vector2f(550, 1000)) };
    sf::Vertex CBD2[] = { sf::Vertex(sf::Vector2f(550, 650)), sf::Vertex(sf::Vector2f(1000, 650)) };

	sf::Vertex DVB[] = { sf::Vertex(sf::Vector2f(450, 575)), sf::Vertex(sf::Vector2f(1000, 575)) };


    sf::Vertex MD[] = { sf::Vertex(sf::Vector2f(550, 550)), sf::Vertex(sf::Vector2f(1000, 550)) };
    sf::Vertex CHD1[] = { sf::Vertex(sf::Vector2f(550, 450)), sf::Vertex(sf::Vector2f(1000, 450)) };
	sf::Vertex CHD2[] = { sf::Vertex(sf::Vector2f(550, 0)), sf::Vertex(sf::Vector2f(550, 450)) };

    sf::Vertex MH[] = { sf::Vertex(sf::Vector2f(450, 0)), sf::Vertex(sf::Vector2f(450, 450)) };
    sf::Vertex CHG1[] = { sf::Vertex(sf::Vector2f(350, 0)), sf::Vertex(sf::Vector2f(350, 450)) };
	sf::Vertex CHG2[] = { sf::Vertex(sf::Vector2f(0, 450)), sf::Vertex(sf::Vector2f(350, 450)) };

    bool running = true;

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
                window.close();
            }
        }
        
        float deltaTime = clock.restart().asSeconds();

        //vehicleGenerator.generateVehicles(vehicules, deltaTime);

        for (auto it = vehicules.begin(); it != vehicules.end();) {
            it->move(vehicules, traffic_lights, deltaTime);
            std::cout << "Position de la voiture après move: (" << it->getX() << ", " << it->getY() << ")" << std::endl;

            // Check if the vehicle is out of bounds
            if (it->isOutOfBounds(WINDOW_SIZE_HORIZ, WINDOW_SIZE_VERTI)) {
                it = vehicules.erase(it); // Remove the vehicle from the vector
            }
            else {
                ++it;
            }
        }

        // Dessiner la fenêtre
        window.clear(sf::Color::Black);

        window.draw(MG, 2, sf::Lines);
		window.draw(CBG1, 2, sf::Lines);
		window.draw(CBG2, 2, sf::Lines);

        window.draw(DVG, 2, sf::Lines);
		window.draw(VGC1, 2, sf::Lines);
		window.draw(VGC2, 2, sf::Lines);
		window.draw(VGC3, 2, sf::Lines);

        window.draw(DBG, 2, sf::Lines);

		window.draw(MB, 2, sf::Lines);
		window.draw(CBD1, 2, sf::Lines);
		window.draw(CBD2, 2, sf::Lines);

		window.draw(DVB, 2, sf::Lines);

		window.draw(MD, 2, sf::Lines);
		window.draw(CHD1, 2, sf::Lines);
		window.draw(CHD2, 2, sf::Lines);

		window.draw(MH, 2, sf::Lines);
		window.draw(CHG1, 2, sf::Lines);
        window.draw(CHG2, 2, sf::Lines);
        
        //Update traffic lights
        sf::CircleShape circle1A(radius);
        circle1A.setFillColor(get_SFML_color(traffic_light_master));
        circle1A.setOrigin(circle1A.getRadius() / 2, circle1A.getRadius() / 2);
        circle1A.setPosition(350, 575);

        sf::CircleShape circle1B(radius);
        circle1B.setFillColor(get_SFML_color(traffic_light_master));
        circle1B.setOrigin(circle1B.getRadius() / 2, circle1B.getRadius() / 2);
        circle1B.setPosition(350, 625);

        sf::CircleShape circle2A(radius);
        circle2A.setFillColor(get_SFML_color(traffic_light_slave));
        circle2A.setOrigin(circle2A.getRadius() / 2, circle2A.getRadius() / 2);
        circle2A.setPosition(475,650);

        sf::CircleShape circle2B(radius);
        circle2B.setFillColor(get_SFML_color(traffic_light_slave));
        circle2B.setOrigin(circle2B.getRadius() / 2, circle2B.getRadius() / 2);
        circle2B.setPosition(525, 650);

        sf::CircleShape circle1C(radius);
        circle1C.setFillColor(get_SFML_color(traffic_light_master));
        circle1C.setOrigin(circle1C.getRadius() / 2, circle1C.getRadius() / 2);
        circle1C.setPosition(550, 525);

        sf::CircleShape circle2C(radius);
        circle2C.setFillColor(get_SFML_color(traffic_light_master));
        circle2C.setOrigin(circle2C.getRadius() / 2, circle2C.getRadius() / 2);
        circle2C.setPosition(550, 475);

        sf::CircleShape circle1D(radius);
        circle1D.setFillColor(get_SFML_color(traffic_light_slave));
        circle1D.setOrigin(circle1D.getRadius() / 2, circle1D.getRadius() / 2);
        circle1D.setPosition(425,450);

        sf::CircleShape circle2D(radius);
        circle2D.setFillColor(get_SFML_color(traffic_light_slave));
        circle2D.setOrigin(circle2D.getRadius() / 2, circle2D.getRadius() / 2);
        circle2D.setPosition(375,450);

        window.draw(circle1A);
        window.draw(circle1B);

        window.draw(circle2A);
        window.draw(circle2B);

        window.draw(circle1C);
        window.draw(circle2C);

        window.draw(circle1D);
        window.draw(circle2D);
       

        for (auto& vehicule : vehicules) {
            window.draw(vehicule.getSprite());
        }
       
        window.display();
    }

    return 0;
};