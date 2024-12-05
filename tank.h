#ifndef TANK_H
#define TANK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include "turret.h"
#include "map.h"
#include "projectile.h"
#include "explosion.h"
#include <irrKlang.h>
using namespace irrklang;

const float SHOOT_COOLDOWN = 2.0f;

class Tank {
public:
    glm::vec2 position;     // Pozicija tenka (x, y)
    float bodyAngle;        // Ugao rotacije tela tenka
    float turretAngle;      // Ugao rotacije kupole
    float speed;            // Brzina kretanja
    float rotationSpeed;    // Brzina rotacije tela
    int ammunition = 50;
    float lastShotTime = -SHOOT_COOLDOWN;
    Turret* turret;
    std::vector<Projectile*> projectiles;
    std::vector<Explosion*> explosions;
    float projectileSpeed = 1.0f;
    unsigned int texture;
    unsigned int VAO, VBO;
    unsigned int unifiedShader;
    bool isDestroyed = false; 
    ISound* drivingSound = nullptr;

    Tank(glm::vec2 initPosition, float initBodyAngle, float initSpeed, float initRotationSpeed, const char* textureSourceFile);

    void render();
    void moveForward(float deltaTime, Map& map);
    void moveBackward(float deltaTime, Map& map);
    void rotateBodyLeft(float deltaTime);
    void rotateBodyRight(float deltaTime);
    void aimTurret(float mouseX, float mouseY, int windowWidth, int windowHeight);
    bool canShoot(float currentTime) const;
    void shoot(float currentTime, ISoundEngine& SoundEngine);

    void setTurret(Turret* tankTurret);
    Turret getTurret();

    glm::mat4 getModelMatrix() const;
};

#endif // TANK_H
