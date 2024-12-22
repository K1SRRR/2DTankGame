#include "Projectile.h"
#include "globalFunctions.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Projectile::Projectile(glm::vec2 startPosition, float angle, float speed, ProjectileType type, float size)
    : position(startPosition), angle(angle + 90.0f), speed(speed),
    active(true), hitTarget(false), texture(0), type(type) {
       
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec2 inPos;
        layout(location = 1) in vec2 inTex;
        uniform mat4 uModel;
        out vec2 chTex;
        void main() {
            gl_Position = uModel * vec4(inPos, 0.0, 1.0);
            chTex = inTex;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 chTex;
        out vec4 outCol;
        uniform sampler2D uTex;
        void main() {
            outCol = texture(uTex, chTex);
        }
    )";

    shader = createShader(vertexShaderSource, fragmentShaderSource);

    float vertices[] = {
        -0.025f * size, -0.025f * size,  1.0f, 0.0f,
        -0.025f * size,  0.025f * size,  0.0f, 0.0f,
         0.025f * size, -0.025f * size,  1.0f, 1.0f,
         0.025f * size,  0.025f * size,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    if (type == ProjectileType::HE) {
        this->speed *= 0.66f; // HE je za trećinu sporiji
        texture = loadImageToTexture("HE-shell.png");
    } 
    else 
    {
        texture = loadImageToTexture("AP-shell.png");
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    
}

void Projectile::update(float deltaTime, Map& map) {
    if (!active) return;

    position.x += cos(glm::radians(angle)) * speed * deltaTime;
    position.y += sin(glm::radians(angle)) * speed * deltaTime;

    checkMapCollision(map);
}

void Projectile::checkMapCollision(Map& map) {
    if (map.checkCollision(position, 0.02f)) {
        hitTarget = true;
        active = false;
    }
}
void Projectile::render() {
    if (!active) return;

    glUseProgram(shader);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

    unsigned int uModelLoc = glGetUniformLocation(shader, "uModel");
    glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}