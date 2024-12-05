#include "Tank.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "globalFunctions.h"

// Konstruktor
Tank::Tank(glm::vec2 initPosition, float initBodyAngle, float initSpeed, float initRotationSpeed, const char* textureSourceFile)
    : position(initPosition), bodyAngle(initBodyAngle+90.0f), turretAngle(0.0f),
    speed(initSpeed), rotationSpeed(initRotationSpeed), texture(0) 
{
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec2 inPos;
        layout(location = 1) in vec2 inTex;
        out vec2 chTex;
        uniform mat4 uModel;
        void main()
        {
	        gl_Position = uModel * vec4(inPos, 0.0, 1.0);
	        chTex = inTex;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 chTex;
        out vec4 outCol;
        uniform sampler2D uTex;
        void main()
        {
	        outCol = texture(uTex, chTex);
        }
    )";
    unifiedShader = createShader(vertexShaderSource, fragmentShaderSource);

    float vertices[] =
    {   //X    Y      S    T 
    -0.08f, -0.08f,  1.0f, 0.0f,  // prvo tjeme
    -0.08f,  0.08f,  0.0f, 0.0f,  // drugo tjeme
     0.08f, -0.08f,  1.0f, 1.0f,  // trece tjeme
     0.08f,  0.08f,  0.0f, 1.0f   // cetvrto tjeme
    };
    unsigned int stride = (2 + 2) * sizeof(float);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    texture = loadImageToTexture(textureSourceFile); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, texture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(unifiedShader);
    unsigned uTexLoc = glGetUniformLocation(unifiedShader, "uTex");
    glUniform1i(uTexLoc, 0);
    glUseProgram(0);
}

void Tank::render() {
    if (isDestroyed) return;

    unsigned int stride = (2 + 2) * sizeof(float);
    glm::mat4 model = getModelMatrix();

    glUseProgram(unifiedShader);
    unsigned uModelLoc = glGetUniformLocation(unifiedShader, "uModel");
    glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Tank::moveForward(float deltaTime, Map& map) {
    glm::vec2 nextPosition = position;
    nextPosition.x += cos(glm::radians(bodyAngle)) * speed * deltaTime;
    nextPosition.y += sin(glm::radians(bodyAngle)) * speed * deltaTime;
    //std::cout << "nextPosition.x" << nextPosition.x << std::endl;
    //std::cout << "nextPosition.y" << nextPosition.y << std::endl;
    
    //koordinate koje predstavljaju položaj tenka na koordinatnom sistemu mape
    int mapX = static_cast<int>(position.x * map.MATRIX_WIDTH / 2.0f + map.MATRIX_WIDTH / 2.0f);
    int mapY = static_cast<int>(position.y * map.MATRIX_HEIGHT / 2.0f + map.MATRIX_HEIGHT / 2.0f);

    if (map.hasBush(mapX, mapY)) {
        map.removeBush(mapX, mapY);
    }
    if (!map.checkCollision(nextPosition, 0.06f)) {
        position = nextPosition;
        if (turret) {
            turret->position = position;
        }
    }
}

void Tank::moveBackward(float deltaTime, Map& map) {
    glm::vec2 nextPosition = position;
    nextPosition.x -= cos(glm::radians(bodyAngle)) * speed * deltaTime;
    nextPosition.y -= sin(glm::radians(bodyAngle)) * speed * deltaTime;

    //koordinate koje predstavljaju položaj tenka na koordinatnom sistemu mape
    int mapX = static_cast<int>(position.x * map.MATRIX_WIDTH / 2.0f + map.MATRIX_WIDTH / 2.0f);
    int mapY = static_cast<int>(position.y * map.MATRIX_HEIGHT / 2.0f + map.MATRIX_HEIGHT / 2.0f);

    if (map.hasBush(mapX, mapY)) {
        map.removeBush(mapX, mapY);
    }
    if (!map.checkCollision(nextPosition, 0.06f)) {
        position = nextPosition;
        if (turret) {
            turret->position = position;
        }
    }
}

void Tank::rotateBodyLeft(float deltaTime) {
    bodyAngle += rotationSpeed * deltaTime;
}

void Tank::rotateBodyRight(float deltaTime) {
    bodyAngle -= rotationSpeed * deltaTime;
}

void Tank::aimTurret(float mouseX, float mouseY, int windowWidth, int windowHeight) {
    float normalizedX = (mouseX / windowWidth) * 2.0f - 1.0f;
    float normalizedY = 1.0f - (mouseY / windowHeight) * 2.0f;

    // Izračunavanje ugla prema poziciji miša (ugao između tačke (x, y) i pozitivne X ose u koordinatnom sistemu)
    turretAngle = glm::degrees(atan2(normalizedY - position.y, normalizedX - position.x)) - 90.0f;
}

glm::mat4 Tank::getModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f)); // Translacija tenka do (0,0) da rotira oko svog centra
    model = glm::rotate(model, glm::radians(bodyAngle), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotacija tela tenka (oko tačke (0,0))
    return model;
}

bool Tank::canShoot(float currentTime) const {
    bool canShoot = ammunition > 0 && (currentTime - lastShotTime >= SHOOT_COOLDOWN);
    //std::cout << "CAN SHOOT: " << canShoot << std::endl;
    return canShoot;
}

void Tank::shoot(float currentTime, ISoundEngine& SoundEngine) {
    if (canShoot(currentTime)) {
        glm::vec2 projectileStart = position; // startna pozicija projektila u odnosu na kupolu
        projectileStart.x += cos(glm::radians(turretAngle+90.0f)) * 0.2f;
        projectileStart.y += sin(glm::radians(turretAngle+90.0f)) * 0.2f;

        Projectile* newProjectile = new Projectile(projectileStart, turretAngle, projectileSpeed);
        projectiles.push_back(newProjectile);

        ammunition--;
        lastShotTime = currentTime;
        SoundEngine.play2D("tankFire.mp3", false);
        SoundEngine.play2D("reload.mp3", false);
    }
}

void Tank::setTurret(Turret* tankTurret) {
    turret = tankTurret;
    if (turret) {
        turret->position = position;
    }
}
Turret Tank::getTurret() {
    return *turret;
}