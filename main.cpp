#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <mutex>
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
    newCar.setDirections({ 1 }); // Assigner la direction spécifique
    vehicules.push_back(newCar);
    Vehicule newCar1(DH, 0, 1, carTexture);
    newCar1.setDirections({ 3 }); // Assigner la direction spécifique
    vehicules.push_back(newCar1);
    Vehicule newCar2(DD, 0, 1, carTexture);
    newCar2.setDirections({ 7 }); // Assigner la direction spécifique
    vehicules.push_back(newCar2);


    //VehicleGenerator vehicleGenerator(carTexture, carPaths);
    /*
    for (auto& vehicule : vehicules) {
        sf::Sprite sprite = vehicule.getSprite();
        std::cout << "Position de la voiture: (" << sprite.getPosition().x << ", " << sprite.getPosition().y << ")" << std::endl;
        std::cout << "Taille de la voiture: (" << sprite.getGlobalBounds().width << ", " << sprite.getGlobalBounds().height << ")" << std::endl;
    }
     */
   
    std::mutex traffic_light_mutex;

    Traffic_light traffic_light_master{ Traffic_color::green, sf::Vector2f(350, 575) };
    Traffic_light traffic_light_master1{ Traffic_color::green, sf::Vector2f(350, 625) };
    Traffic_light traffic_light_slave{ Traffic_color::red, sf::Vector2f(475, 650) };
    Traffic_light traffic_light_slave1{ Traffic_color::red, sf::Vector2f(525, 650) };
    Traffic_light traffic_light_master2{ Traffic_color::green, sf::Vector2f(550, 525) };
    Traffic_light traffic_light_master3{ Traffic_color::green, sf::Vector2f(550, 475) };
    Traffic_light traffic_light_slave2{ Traffic_color::red, sf::Vector2f(425, 450) };
    Traffic_light traffic_light_slave3{ Traffic_color::red, sf::Vector2f(375, 450) };
    Traffic_light traffic_light_slave4{ Traffic_color::red, sf::Vector2f(425, 250) };
    Traffic_light traffic_light_slave5{ Traffic_color::red, sf::Vector2f(375, 250) };

    // Ajout des feux au vecteur FeuTab
    std::vector<Traffic_light*> FeuTab = {
        &traffic_light_master,
        &traffic_light_master1,
        &traffic_light_slave,
        &traffic_light_slave1,
        &traffic_light_master2,
        &traffic_light_master3,
        &traffic_light_slave2,
        &traffic_light_slave3,
        &traffic_light_slave4,
        &traffic_light_slave5
    };

    std::vector<std::vector<Traffic_light*>> phases;
    phases.push_back({ FeuTab[2], FeuTab[3], FeuTab[6], FeuTab[7], FeuTab[8], FeuTab[9] });// Phase 1 : Feux verticaux (nord-sud)
    phases.push_back({ FeuTab[0], FeuTab[1], FeuTab[4], FeuTab[5] });// Phase 2 : Feux horizontaux (est-ouest)

    std::stop_source stopping;
    std::jthread traffic_light_thread(run_traffic_light_multiple, std::ref(FeuTab), std::ref(phases), std::ref(traffic_light_mutex), stopping.get_token());

    // Log the positions of the traffic lights
    for (const auto& light : FeuTab) {
        std::cout << "Traffic light position: (" << light->get_position().x << ", " << light->get_position().y << ")" << std::endl;
    }

    // Fenêtre SFML
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE_HORIZ, WINDOW_SIZE_VERTI), "Traffic Simulation");

    std::jthread write_traffic_light(print_traffic_light, std::ref(traffic_light_master), std::ref(traffic_light_slave), stopping.get_token());

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
            it->move(vehicules, FeuTab, traffic_light_mutex, deltaTime);
            
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

		window.draw(MD, 2, sf::Lines);
		window.draw(CHD1, 2, sf::Lines);
		window.draw(CHD2, 2, sf::Lines);

		window.draw(MH, 2, sf::Lines);
		window.draw(CHG1, 2, sf::Lines);
        window.draw(CHG2, 2, sf::Lines);
       
        for (const auto& light : FeuTab) {
            sf::CircleShape lightShape(radius);
            lightShape.setFillColor(get_SFML_color(*light));
            lightShape.setOrigin(radius / 2, radius / 2);
            lightShape.setPosition(light->get_position());
            window.draw(lightShape);
        }

        for (auto& vehicule : vehicules) {
            window.draw(vehicule.getSprite());
        }

        for (auto& vehicule : vehicules) {
            vehicule.move(vehicules, FeuTab, traffic_light_mutex, deltaTime);
        }
       
        window.display();
    }

    stopping.request_stop();
    traffic_light_thread.join();

    return 0;
}