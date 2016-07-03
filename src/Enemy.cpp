#include "Enemy.h"

Enemy::Enemy(Image &image, MapObjects &objects, Level &lvl, Vector2f Position, Vector2i Size, Vector2f &temp,
             String Name) : Entity(
        image, Position, Size, Name) {
    m_isAggro = false;
    m_playerCoordinates = &objects.playerPosition;
    objects.obj = lvl.GetObjects("solid");
    m_frameCounter = 0;
    sprite.setPosition(position.x + size.x / 2, position.y + size.y / 2);
    if (name == "easyEnemy") {
        speed = 0.003;
        boost = {position.x, position.y};
        sprite.setTextureRect(IntRect(0, 0, size.x, size.y));
    }
    m_explosionTexture.loadFromFile("IMG/Exp_type_B1.png");
    m_shieldRewardTexture.loadFromFile("IMG/shieldReward.png");
    m_healthRewardTexture.loadFromFile("IMG/healthReward.png");
}

void Enemy::checkCollisionWithMap(float Dx, float Dy, MapObjects &objects) {
    for (int i = 0; i < objects.obj.size(); i++) {
        if (RetRect().intersects(objects.obj[i].rect)) {
            if (objects.obj[i].name == "solid") {
                if (Dy > 0) {
                    position.y = objects.obj[i].rect.top - size.y;
                }
                if (Dy < 0) {
                    position.y = objects.obj[i].rect.top + objects.obj[i].rect.height;
                }
                if (Dx > 0) {
                    position.x = objects.obj[i].rect.left - size.x;
                }
                if (Dx < 0) {
                    position.x = objects.obj[i].rect.left + objects.obj[i].rect.width;
                }
            }
        }
    }
}

void Enemy::CreateNewReward() {
    sprite.setTextureRect(IntRect(0, 0, 40, 37));
    if (rand() % 2 == 0) {
        sprite.setTexture(m_healthRewardTexture);
        name = "HealthReward";
    }
    else {
        sprite.setTexture(m_shieldRewardTexture);
        name = "ShieldReward";
    }
}

void Enemy::ExplosionAnimation(const float &time) {
    m_frameCounter += 0.054 * time;
    sprite.setTexture(m_explosionTexture);
    sprite.setTextureRect(IntRect(96 * int(m_frameCounter), 0, 96, 96));
}

void Enemy::Update(float time, MapObjects &objects) {

    SetRightPosition(position);
    if (name == "easyEnemy") {
        distance = sqrt((m_playerCoordinates->x - position.x) * (m_playerCoordinates->x - position.x) +
                        (m_playerCoordinates->y - position.y) * (m_playerCoordinates->y -
                                                                 position.y));
        if (!m_isAggro && (distance < 700 || healthEasyEnemy != EASY_ENEMY_MAX_HEALTH)) {
            m_isAggro = true;
        }
        else if (m_isAggro) {
            m_rotation = static_cast<float>(
                    (atan2(m_playerCoordinates->y - position.y, m_playerCoordinates->x - position.x)) *
                    parameters.ANGLE / M_PI);
            if (distance > 250) {
                velocity += {static_cast<float>(0.002 * time * (m_playerCoordinates->x - position.x) / distance),
                             static_cast<float>(0.002 * time * (m_playerCoordinates->y - position.y) / distance)};
            }
            else {
                velocity.x *= 0.985;
                velocity.y *= 0.985;
            }
            sprite.setRotation(m_rotation);
            checkCollisionWithMap(boost.x, boost.y, objects);
            position.x += velocity.x;
            position.y += velocity.y;
            sprite.setPosition(position.x + size.x / 2, position.y + size.y / 2);
        }
        if (healthEasyEnemy <= 0) {
            name = "explosion";
        }
    }
    else if (name == "explosion") {
        ExplosionAnimation(time);
        if (m_frameCounter > 64) {
            CreateNewReward();
        }
    }
}

