#include "PauseMenu.h"
#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "globalFunctions.h"

PauseMenu::PauseMenu(unsigned int windowWidth1, unsigned int windowHeight1, TextRenderer& textRenderer1) :
    isPaused(false), VAO(0), VBO(0),
    windowWidth(windowWidth1), windowHeight(windowHeight1), textRenderer(textRenderer1)
{
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 inPos; // <vec2 pos, vec2 tex>

        void main()
        {
            gl_Position = vec2(inPos.x, inPos.y);
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
        -1.0f, -1.0f, // Donji levi ugao
        1.0f, -1.0f,  // Donji desni ugao
        1.0f, 1.0f,  // Gornji desni ugao
        -1.0f, 1.0f,  // Gornji levi ugao
    };

    // Postavljanje quit dugmeta
    quitButton.x = windowWidth / 4.0f - 50;
    quitButton.y = windowHeight / 2.0f - 25;
    quitButton.width = 100;
    quitButton.height = 50;
    quitButton.isHovered = false;

    resumeButton.x = windowWidth / (4.0f/3.0f) - 80;
    resumeButton.y = windowHeight / 2.0f - 25;
    resumeButton.width = 160;
    resumeButton.height = 50;
    resumeButton.isHovered = false;

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

PauseMenu::~PauseMenu() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(unifiedShader);
}

void PauseMenu::togglePause() {
    isPaused = !isPaused;
}

bool PauseMenu::isGamePaused() const {
    return isPaused;
}

void PauseMenu::renderOverlay(unsigned int textRenderingShader, double mouseX, double mouseY) {
    if (!isPaused) return;
    quitButton.isHovered = quitButton.isMouseOver(mouseX, mouseY);
    resumeButton.isHovered = resumeButton.isMouseOver(mouseX, mouseY);

    glUseProgram(unifiedShader);

    unsigned uColor = glGetUniformLocation(unifiedShader, "color"); //poluprovidni overlay
    glUniform4f(uColor, 0.0f, 0.0f, 0.0f, 0.7f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    renderButtons(textRenderingShader);
    glBindVertexArray(0);
    glUseProgram(0);
}

void PauseMenu::renderButtons(unsigned int textRenderingShader) {
    float quitNormalizedX = 2.0f * (quitButton.x / windowWidth); // namestamo koordinate za text rendering koji prima od 0 do 2 (gde je levi donji cosak 0,0)
    float quitNormalizedY = 2.0f * (quitButton.y / windowHeight);
    float resumeNormalizedX = 2.0f * (resumeButton.x / windowWidth);
    float resumeNormalizedY = 2.0f * (resumeButton.y / windowHeight);

    textRenderer.renderText(textRenderingShader, "QUIT", quitNormalizedX, quitNormalizedY, 1.0f,
        quitButton.isHovered ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 0.0f));
    textRenderer.renderText(textRenderingShader, "RESUME", resumeNormalizedX, resumeNormalizedY, 1.0f,
        resumeButton.isHovered ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 0.0f));
}

bool PauseMenu::handleQuitClick(double mouseX, double mouseY) {
    if (!isPaused) return false;
    quitButton.isHovered = quitButton.isMouseOver(mouseX, mouseY);
    return quitButton.isHovered;
}
bool PauseMenu::handleResumeClick(double mouseX, double mouseY) {
    if (!isPaused) return false;
    resumeButton.isHovered = resumeButton.isMouseOver(mouseX, mouseY);
    return resumeButton.isHovered;
}

bool PauseMenu::Button::isMouseOver(double mouseX, double mouseY) {
    return (mouseX >= x && mouseX <= x + width &&
        mouseY >= y && mouseY <= y + height);
}