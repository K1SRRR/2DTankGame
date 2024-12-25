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
#include "healthBar.h"
using namespace irrklang;

enum class TankType {
    HEAVY,
    LIGHT
};
const float SHOOT_COOLDOWN = 0.4f;

class Tank {
public:
    glm::vec2 position;     // Pozicija tenka (x, y)
    float bodyAngle;        // Ugao rotacije tela tenka
    float turretAngle;      // Ugao rotacije kupole
    float speed;            // Brzina kretanja
    float rotationSpeed;    // Brzina rotacije tela
    int ammunitionAP = 10;  // Broj AP granata
    int ammunitionHE = 10;  // Broj HE granata
    TankType type;
    float hitpoints = 100;
    HealthBar healthBar;
    ProjectileType currentProjectileType = ProjectileType::AP;
    float lastShotTime = -SHOOT_COOLDOWN;
    Turret* turret;
    std::vector<Projectile*> projectiles;
    std::vector<Explosion*> explosions;
    float projectileSpeed = 1.0f;
    bool isDestroyed = false; 
    ISound* drivingSound = nullptr;
    std::vector<Tank*> enemyTanks;
    float nextShotTime = 2.0f;

    unsigned int texture;
    unsigned int VAO, VBO;
    unsigned int unifiedShader;

    Tank(glm::vec2 initPosition, float initBodyAngle, float initSpeed, float initRotationSpeed, TankType type, const char* textureSourceFile);

    void render(float deltaTime, Map& map, ISoundEngine& soundEngine);
    void moveForward(float deltaTime, Map& map);
    void moveBackward(float deltaTime, Map& map);
    void rotateBodyLeft(float deltaTime);
    void rotateBodyRight(float deltaTime);
    void aimTurret(float mouseX, float mouseY, int windowWidth, int windowHeight);
    bool canShoot(float currentTime) const;
    void shoot(float currentTime, ISoundEngine& SoundEngine);
    void setAmmunitionAP(float currentTime);
    void setAmmunitionHE(float currentTime);
    void aimTurretEnemy(float mouseX, float mouseY, int windowWidth, int windowHeight);
    void CheckProjectileTankHit(Tank& tank, Projectile& projectile);
    void reset(glm::vec2 initPosition, float initBodyAngle);

    void updateProjectiles(ISoundEngine& soundEngine);
    void renderProjectiles(float deltaTime, Map& map);
    void updateExplosions();
    void renderExplosions(float deltaTime);

    void setTurret(Turret* tankTurret);
    Turret getTurret();
    void setEnemy(Tank& tank);
    void setAngle(float newAngle);

    void followPath(const std::vector<glm::vec2>& path, float deltaTime, Map& map);
    void moveToTarget(glm::vec2 target, float deltaTime, Map& map); // Nova metoda
    void performAITasks(glm::vec2 playerPosition, float currentTime, float deltaTime, Map& map, int windowWidth, int windowHeight, ISoundEngine& SoundEngine); // AI logika

    glm::mat4 getModelMatrix() const;
    ~Tank();
};

#endif // TANK_H
