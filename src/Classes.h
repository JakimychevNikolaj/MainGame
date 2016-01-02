#pragma once

#include <SFML/Graphics.hpp>
#include "level.h"
#include <vector>
#include <list>

using namespace sf;

void getPlayerCoordinateForView(Vector2f position);

struct Parameters {
	const unsigned int WINDOW_SIZE_X = 1680;
	const unsigned int WINDOW_SIZE_Y = 1050;
	const unsigned int ANGLE = 180;
} parameters;

struct PlayerBulletStruct {
	const unsigned int WIDTH = 100;
	const unsigned int HEIGHT = 14;
	const unsigned int DAMAGE = 10;
} playerBulletStruct;

struct EasyEnemyStruct {
	const unsigned int WIDTH = 74;
	const unsigned int HEIGHT = 67;
	const unsigned int DAMAGE = 1;
	const float SPEED = 0.1;
} easyEnemyStruct;

struct PlayerStruct {
	const unsigned int WIDTH = 147;
	const unsigned int HEIGHT = 125;
	const int HEALTH = 100;
	const float SPEED = 0.2;
} playerStruct;




class Entity {
public:
	std::vector<Object> obj;
	float dx, dy, speed, moveTimer;
	Vector2f position;
	int health;
	Vector2i size;
	bool life, isMove, isSelect;
	Texture texture;
	Sprite sprite;
	String name;
	Entity(Image &image, float X, float Y, int W, int H, String Name) {
		position.x = X; position.y = Y; size.x = W; size.y = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(size.x / 2, size.y / 2);
	}
	FloatRect getRect() {
		return FloatRect(position.x, position.y, size.x, size.y);
	}
	virtual void update(float time) = 0;
};


class Player :public Entity {
public:
	enum { left, right, up, down, leftUp, rightUp, leftDown, rightDown, stay } state;
	float rotation;
	int playerScore;
	Player(Image &image, Level &lev, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
		playerScore = 0;
		state = stay;
		isSelect = false;
		obj = lev.GetAllObjects();
		if (name == "Player") {
			sprite.setPosition(size.x, size.y);
		}
	}

	void control() {
		bool pressBut = false;
		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			state = left;
			speed = playerStruct.SPEED;
			pressBut = true;
		}
		if (Keyboard::isKeyPressed(Keyboard::Right)) {
			state = right;
			speed = playerStruct.SPEED;
			pressBut = true;
		}
		if (Keyboard::isKeyPressed(Keyboard::Up)) {
			if (pressBut) {
				if (state == right) {
					state = rightUp;
					speed = playerStruct.SPEED;
				}
				if (state == left) {
					state = leftUp;
					speed = playerStruct.SPEED;
				}
			}
			else {
				state = up;
				speed = playerStruct.SPEED;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Down)) {
			if (pressBut) {
				if (state == right) {
					state = rightDown;
					speed = playerStruct.SPEED;
				}
				if (state == left) {
					state = leftDown;
					speed = playerStruct.SPEED;
				}
			}
			else {
				state = down;
				speed = playerStruct.SPEED;
			}
		}
	}
	void rotation_GG(Vector2f pos) {
		float dX = pos.x - position.x;
		float dY = pos.y - position.y;
		rotation = (atan2(dY, dX)) * parameters.ANGLE / M_PI;
	}
	void checkCollisionWithMap(float Dx, float Dy) {
		for (int i = 0; i<obj.size(); i++) {
			if (getRect().intersects(obj[i].rect)) {
				if (obj[i].name == "solid") {
					if (Dy > 0) {
						position.y = obj[i].rect.top - size.y;
						dy = 0;
					}
					if (Dy < 0) {
						position.y = obj[i].rect.top + obj[i].rect.height;
						dy = 0;
					}
					if (Dx > 0) {
						position.x = obj[i].rect.left - size.x;
					}
					if (Dx < 0) {
						position.x = obj[i].rect.left + obj[i].rect.width;
					}
				}
			}
		}
	}

	void update(float time)	{
		sprite.setRotation(rotation);
		control();
		switch (state) {
			case right: dx = speed; dy = 0; break;
			case rightUp: dx = speed; dy = -speed; break;
			case rightDown: dx = speed; dy = speed; break;
			case left: dx = -speed; dy = 0; break;
			case leftUp: dx = -speed; dy = -speed; break;
			case leftDown: dx = -speed; dy = speed; break;
			case up: dx = 0; dy = -speed; break;
			case down: dx = 0; dy = speed; break;
			case stay: break;
		}
		position.x += dx*time;
		checkCollisionWithMap(dx, 0);
		position.y += dy*time;
		checkCollisionWithMap(0, dy);
		sprite.setPosition(position.x + size.x / 2, position.y + size.y / 2);

		if (health <= 0) {
			life = false;
		}
		if (!isMove) {
			speed = 0;
		}
		if (life) {
			getPlayerCoordinateForView(position);
		}
	}
};


class Enemy :public Entity {
public:
	Enemy(Image &image, Level &lvl, float X, float Y, int W, int H, String Name) :Entity(image, X, Y, W, H, Name) {
		obj = lvl.GetObjects("solid");
		if (name == "easyEnemy") {
			sprite.setTextureRect(IntRect(0, 0, size.x, size.y));
			sprite.rotate(parameters.ANGLE);
			dx = -easyEnemyStruct.SPEED;
		}
	}

	void checkCollisionWithMap(float Dx, float Dy) {
		for (int i = 0; i < obj.size(); i++) {
			if (getRect().intersects(obj[i].rect)) {
				if (obj[i].name == "solid") {
					if (Dy > 0) {
						position.y = obj[i].rect.top - size.y;
						dy = -easyEnemyStruct.SPEED;
					}
					if (Dy < 0) {
						position.y = obj[i].rect.top + obj[i].rect.height;
						dy = easyEnemyStruct.SPEED;
					}
					if (Dx > 0) {
						position.x = obj[i].rect.left - size.x;
						dx = -easyEnemyStruct.SPEED;
					}
					if (Dx < 0) {
						position.x = obj[i].rect.left + obj[i].rect.width;
						dx = easyEnemyStruct.SPEED;
					}
				}
			}
		}
	}

	void update(float time) {
		if (name == "easyEnemy") {
			checkCollisionWithMap(dx, dy);
			position.x += dx*time;
			position.y += dy*time;
			sprite.setPosition(position.x + size.x / 2, position.y + size.y / 2);
			if (health <= 0) {
				life = false;
			}
		}
	}
};


class Bullet :public Entity {
public:
	int direction;
	float tempy, tempx, rotation, Dx, Dy;
	Bullet(Image &image, Level &lvl, float X, float Y, int W, int H, float tempX, float tempY, String Name) :Entity(image, X, Y, W, H, Name) {
		obj = lvl.GetObjects("solid");
		position.x = X;
		position.y = Y;
		speed = 0.1;
		tempx = tempX;
		tempy = tempY;
		size.x = W;
		size.y = H;
		life = true;
		dx = position.x;
		dy = position.y;
		life = true;
		Dx = tempx - position.x;
		Dy = tempy - position.y;
		rotation = (atan2(Dy, Dx)) * parameters.ANGLE / M_PI;
	}

	void update(float time) {
		position.x += speed * (tempx - dx);
		position.y += speed * (tempy - dy);

		if (position.x <= 0)
			position.x = 1;

		if (position.y <= 0)
			position.y = 1;

		for (int i = 0; i < obj.size(); i++) {
			if (getRect().intersects(obj[i].rect)) {
				life = false;
			}
		}
		sprite.setRotation(rotation);
		sprite.setPosition(position.x + size.x / 2, position.y + size.y / 2);
	}
};