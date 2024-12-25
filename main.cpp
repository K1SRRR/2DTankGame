// Autor: Srdjan Ilic

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <GL/glew.h>   
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//Biblioteke za pokretanje generatora podataka
#include "Tank.h"
#include "turret.h"
#include "crosshair.h"
#include "map.h"
#include <map>
#include <string>
#include "globalFunctions.h"
#include "pauseMenu.h"
#include "levelCompleteMenu.h"
#include "TextRenderer.h"

#include <thread>
#include <chrono>
#include <irrKlang.h>
using namespace irrklang;

const unsigned int WINDOW_WIDTH = 900;
const unsigned int WINDOW_HEIGHT= 900;
const float TARGET_FPS = 120.0f;
const float FRAME_TIME = 1.0f / TARGET_FPS;
const float GAME_TIME = 20.0f;
const int TOTAL_LEVELS = 2;

void processInput(GLFWwindow* window, ISoundEngine& SoundEngine, Tank& tank, Crosshair& crosshair, Turret& turret, Map& map,
                  PauseMenu& pauseMenu, float deltaTime);
void TextRendering(unsigned int textRenderingShader, Tank& tank, TextRenderer& textRenderer, float elapsedGameTime, bool isGameWon);
void setupLevel(int level, Tank& tank, Tank& tank2, Tank& tank3,
    Tank& light1, Tank& light2, Tank& light3, Tank& light4, Map& map);
//void CheckProjectileTankHit(ISoundEngine& SoundEngine, Tank& tank, Projectile& projectile);

int main(void)
{
    srand(static_cast<unsigned>(time(0)));
    if (!glfwInit()) 
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window; 
    const char wTitle[] = "2D tank game";
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, wTitle, NULL, NULL);
    if (window == NULL) 
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate(); 
        return 2; 
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) 
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int textRenderingShader = createShaderFromFile("textShader.vert", "textShader.frag");
    TextRenderer mainTextRenderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    mainTextRenderer.init("BRLNSR.TTF", 48);
    TextRenderer pauseMenuTextRenderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    pauseMenuTextRenderer.init("BRLNSR.TTF", 48);
    TextRenderer levelCompleteMenuTextRenderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    levelCompleteMenuTextRenderer.init("BRLNSR.TTF", 48);

    Tank tank(glm::vec2(0.0f, -0.7f), 0.0f, 0.3f, 60.0f, TankType::HEAVY, "tank.png"); // Početna pozicija, pocetni ugao, brzina, rotaciona brzina
    Turret turret(glm::vec2(0.0f, 0.0f), "turret.png");
    tank.setTurret(&turret);
    Tank tank2(glm::vec2(-0.7f, 0.7f), 0.0f, 0.03f, 2.0f, TankType::HEAVY, "tank2.png"); // Početna pozicija, pocetni ugao, brzina, rotaciona brzina
    Turret turret2(glm::vec2(0.0f, 0.0f), "turret2.png");
    tank2.setTurret(&turret2);
    Tank tank3(glm::vec2(0.7f, 0.7f), 0.0f, 0.03f, 2.0f, TankType::HEAVY, "tank2.png"); // Početna pozicija, pocetni ugao, brzina, rotaciona brzina
    Turret turret3(glm::vec2(0.0f, 0.0f), "turret2.png");
    Tank light1(glm::vec2(0.5f, 0.4f), 0.0f, 0.0f, 0.0f, TankType::LIGHT, "light-target.png");
    Tank light2(glm::vec2(0.7f, 0.4f), 0.0f, 0.0f, 0.0f, TankType::LIGHT, "light-target.png");
    Tank light3(glm::vec2(-0.5f, 0.4f), 0.0f, 0.0f, 0.0f, TankType::LIGHT, "light-target.png");
    Tank light4(glm::vec2(-0.7f, 0.4f), 0.0f, 0.0f, 0.0f, TankType::LIGHT, "light-target.png");
    tank3.setTurret(&turret3);
    tank.setEnemy(tank2);
    tank.setEnemy(tank3);
    tank.setEnemy(light1);
    tank.setEnemy(light2);
    tank.setEnemy(light3);
    tank.setEnemy(light4);
    tank2.setEnemy(tank); tank3.setEnemy(tank);
    light1.setEnemy(tank); light2.setEnemy(tank); light3.setEnemy(tank); light4.setEnemy(tank);
    light1.setAngle(-90.0f); light2.setAngle(-90.0f); light3.setAngle(-90.0f); light4.setAngle(-90.0f);

    Crosshair crosshair; 
    Map map;
    map.setTile(5, 4, TileType::WALL);
    map.setTile(6, 4, TileType::WALL);
    map.setTile(7, 4, TileType::WALL);
    map.setTile(8, 4, TileType::WALL);
    map.setTile(9, 4, TileType::WALL);
    map.setTile(5, 10, TileType::WALL);
    map.setTile(6, 10, TileType::WALL);
    map.setTile(7, 10, TileType::WALL);
    map.setTile(8, 10, TileType::WALL);
    map.setTile(9, 10, TileType::WALL);
    map.placeBush(4, 5);
    map.placeBush(5, 5);
    map.placeBush(6, 5);
    map.placeBush(7, 5);
    map.placeBush(8, 5);
    map.placeBush(9, 5);
    map.placeBush(10, 5);
    map.placeBush(4, 9);
    map.placeBush(5, 9);
    map.placeBush(6, 9);
    map.placeBush(7, 9);
    map.placeBush(8, 9);
    map.placeBush(9, 9);
    map.placeBush(10, 9);
    PauseMenu pauseMenu(WINDOW_WIDTH, WINDOW_HEIGHT, pauseMenuTextRenderer);
    int currentLevel = 1;
    LevelCompleteMenu levelCompleteMenu(WINDOW_WIDTH, WINDOW_HEIGHT, levelCompleteMenuTextRenderer);
    
    Projectile apProjectile(glm::vec2(0.45f, -0.93f), 0.0f, 0.0f, ProjectileType::AP, 2.5f);
    Projectile heProjectile(glm::vec2(0.75f, -0.93f), 0.0f, 0.0f, ProjectileType::HE, 2.5f);
    //HealthBar healthBar;

    ISoundEngine* SoundEngine = createIrrKlangDevice(); 
    SoundEngine->setSoundVolume(0.15f);
    //SoundEngine->play2D("GameBackgroundMusic.mp3", true);///////////////////////////////////////////////////

    float gameStartTime = glfwGetTime();
    bool isGameWon = false;
    float previousTime = 0.0f;
    float deltaTime = 0.0f;
    bool APSelected = true;
    bool HESelected = false;
    setupLevel(currentLevel, tank, tank2, tank3, light1, light2, light3, light4, map);

    while (!glfwWindowShouldClose(window))
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        float currentTime = glfwGetTime();
        float elapsedGameTime = currentTime - gameStartTime;
        float frameTime = currentTime - previousTime;

        if (frameTime >= FRAME_TIME) {
            deltaTime = frameTime;
            std::cout << deltaTime << std::endl;
            currentTime = glfwGetTime();
            previousTime = currentTime;

            if (tank2.isDestroyed && tank3.isDestroyed && ((GAME_TIME - elapsedGameTime) > 0.0)) {
                isGameWon = true;
            }

            glClearColor(0.5, 0.5, 0.5, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);

            map.render();
            std::vector<Tank*> enemyTanks = { &tank2, &tank3 };
            if (currentLevel == 2) {
                if (!tank2.isDestroyed) tank2.render(deltaTime, map, *SoundEngine);
                if (!tank2.isDestroyed) turret2.render();
                if (!tank3.isDestroyed) tank3.render(deltaTime, map, *SoundEngine);
                if (!tank3.isDestroyed) turret3.render();
            }
            if (!tank.isDestroyed) tank.render(deltaTime, map, *SoundEngine);
            if (!tank.isDestroyed) turret.render();
            light1.render(deltaTime, map, *SoundEngine);
            light2.render(deltaTime, map, *SoundEngine);
            light3.render(deltaTime, map, *SoundEngine);
            light4.render(deltaTime, map, *SoundEngine);
            crosshair.render(tank, currentTime);
            apProjectile.render();
            heProjectile.render();
            //tank2.performAITasks(tank.position, currentTime, deltaTime, map, WINDOW_WIDTH, WINDOW_HEIGHT, *SoundEngine);
            light1.performAITasks(tank.position, currentTime, deltaTime, map, WINDOW_WIDTH, WINDOW_HEIGHT, *SoundEngine);
            light2.performAITasks(tank.position, currentTime, deltaTime, map, WINDOW_WIDTH, WINDOW_HEIGHT, *SoundEngine);
            light3.performAITasks(tank.position, currentTime, deltaTime, map, WINDOW_WIDTH, WINDOW_HEIGHT, *SoundEngine);
            light4.performAITasks(tank.position, currentTime, deltaTime, map, WINDOW_WIDTH, WINDOW_HEIGHT, *SoundEngine);

            TextRendering(textRenderingShader, tank, mainTextRenderer, elapsedGameTime, isGameWon);
            pauseMenu.renderOverlay(textRenderingShader, mouseX, mouseY);

            processInput(window, *SoundEngine, tank, crosshair, turret, map, pauseMenu, deltaTime);

            // Check for level completion
            if (currentLevel == 1 && light1.isDestroyed && light2.isDestroyed && 
                light3.isDestroyed && light4.isDestroyed) {
                levelCompleteMenu.show(false); // Show "Level Complete" message
            }
            else if (currentLevel == 2 && tank2.isDestroyed && tank3.isDestroyed &&
                light1.isDestroyed && light2.isDestroyed &&
                light3.isDestroyed && light4.isDestroyed) {
                levelCompleteMenu.show(true); // Show "Game Complete" message
            }

            // Handle level complete menu
            if (levelCompleteMenu.isShown()) {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);

                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                    if (levelCompleteMenu.handleNextLevelClick(mouseX, mouseY)) {
                        currentLevel++;
                        setupLevel(currentLevel, tank, tank2, tank3, light1, light2, light3, light4, map);
                        levelCompleteMenu.hide();
                        gameStartTime = glfwGetTime(); // Reset timer for new level
                    }
                }
            }

            // Your existing rendering code...
            levelCompleteMenu.renderOverlay(textRenderingShader, mouseX, mouseY);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    SoundEngine->drop();
    glfwTerminate();
    return 0;
}

void TextRendering(unsigned int textRenderingShader, Tank& tank, TextRenderer& textRenderer, float elapsedGameTime, bool isGameWon) {
    float currentTime = glfwGetTime();
    textRenderer.renderText(textRenderingShader, "Srdjan Ilic RA31/2021", 1.18f, 1.92f, 0.8f, glm::vec3(0.2f, 1.0f, 0.2f));
    // RELOADING
    float remainingTime = SHOOT_COOLDOWN - (currentTime - tank.lastShotTime);
    std::ostringstream stream;
    stream << "Reloading: " << std::fixed << std::setprecision(2) << remainingTime;

    if (tank.ammunitionAP > 0 || tank.ammunitionHE > 0) {
        textRenderer.renderText(textRenderingShader, (tank.canShoot(currentTime)) ? "Ready to fire" : stream.str(),
            0.02f, 0.05f, 0.8f, (tank.canShoot(currentTime)) ? glm::vec3(0.2f, 1.0f, 0.2f) : glm::vec3(0.8, 0.0f, 0.0f));
        //textRenderer.renderText(textRenderingShader, "Ammunition: " + std::to_string(tank.ammunition), 1.35f, 0.02f, 0.8f, glm::vec3(0.2f, 1.0f, 0.2f));
    }
    else {
        textRenderer.renderText(textRenderingShader, "Ammunition empty", 0.02f, 0.05f, 0.8f, glm::vec3(0.8, 0.0f, 0.0f));
    }
    glm::vec3 apColor = (tank.currentProjectileType == ProjectileType::AP) ? glm::vec3(0.96f, 0.76f, 0.0f) : glm::vec3(0.2f, 1.0f, 0.2f);
    glm::vec3 heColor = (tank.currentProjectileType == ProjectileType::HE) ? glm::vec3(0.96f, 0.76f, 0.0f) : glm::vec3(0.2f, 1.0f, 0.2f);
    std::string apText = (tank.currentProjectileType == ProjectileType::AP) ? "*" : "";
    std::string heText = (tank.currentProjectileType == ProjectileType::HE) ? "*" : "";
    if (tank.ammunitionAP == 0) {
        apColor = glm::vec3(0.8, 0.0f, 0.0f);
        apText = "";
    }

    if (tank.ammunitionHE == 0) {
        heColor = glm::vec3(0.8, 0.0f, 0.0f);
        heText = "";
    }

    textRenderer.renderText(textRenderingShader, apText + std::to_string(tank.ammunitionAP) + apText, 1.5f, 0.05f, 0.8f, apColor);
    textRenderer.renderText(textRenderingShader, heText + std::to_string(tank.ammunitionHE) + heText, 1.8f, 0.05f, 0.8f, heColor);

    std::ostringstream timeStream;
    timeStream << "Time left: " << std::fixed << std::setprecision(2) << (GAME_TIME - elapsedGameTime) << "s";
    if(isGameWon)
        textRenderer.renderText(textRenderingShader, "YOU WON!", 0.02f, 1.92f, 0.8f, glm::vec3(0.2f, 1.0f, 0.2f));
    else
        textRenderer.renderText(textRenderingShader, ((GAME_TIME - elapsedGameTime) <= 0.0) ? "YOU LOST!" : timeStream.str() ,
        0.02f, 1.92f, 0.8f, ((GAME_TIME - elapsedGameTime) <= 0.0) ? glm::vec3(0.8, 0.0f, 0.0f) : glm::vec3(0.8, 0.0f, 0.0f));

}

void setupLevel(int level, Tank& tank, Tank& tank2, Tank& tank3,
    Tank& light1, Tank& light2, Tank& light3, Tank& light4, Map& map) {
    // Reset tank positions and health
    tank.reset(glm::vec2(0.0f, -0.7f), 0.0f);

    if (level == 1) {
        // Remove light tanks for level 1
        light1.reset(glm::vec2(0.6f, 0.4f), 0.0f); // Move off screen
        light2.reset(glm::vec2(-0.6f, 0.4f), 0.0f);
        light3.reset(glm::vec2(0.15f, 0.7f), 0.0f);
        light4.reset(glm::vec2(-0.15f, 0.7f), 0.0f);

        light1.setAngle(-90.0f); light2.setAngle(-90.0f); light3.setAngle(0.0f); light4.setAngle(180.0f);
    }
    else if (level == 2) {
        // Level 2 setup - moving targets and light tanks
        tank2.reset(glm::vec2(-0.7f, 0.7f), 180.0f);
        tank3.reset(glm::vec2(0.7f, 0.7f), 180.0f);

        // Add light tanks
        light1.reset(glm::vec2(0.5f, 0.4f), 0.0f);
        light2.reset(glm::vec2(0.7f, 0.4f), 0.0f);
        light3.reset(glm::vec2(-0.5f, 0.4f), 0.0f);
        light4.reset(glm::vec2(-0.7f, 0.4f), 0.0f);

        tank2.setAngle(-90.0f); tank3.setAngle(-90.0f);
        light1.setAngle(-90.0f); light2.setAngle(-90.0f); light3.setAngle(-90.0f); light4.setAngle(-90.0f);
    }

    // Reset map for new level
    map.clearAll();
    if (level == 1) {
        // Level 1 map setup
        map.setTile(5, 4, TileType::WALL); map.setTile(6, 4, TileType::WALL); map.setTile(7, 4, TileType::WALL);
        map.setTile(8, 4, TileType::WALL); map.setTile(9, 4, TileType::WALL);
        map.setTile(5, 10, TileType::WALL); map.setTile(6, 10, TileType::WALL); map.setTile(7, 10, TileType::WALL);
        map.setTile(8, 10, TileType::WALL); map.setTile(9, 10, TileType::WALL);
        map.placeBush(4, 5);  map.placeBush(5, 5); map.placeBush(6, 5); map.placeBush(7, 5);
        map.placeBush(8, 5); map.placeBush(9, 5); map.placeBush(10, 5);
        map.placeBush(4, 9); map.placeBush(5, 9); map.placeBush(6, 9); map.placeBush(7, 9);
        map.placeBush(8, 9); map.placeBush(9, 9); map.placeBush(10, 9);
    }
    else if (level == 2) {
        // Level 2 map setup - more complex
        map.setTile(5, 4, TileType::WALL); map.setTile(6, 4, TileType::WALL); map.setTile(7, 4, TileType::WALL);
        map.setTile(8, 4, TileType::WALL); map.setTile(9, 4, TileType::WALL);
        map.setTile(5, 10, TileType::WALL); map.setTile(6, 10, TileType::WALL); map.setTile(7, 10, TileType::WALL);
        map.setTile(8, 10, TileType::WALL); map.setTile(9, 10, TileType::WALL);
        map.placeBush(4, 5);  map.placeBush(5, 5); map.placeBush(6, 5); map.placeBush(7, 5);
        map.placeBush(8, 5); map.placeBush(9, 5); map.placeBush(10, 5);
        map.placeBush(4, 9); map.placeBush(5, 9); map.placeBush(6, 9); map.placeBush(7, 9);
        map.placeBush(8, 9); map.placeBush(9, 9); map.placeBush(10, 9);
    }
}
void processInput(GLFWwindow* window, ISoundEngine& SoundEngine, Tank& tank, Crosshair& crosshair, Turret& turret, Map& map,
                  PauseMenu& pauseMenu, float deltaTime) {
    float currentTime = glfwGetTime();

    static bool escapeWasPressed = false;
    static bool leftMousePressed = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!escapeWasPressed) {
            pauseMenu.togglePause();
            escapeWasPressed = true;
        }
    } else {
        escapeWasPressed = false;
    }
    // Proverite mouse click samo kada je igra pauzirana
    if (pauseMenu.isGamePaused()) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (!leftMousePressed) {
                if (pauseMenu.handleQuitClick(mouseX, mouseY)) {
                    glfwSetWindowShouldClose(window, true);
                }
                if (pauseMenu.handleResumeClick(mouseX, mouseY)) {
                    pauseMenu.togglePause();
                }
                leftMousePressed = true;
            }
        } else {
            leftMousePressed = false;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        tank.setAmmunitionAP(currentTime); // Postavi na AP
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        tank.setAmmunitionHE(currentTime); // Postavi na HE

    bool isMoving = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        tank.moveForward(deltaTime, map);
        isMoving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        tank.moveBackward(deltaTime, map);
        isMoving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        tank.rotateBodyLeft(deltaTime);
        isMoving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        tank.rotateBodyRight(deltaTime);
        isMoving = true;
    }
    if (isMoving) {
        if (tank.drivingSound == nullptr || tank.drivingSound->isFinished()) {
            //tank.drivingSound = SoundEngine.play2D("tankDriving.mp3", true, false, true);
        }
    }
    else {
        if (tank.drivingSound != nullptr) {
            tank.drivingSound->stop();
            tank.drivingSound->drop();
            tank.drivingSound = nullptr;
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!leftMousePressed) {
            tank.shoot(currentTime, SoundEngine);
            leftMousePressed = true;
        }
    } else {
        leftMousePressed = false;
    }

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    crosshair.setPosition(mouseX / WINDOW_WIDTH * 2.0f - 1.0f, 1.0f - mouseY / WINDOW_HEIGHT* 2.0f);
    tank.aimTurret(mouseX, mouseY, WINDOW_WIDTH, WINDOW_HEIGHT);
    turret.aimAtMouse(mouseX, mouseY, WINDOW_WIDTH, WINDOW_HEIGHT);
}