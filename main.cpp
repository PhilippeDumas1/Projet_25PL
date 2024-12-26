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
#include <random>

using namespace std;
namespace fs = std::filesystem;

const int WINDOW_SIZE_HORIZ = 1000; // ou toute autre valeur appropri�e
const int WINDOW_SIZE_VERTI = 1000;

float radius = 5;

enum SpawnPoint {
	DH, // D�but haut
	DG, // D�but gauche
	DD, // D�but droite
	DB, // D�but bas
};


void generateRandomVehicles(std::vector<Vehicule>& vehicules, sf::Texture& carTexture, sf::Texture& busTexture, sf::Texture& veloTexture) {
    // G�n�rateur al�atoire pour les positions de spawn et directions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> spawnDist(0, 3);   // Pour DH, DG, DD, DB
    std::uniform_int_distribution<> directionDist(0, 7); // Directions possibles de 0 � 7
    std::uniform_int_distribution<> typeDist(1, 3);    // Exemple: type de v�hicule (0, 1, ou 2)

    // G�n�rer un spawn al�atoire
    int spawn = spawnDist(gen);
    int direction = directionDist(gen);  // Direction al�atoire
    int Ddirection = 0;
    int type = typeDist(gen);

    // D�finir Ddirection en fonction du spawn
    if (type == 1) {
        switch (spawn) {
        case 0: // DH
            Ddirection = 3;
            spawn = DH;
            break;
        case 1: // DG
            Ddirection = 1;
            spawn = DG;
            break;
        case 2: // DD
            Ddirection = 7;
            spawn = DD;
            break;
        case 3: // DB 
            Ddirection = 10;  // Exemple par d�faut
            spawn = DB;
            break;
        default:
            break;
        }
    }
    else if (type == 2) {
        switch (spawn) {
        case 0: // DH
            Ddirection = 14;
            spawn = DH;
            break;
        case 1: // DG
            Ddirection = 12;
            spawn = DG;
            break;
        case 2: // DD
            Ddirection = 16;
            spawn = DD;
            break;
        case 3: // DB 
            Ddirection = 18;  // Exemple par d�faut
            spawn = DB;
            break;
        default:
            break;
        }
    }
    else if (type == 3) {
        switch (spawn) {
        case 0: // DH
            Ddirection = 14;
            spawn = DH;
            break;
        case 1: // DG
            Ddirection = 12;
            spawn = DG;
            break;
        case 2: // DD
            Ddirection = 16;
            spawn = DD;
            break;
        case 3: // DB 
            Ddirection = 18;  // Exemple par d�faut
            spawn = DB;
            break;
        default:
            break;
        }
    }

    // Cr�er un v�hicule avec les propri�t�s g�n�r�es
    sf::Texture& vehicleTexture = type == 1 ? carTexture : (type == 2 ? busTexture : veloTexture);
    Vehicule newVehicle(spawn, direction, type, vehicleTexture);
    newVehicle.setDirections({ Ddirection }); // Assigner la direction

    // Ajouter le v�hicule au vecteur
    vehicules.push_back(newVehicle);

    std::cout << "Nouveau v�hicule g�n�r�: spawn=" << spawn
        << ", direction=" << direction
        << ", type=" << type
        << ", Ddirection=" << Ddirection << std::endl;
};


int main() {

    int back = true;

	// Initialisation des textures
	sf::Texture carTexture;
    if (!carTexture.loadFromFile("C:/Program Files/SFML/img/car.png")) {
        std::cerr << "Erreur: Impossible de charger la texture de la voiture." << std::endl;
        return -1; // Ou toute autre action appropri�e en cas d'erreur
    }
    else{
		std::cout << "Texture de la voiture charg�e avec succ�s." << std::endl;
    }

    sf::Texture busTexture;
    if (!busTexture.loadFromFile("C:/Program Files/SFML/img/bus.png")) {
        std::cerr << "Erreur: Impossible de charger la texture du Bus." << std::endl;
        return -1; // Ou toute autre action appropri�e en cas d'erreur
    }
    else {
        std::cout << "Texture du bus charg�e avec succ�s." << std::endl;
    }

    sf::Texture veloTexture;
    if (!veloTexture.loadFromFile("C:/Program Files/SFML/img/velo.png")) {
        std::cerr << "Erreur: Impossible de charger la texture du velo." << std::endl;
        return -1; // Ou toute autre action appropri�e en cas d'erreur
    }
    else {
        std::cout << "Texture du velo charg�e avec succ�s." << std::endl;
    }

    // Charger l'image de fond
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("C:/Program Files/SFML/img/background.png")) {
        std::cerr << "Erreur: Impossible de charger la texture de fond." << std::endl;
        return back = false;
    }
    else {
        std::cout << "Texture de fond charg�e avec succ�s." << std::endl;
    }

    // Cr�er un sprite pour l'image de fond
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

	// Initialisation des v�hicules
    std::vector<Vehicule> vehicules;
    Vehicule newCar(DG, 0, 1, carTexture);
    newCar.setDirections({ 2 });
    //vehicules.push_back(newCar);
    Vehicule newCar1(DH, 0, 1, carTexture);
    newCar1.setDirections({ 3 });
    // vehicules.push_back(newCar1);
    Vehicule newCar2(DD, 0, 1, carTexture);
    newCar2.setDirections({ 8 });
    //vehicules.push_back(newCar2);
    Vehicule newCar3(DB, 0, 1, carTexture);
    newCar3.setDirections({ 11 });
    //vehicules.push_back(newCar3);

    Vehicule newBus(DB, 0, 2, busTexture);
    newBus.setDirections({ 18 });
    vehicules.push_back(newBus);

    std::mutex traffic_light_mutex;

    Traffic_light traffic_light_master{ Traffic_color::red, sf::Vector2f(330, 575) };
    Traffic_light traffic_light_master1{ Traffic_color::red, sf::Vector2f(330, 625) };
    Traffic_light traffic_light_slave{ Traffic_color::green, sf::Vector2f(475, 675) };
    Traffic_light traffic_light_slave1{ Traffic_color::green, sf::Vector2f(525, 675) };
    Traffic_light traffic_light_master2{ Traffic_color::red, sf::Vector2f(600, 525) };
    Traffic_light traffic_light_master3{ Traffic_color::red, sf::Vector2f(600, 475) };
    Traffic_light traffic_light_master4{ Traffic_color::red, sf::Vector2f(775, 525) };
    Traffic_light traffic_light_master5{ Traffic_color::red, sf::Vector2f(775, 475) };
    Traffic_light traffic_light_slave2{ Traffic_color::green, sf::Vector2f(425, 425) };
    Traffic_light traffic_light_slave3{ Traffic_color::green, sf::Vector2f(375, 425) };
    Traffic_light traffic_light_slave4{ Traffic_color::green, sf::Vector2f(425, 250) };
    Traffic_light traffic_light_slave5{ Traffic_color::green, sf::Vector2f(375, 250) };

    // Ajout des feux au vecteur FeuTab
    std::vector<Traffic_light*> FeuTab = {
        &traffic_light_master, // FeuTab[0]
        &traffic_light_master1, // FeuTab[1]
        &traffic_light_slave, // FeuTab[2]
        &traffic_light_slave1, // FeuTab[3]
        &traffic_light_master2, // FeuTab[4]
        &traffic_light_master3, // FeuTab[5]
		&traffic_light_master4, // FeuTab[6]
		&traffic_light_master5, // FeuTab[7]
        &traffic_light_slave2, // FeuTab[8]
        &traffic_light_slave3, // FeuTab[9]
        &traffic_light_slave4, // FeuTab[10]
        &traffic_light_slave5 // FeuTab[11]
    };

    std::vector<std::vector<Traffic_light*>> phases;
    phases.push_back({ FeuTab[2], FeuTab[3], FeuTab[8], FeuTab[9], FeuTab[10], FeuTab[11] });// Phase 1 : Feux verticaux (nord-sud)
    phases.push_back({ FeuTab[0], FeuTab[1], FeuTab[4], FeuTab[5], FeuTab[6], FeuTab[7] });// Phase 2 : Feux horizontaux (est-ouest)

    std::stop_source stopping;
    std::jthread traffic_light_thread(run_traffic_light_multiple, std::ref(FeuTab), std::ref(phases), std::ref(traffic_light_mutex), stopping.get_token());

    // Log the positions of the traffic lights
    for (const auto& light : FeuTab) {
        std::cout << "Traffic light position: (" << light->get_position().x << ", " << light->get_position().y << ")" << std::endl;
    }

    // Fen�tre SFML
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE_HORIZ, WINDOW_SIZE_VERTI), "Traffic Simulation");

    std::jthread write_traffic_light(print_traffic_light, std::ref(traffic_light_master), std::ref(traffic_light_slave), stopping.get_token());

    bool running = true;

    sf::Clock clock;
    sf::Clock vehicleSpawnClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
                window.close();
            }
        }
        
        float deltaTime = clock.restart().asSeconds();

        if (vehicleSpawnClock.getElapsedTime().asSeconds() > 10.0f) {
            generateRandomVehicles(vehicules, carTexture, busTexture, veloTexture);
            vehicleSpawnClock.restart();
        }

        for (auto it = vehicules.begin(); it != vehicules.end();) {
            it->move(vehicules, FeuTab, traffic_light_mutex, deltaTime);

            // Supprimer les v�hicules hors des limites
            if (it->isOutOfBounds(WINDOW_SIZE_HORIZ, WINDOW_SIZE_VERTI)) {
                it = vehicules.erase(it);
            }
            else {
                ++it;
            }
        }

        // Dessiner la fen�tre
        window.clear(sf::Color::Black);

        window.draw(backgroundSprite);
       
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