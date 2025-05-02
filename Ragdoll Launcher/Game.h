#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "SFMLRenderer.h"
#include <list>
#include <vector>
#include <cstdlib>
#include <ctime> 


using namespace sf;
class Game
{
private:
	//Sistemas principales
	sf::RenderWindow* wnd;
	b2World* phyWorld;
	SFMLRenderer* renderer;

	//Configuración
	float fps;
	float frameTime;
	sf::Color clearColor;

	//Cañón
	b2Body* cannonBase;
	b2Body* cannonBarrel;
	b2RevoluteJoint* cannonJoint;

	//las paredes y límites
	b2Body* groundBody;   // Suelo
	b2Body* leftWall;     // Pared izquierda
	b2Body* rightWall;    // Pared derecha
	b2Body* ceiling;      // Techo

	struct Ragdoll {
		std::vector<b2Body*> parts;
		bool hasBeenFired;
	};

	std::vector<Ragdoll> ragdolls; //Lista de ragdolls
	std::vector<b2Body*> currentRagdollParts; //Partes del ragdoll en creación

	//Obstáculos (estáticos o dinámicos)
	struct Obstacle {
		b2Body* body;
		bool isStatic;
		enum Type { PLATFORM, BARREL, COLUMN } type;
	};

	std::vector<Obstacle> obstacles;
	void CreateCannon();
	void UpdateCannonAim();
	void ShootRagdoll();

	b2Vec2 GenerateRandomPosition();

public:

	//Constructores, destructores e inicializadores
	Game(int ancho, int alto, std::string titulo);
	void Run();
	void InitPhysics();
	void CreateRagdoll(const b2Vec2& position);
	void SetZoom();
	void DoEvents();
	void UpdatePhysics();
	void DrawGame();

};