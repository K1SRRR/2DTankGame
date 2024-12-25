// LevelCompleteMenu.cpp
#include "LevelCompleteMenu.h"
#include "globalFunctions.h"
#include <glm/gtc/type_ptr.hpp>

LevelCompleteMenu::LevelCompleteMenu(unsigned int windowWidth1, unsigned int windowHeight1, TextRenderer& textRenderer1) :
    isVisible(false), isLastLevelComplete(false), VAO(0), VBO(0),
    windowWidth(windowWidth1), windowHeight(windowHeight1), textRenderer(textRenderer1)
{
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 inPos;
        void main()
        {
            gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec4 color;
        void main()
        {
            FragColor = color;
        }
    )";

    unifiedShader = createShader(vertexShaderSource, fragmentShaderSource);
    float overlayVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f,
    };

    nextLevelButton.x = windowWidth / 2.0f - 130;
    nextLevelButton.y = windowHeight / 2.0f - 25;
    nextLevelButton.width = 260;
    nextLevelButton.height = 50;
    nextLevelButton.isHovered = false;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(overlayVertices), overlayVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

LevelCompleteMenu::~LevelCompleteMenu() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(unifiedShader);
}

void LevelCompleteMenu::show(bool isLastLevel) {
    isVisible = true;
    isLastLevelComplete = isLastLevel;
}

void LevelCompleteMenu::hide() {
    isVisible = false;
}

bool LevelCompleteMenu::isShown() const {
    return isVisible;
}

void LevelCompleteMenu::renderOverlay(unsigned int textRenderingShader, double mouseX, double mouseY) {
    if (!isVisible) return;
    nextLevelButton.isHovered = nextLevelButton.isMouseOver(mouseX, mouseY);

    glUseProgram(unifiedShader);

    unsigned int uColor = glGetUniformLocation(unifiedShader, "color");
    glUniform4f(uColor, 0.0f, 0.0f, 0.0f, 0.7f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Render completion message
    float messageX = windowWidth / 2.0f - 200;
    float messageY = windowHeight / 2.0f + 50;
    float normalizedMessageX = 2.0f * (messageX / windowWidth);
    float normalizedMessageY = 2.0f * (messageY / windowHeight);
    
    if (isLastLevelComplete) {
        textRenderer.renderText(textRenderingShader, "Congratulations!", 
            normalizedMessageX + 0.06f, normalizedMessageY, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        textRenderer.renderText(textRenderingShader, "You've completed the game!",
            normalizedMessageX - 0.2f, normalizedMessageY - 0.15f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    } else {
        textRenderer.renderText(textRenderingShader, "Level Complete!", 
            normalizedMessageX + 0.08f, normalizedMessageY, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    renderButtons(textRenderingShader);
    glBindVertexArray(0);
    glUseProgram(0);
}

void LevelCompleteMenu::renderButtons(unsigned int textRenderingShader) {
    if (isLastLevelComplete) return; // Don't show next level button on last level

    float nextLevelNormalizedX = 2.0f * (nextLevelButton.x / windowWidth);
    float nextLevelNormalizedY = 2.0f * (nextLevelButton.y / windowHeight);

    textRenderer.renderText(textRenderingShader, "NEXT LEVEL", nextLevelNormalizedX, nextLevelNormalizedY, 1.0f,
        nextLevelButton.isHovered ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 0.0f));
}

bool LevelCompleteMenu::handleNextLevelClick(double mouseX, double mouseY) {
    if (!isVisible || isLastLevelComplete) return false;
    nextLevelButton.isHovered = nextLevelButton.isMouseOver(mouseX, mouseY);
    return nextLevelButton.isHovered;
}

bool LevelCompleteMenu::Button::isMouseOver(double mouseX, double mouseY) {
    return (mouseX >= x && mouseX <= x + width &&
        mouseY >= y && mouseY <= y + height);
}