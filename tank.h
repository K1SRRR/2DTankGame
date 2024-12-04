#ifndef TANK_H
#define TANK_H

#include <glm/glm.hpp> // Za pozicije, rotaciju i transformacije
#include <glm/gtc/matrix_transform.hpp> // Za matrice transformacija
#include <GL/glew.h>
#include "turret.h"
#include "map.h"
#include "projectile.h"
#include "explosion.h"

const float SHOOT_COOLDOWN = 2.0f;

class Tank {
public:
    // Osnovni atributi tenka
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
    float projectileSpeed = 1.0f;  // Adjust as needed
    unsigned int texture;
    unsigned int VAO, VBO;
    unsigned int unifiedShader;
    bool isDestroyed = false;
    //float vertices[];

    // Konstruktor
    Tank(glm::vec2 initPosition, float initSpeed, float initRotationSpeed, const char* textureSourceFile);

    void render();

    // Funkcije
    void moveForward(float deltaTime, Map& map);
    void moveBackward(float deltaTime, Map& map);
    void rotateBodyLeft(float deltaTime, Map& map);
    void rotateBodyRight(float deltaTime, Map& map);
    void aimTurret(float mouseX, float mouseY, int windowWidth, int windowHeight);
    bool canShoot(float currentTime) const;
    void shoot(float currentTime);
    void resetPosition();

    void setTurret(Turret* tankTurret);
    Turret getTurret();

    // Matrica modela za rendering
    glm::mat4 getModelMatrix() const;
};

#endif // TANK_H
