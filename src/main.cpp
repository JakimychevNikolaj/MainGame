#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"




struct Application {
	const Vector2f MAP_WIDTH = {850, 2400};
	const Vector2f MAP_HEIGHT = {530, 2600};
	Clock clock;
	Level lvl;
	sf::View view;
	std::list<EntityLogic*> entities;
	std::list<EntityVisual*> entitiesVisual;
};

Application g_application;

struct PlayerPosition {
	Vector2f pos;
};

struct ImageAssets {
	Image heroImage;
	Image easyEnemyImage;
	Image bulletImage;
};


void getPlayerCoordinateForView(Vector2f position) {
	Vector2f centerPosition = {position.x, position.y};
	if (position.x < g_application.MAP_WIDTH.x) centerPosition.x = g_application.MAP_WIDTH.x;
	if (position.x > g_application.MAP_WIDTH.y) centerPosition.x = g_application.MAP_WIDTH.y;
	if (position.y < g_application.MAP_HEIGHT.x) centerPosition.y = g_application.MAP_HEIGHT.x;
	if (position.y > g_application.MAP_HEIGHT.y) centerPosition.y = g_application.MAP_HEIGHT.y;
	g_application.view.setCenter(centerPosition.x, centerPosition.y);
}

float RunTimer() {
	float time_ms = g_application.clock.getElapsedTime().asMicroseconds();
	g_application.clock.restart();
	time_ms = time_ms * 0.001;
	return time_ms;
}

void ProcessEvents(RenderWindow & window, Player & protagonist, ImageAssets & imagesStruct, PlayerPosition & playerPosition, PlayerBullet & playerBullet, MapObjects & objects) {
	sf::Event event;
	while (window.pollEvent(event))	{
		if (event.type == Event::Closed) {
			window.close();
		}
		if (event.key.code == Mouse::Left) {
			g_application.entities.push_back(new Bullet(imagesStruct.bulletImage, objects, g_application.lvl, protagonist.position, playerBullet.SIZE, playerPosition.pos, "Bullet"));
			//g_application.entitiesVisual.push_back();
		}
	}
}

void GetMousePosition(RenderWindow & window, PlayerPosition & playerPosition) {
	Vector2i pixelPos = Mouse::getPosition(window);
	playerPosition.pos = window.mapPixelToCoords(pixelPos);
}

void InitializeImages(ImageAssets & imagesStruct) {
	g_application.lvl.LoadFromFile("Assets/map.tmx");
	imagesStruct.bulletImage.loadFromFile("IMG/projectile_bolt_blue_single.png");
	imagesStruct.heroImage.loadFromFile("IMG/PlayerShip.png");
	imagesStruct.easyEnemyImage.loadFromFile("IMG/EasyEnemy.png");
}

Object InitializePlayer() {
	Object player = g_application.lvl.GetObject("player");
	return player;
}

bool IsAliveEntity(EntityLogic *entity) {
	return !entity->alive;
}

void ProcessEntities(float & time_ms, MapObjects & objects) {
	auto new_end = std::remove_if(g_application.entities.begin(), g_application.entities.end(), IsAliveEntity);
	g_application.entities.erase(new_end, g_application.entities.end());
	for (auto it : g_application.entities) {
		it->update(time_ms, objects);
	}
}

void ProcessDamage(Player & protagonist, PlayerBullet & playerBullet, EasyEnemy & easyEnemy, EntityVisual * visual) {
	for (auto it : g_application.entities) {
		for (auto at : g_application.entities) {
			if ((it)->getRect(*visual).intersects((at)->getRect(*visual)) && (((at)->name == "Bullet") && ((it)->name == "easyEnemy"))) {
				(it)->health -= playerBullet.DAMAGE;
				(at)->alive = false;
			}
		}
		if ((it)->getRect(*visual).intersects(protagonist.getRect())) {
			if ((it)->name == "easyEnemy") {
				(it)->boost.x = 0;
				protagonist.health -= easyEnemy.DAMAGE;
			}
		}
	}
}

void AppendEnemies(std::vector<Object> & easyOpponent, ImageAssets & imagesStruct, EasyEnemy & easyEnemy, MapObjects & objects) {
	for (int i = 0; i < easyOpponent.size(); i++) {
		g_application.entities.push_back(new Enemy(imagesStruct.easyEnemyImage, objects, g_application.lvl, {easyOpponent[i].rect.left, easyOpponent[i].rect.top}, easyEnemy.SIZE, "easyEnemy"));
	}
}

void CheckExistenceProtagonist(Player &protagonist, RenderWindow &window) {
	if (!protagonist.alive)
		window.close();
}

void Draw(RenderWindow &window, Player & protagonist) { /////////////////////////////////
	window.clear();
	g_application.lvl.Draw(window);
	for (auto it : g_application.entitiesVisual) {
		window.draw((it)->sprite);
	}
	window.draw(protagonist.sprite);
	window.display();
}


int main() {
	EntityVisual *visual;
	EasyEnemy easyEnemy;
	PlayerBullet playerBullet;
	PlayerProperties playerProperties;
	Parameters parameters;
	MapObjects objects;
	sf::RenderWindow window(sf::VideoMode(parameters.WINDOW_SIZE_X, parameters.WINDOW_SIZE_Y), "Game");
	g_application.view.reset(sf::FloatRect(0, 0, parameters.WINDOW_SIZE_X, parameters.WINDOW_SIZE_Y));
	ImageAssets imageAssets;
	PlayerPosition playerPosition;
	InitializeImages(imageAssets);
	Object player = InitializePlayer();
	std::vector<Object> easyOpponent = g_application.lvl.GetObjects("easyEnemy");
	Player protagonist(imageAssets.heroImage, objects, g_application.lvl, {player.rect.left, player.rect.top}, playerProperties.SIZE, "Player");
	AppendEnemies(easyOpponent, imageAssets, easyEnemy, objects);
	while (window.isOpen()) {
		GetMousePosition(window, playerPosition);
		float time_ms = RunTimer();
		ProcessEvents(window, protagonist, imageAssets, playerPosition, playerBullet, objects);
		protagonist.rotation_GG(playerPosition.pos);
		protagonist.update(time_ms, objects);
		ProcessEntities(time_ms, objects);
		ProcessDamage(protagonist, playerBullet, easyEnemy, visual);
		CheckExistenceProtagonist(protagonist, window);
		window.setView(g_application.view);
		Draw(window, protagonist);
	}
	return 0;
}