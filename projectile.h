#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Map.h"
#include "explosion.h"

class Projectile {
public:
    Projectile(glm::vec2 startPosition, float angle, float speed);

    void update(float deltaTime, Map& map);
    void render();

    bool isActive() const { return active; }
    bool hasHitTarget() const { return hitTarget; }
    glm::vec2 getPosition() const { return position; }


    glm::vec2 position;
    float angle;
    float speed;
    bool active;
    bool hitTarget;

    unsigned int texture;
    unsigned int VAO, VBO;
    unsigned int shader;

    void initialize();
    void checkMapCollision(Map& map);
};

#endif