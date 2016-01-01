#include <SFML/Graphics.hpp>
#include <iostream>
#include "level.h"
#include "Classes.h"


using namespace sf;

struct Application {
	Clock clock;
	Level lvl;
	sf::View view;
	std::list<Entity*> entities;
} application;

struct PlayerPosition {
	Vector2i pixelPos;
	Vector2f pos;
} playerPosition;

struct ImagesStruct {
	Image heroImage, easyEnemyImage;
	Image bulletImage;
} imagesStruct;

void getPlayerCoordinateForView(float x, float y) {
	float tempX = x; float tempY = y;

	if (x < 685) tempX = 685;
	if (x > 1325) tempX = 1325;
	if (y < 380) tempY = 380;
	if (y > 1650) tempY = 1650;

	application.view.setCenter(tempX, tempY);

}


void ProcessEvents(RenderWindow & window, Player & protagonist) {
	sf::Event event;
	while (window.pollEvent(event))	{
		if (event.type == Event::Closed) {
			window.close();
		}
		if (event.key.code == Mouse::Left) {
			application.entities.push_back(new Bullet(imagesStruct.bulletImage, application.lvl, protagonist.x, protagonist.y, playerBulletStruct.WIDTH, playerBulletStruct.HEIGHT, playerPosition.pos.x, playerPosition.pos.y, "Bullet"));
		}
	}
}

void GetMousePosition(RenderWindow & window) {
	playerPosition.pixelPos = Mouse::getPosition(window);
	playerPosition.pos = window.mapPixelToCoords(playerPosition.pixelPos);
}

void InitializeImages() {
	imagesStruct.bulletImage.loadFromFile("IMG/projectile_bolt_blue_single.png");
	imagesStruct.heroImage.loadFromFile("IMG/PlayerShip.png");
	imagesStruct.easyEnemyImage.loadFromFile("IMG/EasyEnemy.png");
}

int main() {
	sf::RenderWindow window(sf::VideoMode(parameters.WINDOW_SIZE_X, parameters.WINDOW_SIZE_Y), "Game");
	application.view.reset(sf::FloatRect(0, 0, parameters.WINDOW_SIZE_X, parameters.WINDOW_SIZE_Y));

	application.lvl.LoadFromFile("Assets/map.tmx");

	Object player = application.lvl.GetObject("player");


	std::list<Entity*>::iterator it;

	std::vector<Object> easyOpponent = application.lvl.GetObjects("easyEnemy");

	InitializeImages();

	Player protagonist(imagesStruct.heroImage, application.lvl, player.rect.left, player.rect.top, playerStruct.WIDTH, playerStruct.HEIGHT, "Player");

	for (int i = 0; i < easyOpponent.size(); i++)
		application.entities.push_back(new Enemy(imagesStruct.easyEnemyImage, application.lvl, easyOpponent[i].rect.left, easyOpponent[i].rect.top, easyEnemyStruct.WIDTH, easyEnemyStruct.HEIGHT, "easyEnemy"));

	while (window.isOpen()) {

		GetMousePosition(window);
		float time = application.clock.getElapsedTime().asMicroseconds();

		application.clock.restart();
		time = time / 800;
		ProcessEvents(window, protagonist);
		protagonist.rotation_GG(playerPosition.pos);
		protagonist.update(time);
		for (it = application.entities.begin(); it != application.entities.end();) {
			Entity *projectile = *it;
			projectile->update(time);
			if (!projectile->life) {
				it = application.entities.erase(it);
				delete projectile;
			}
			else it++;
		}
		for (auto it = application.entities.begin(); it != application.entities.end(); ++it) {
			for (auto at = application.entities.begin(); at != application.entities.end(); ++at) {
				if ((*it)->getRect().intersects((*at)->getRect()) && (((*at)->name == "Bullet") && ((*it)->name == "easyEnemy"))) {
					(*it)->health -= playerBulletStruct.DAMAGE;
					(*at)->life = false;
				}
			}
			if ((*it)->getRect().intersects(protagonist.getRect())) {
				if ((*it)->name == "easyEnemy") {
					(*it)->dx = 0;
					protagonist.health -= easyEnemyStruct.DAMAGE;
				}
			}
		}
		if (!protagonist.life)
			window.close();
		window.setView(application.view);
		window.clear();
		application.lvl.Draw(window);
		for (it = application.entities.begin(); it != application.entities.end(); it++) {
			window.draw((*it)->sprite);
		}
		window.draw(protagonist.sprite);
		window.display();
	}
	return 0;
}