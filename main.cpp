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
#include "TextRenderer.h"

#include <thread>
#include <chrono>

const unsigned int WINDOW_WIDTH = 900;
const unsigned int WINDOW_HEIGHT= 900;
const float TARGET_FPS = 120.0f;
const float FRAME_TIME = 1.0f / TARGET_FPS;
const float GAME_TIME = 20.0f;

void processInput(GLFWwindow* window, Tank& tank, Crosshair& crosshair, Turret& turret, Map& map, 
                  PauseMenu& pauseMenu, float deltaTime);
void TextRendering(unsigned int textRenderingShader, Tank& tank, TextRenderer& textRenderer, float elapsedGameTime, bool isGameWon);
void CheckProjectileTankHit(Tank& tank, Projectile& projectile);

int main(void)
{
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

    //Shader shader("textShader.vert", "textShader.frag");
    unsigned int textRenderingShader = createShaderFromFile("textShader.vert", "textShader.frag");
    //Shader pauseMenuShader("pauseMenu.vert", "pauseMenu.frag
    //unsigned int pauseMenuShader = createShaderFromFile("pauseMenu.vert", "pauseMenu.frag");
    TextRenderer mainTextRenderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    mainTextRenderer.init("BRLNSR.TTF", 48);
    TextRenderer pauseMenuTextRenderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    pauseMenuTextRenderer.init("BRLNSR.TTF", 48);
    //Background background;
    Tank tank(glm::vec2(0.0f, -0.7f), 0.3f, 60.0f, "tank.png"); // Početna pozicija, brzina, rotaciona brzina
    Turret turret(glm::vec2(0.0f, 0.0f), "turret.png");
    tank.setTurret(&turret);
    Tank tank2(glm::vec2(-0.7f, 0.7f), 0.001f, 0.2f, "tank2.png"); // Početna pozicija, brzina, rotaciona brzina
    Turret turret2(glm::vec2(0.0f, 0.0f), "turret2.png");
    tank2.setTurret(&turret2);
    Tank tank3(glm::vec2(0.7f, 0.7f), 0.001f, 0.2f, "tank2.png"); // Početna pozicija, brzina, rotaciona brzina
    Turret turret3(glm::vec2(0.0f, 0.0f), "turret2.png");
    tank3.setTurret(&turret3);

    Crosshair crosshair; 
    Map map;
    map.setTile(5, 4, TileType::WALL);
    map.setTile(6, 4, TileType::WALL);
    map.setTile(7, 4, TileType::WALL);
    map.setTile(8, 4, TileType::WALL);
    map.setTile(9, 4, TileType::WALL);
    map.setTile(5, 11, TileType::WALL);
    map.setTile(6, 11, TileType::WALL);
    map.setTile(7, 11, TileType::WALL);
    map.setTile(8, 11, TileType::WALL);
    map.setTile(9, 11, TileType::WALL);
    map.placeBush(4, 5);
    map.placeBush(5, 5);
    map.placeBush(6, 5);
    map.placeBush(7, 5);
    map.placeBush(8, 5);
    map.placeBush(9, 5);
    map.placeBush(10, 5);
    map.placeBush(4, 10);
    map.placeBush(5, 10);
    map.placeBush(6, 10);
    map.placeBush(7, 10);
    map.placeBush(8, 10);
    map.placeBush(9, 10);
    map.placeBush(10, 10);
    PauseMenu pauseMenu(WINDOW_WIDTH, WINDOW_HEIGHT, pauseMenuTextRenderer);

    float gameStartTime = glfwGetTime();
    bool isGameWon = false;
    float previousTime = 0.0f;
    float deltaTime = 0.0f;
    while (!glfwWindowShouldClose(window)) 
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        float currentTime = glfwGetTime();
        float elapsedGameTime = currentTime - gameStartTime;
        float frameTime = currentTime - previousTime;
        /*if (frameTime < FRAME_TIME) { //ako je frame prebrz, pauziramo program da ispostujem 60fpsa po sekundi, ako nije samo nastavi
            float sleepTime = FRAME_TIME - frameTime;
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));//cekamo dok se ne dodje do zeljenog trajanja frejma
            currentTime = glfwGetTime();
            frameTime = currentTime - previousTime;
        }*/
        //deltaTime = frameTime;
        //previousTime = currentTime;
        if (frameTime >= FRAME_TIME) {
            deltaTime = frameTime;
            std::cout << deltaTime << std::endl;
            currentTime = glfwGetTime();
            previousTime = currentTime;


            // da li su svi tenkovi unisteni
            if (tank2.isDestroyed && tank3.isDestroyed && ((GAME_TIME - elapsedGameTime) > 0.0)) {
                isGameWon = true;
            }

            glClearColor(0.5, 0.5, 0.5, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);

            map.render();
            tank.render();
            turret.render();
            if (!tank2.isDestroyed) tank2.render();
            if (!tank2.isDestroyed) turret2.render();
            if (!tank3.isDestroyed) tank3.render();
            if (!tank3.isDestroyed) turret3.render();
            crosshair.render(tank, currentTime);
            TextRendering(textRenderingShader, tank, mainTextRenderer, elapsedGameTime, isGameWon);
            pauseMenu.renderOverlay(textRenderingShader, mouseX, mouseY);
            for (auto* projectile : tank.projectiles) {
                projectile->update(deltaTime, map);
                CheckProjectileTankHit(tank2, *projectile);
                CheckProjectileTankHit(tank3, *projectile);

                projectile->render();
            }
            for (auto it = tank.projectiles.begin(); it != tank.projectiles.end();) {
                if ((*it)->hasHitTarget() || !(*it)->isActive()) {
                    glm::vec2 explosionPos = (*it)->getPosition();
                    tank.explosions.push_back(new Explosion(explosionPos, FRAME_TIME));
                    delete* it;
                    it = tank.projectiles.erase(it);
                }
                else {
                    ++it;
                }
            }
            for (auto* explosion : tank.explosions) {
                explosion->update(deltaTime);
                explosion->render();
            }
            for (auto it = tank.explosions.begin(); it != tank.explosions.end();) {
                if ((*it)->isFinished()) {
                    delete* it;
                    it = tank.explosions.erase(it);
                }
                else {
                    ++it;
                }
            }

            processInput(window, tank, crosshair, turret, map, pauseMenu, deltaTime);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    tank.explosions.clear();
    tank.projectiles.clear();
    glfwTerminate();
    return 0;
}

void CheckProjectileTankHit(Tank& tank, Projectile& projectile) {
    float distance = glm::distance(projectile.getPosition(), tank.position); //euklidska distanca izmedju projektila i tenka
    if (distance < 0.1f) {  // Adjust collision radius as needed
        //tank.position = glm::vec2(-0.7f, -0.7f);
        //turret2.position = glm::vec2(-0.7f, -0.7f);
        tank.isDestroyed = true;
        projectile.hitTarget = true;
        projectile.active = false;
    }
}

void TextRendering(unsigned int textRenderingShader, Tank& tank, TextRenderer& textRenderer, float elapsedGameTime, bool isGameWon) {
    float currentTime = glfwGetTime();
    textRenderer.renderText(textRenderingShader, "Srdjan Ilic RA31/2021", 1.18f, 1.92f, 0.8f, glm::vec3(0.2f, 1.0f, 0.2f));
    // RELOADING
    float remainingTime = SHOOT_COOLDOWN - (currentTime - tank.lastShotTime);
    std::ostringstream stream;
    stream << "Reloading: " << std::fixed << std::setprecision(2) << remainingTime;

    if (tank.ammunition > 0) {
        textRenderer.renderText(textRenderingShader, (tank.canShoot(currentTime)) ? "Ready to fire" : stream.str(),
            0.02f, 0.02f, 0.8f, (tank.canShoot(currentTime)) ? glm::vec3(0.2f, 1.0f, 0.2f) : glm::vec3(0.8, 0.0f, 0.0f));
        textRenderer.renderText(textRenderingShader, "Ammunition: " + std::to_string(tank.ammunition), 1.35f, 0.02f, 0.8f, glm::vec3(0.2f, 1.0f, 0.2f));
    }
    else {
        textRenderer.renderText(textRenderingShader, "Ammunition empty", 1.18f, 0.02f, 0.8f, glm::vec3(0.8, 0.0f, 0.0f));
    }

    std::ostringstream timeStream;
    timeStream << "Time left: " << std::fixed << std::setprecision(2) << (GAME_TIME - elapsedGameTime) << "s";
    if(isGameWon)
        textRenderer.renderText(textRenderingShader, "YOU WON!", 0.02f, 1.92f, 0.8f, glm::vec3(0.2f, 1.0f, 0.2f));
    else
        textRenderer.renderText(textRenderingShader, ((GAME_TIME - elapsedGameTime) <= 0.0) ? "YOU LOST!" : timeStream.str() ,
        0.02f, 1.92f, 0.8f, ((GAME_TIME - elapsedGameTime) <= 0.0) ? glm::vec3(0.8, 0.0f, 0.0f) : glm::vec3(0.8, 0.0f, 0.0f));

}
void processInput(GLFWwindow* window, Tank& tank, Crosshair& crosshair, Turret& turret, Map& map, 
                  PauseMenu& pauseMenu, float deltaTime) {
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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) tank.moveForward(deltaTime, map);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) tank.moveBackward(deltaTime, map);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) tank.rotateBodyLeft(deltaTime, map);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) tank.rotateBodyRight(deltaTime, map); 

    float currentTime = glfwGetTime();
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!leftMousePressed) {
            tank.shoot(currentTime);
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