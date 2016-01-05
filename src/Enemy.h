#pragma once
#include "Entity.h"


using namespace sf;
using namespace std;

struct Enemy :public Entity {
public:
    Enemy(Image &image, MapObjects & objects, Level &lvl, Vector2f Position, Vector2i Size, String Name);
    void checkCollisionWithMap(float Dx, float Dy, MapObjects & objects);
    void update(float time, MapObjects & objects);
};