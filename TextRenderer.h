#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    unsigned int TextureID;
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};

class TextRenderer {
public:
    TextRenderer(unsigned int windowWidth, unsigned int windowHeight);
    ~TextRenderer();

    void init(const std::string& fontPath, unsigned int fontSize);
    void renderText(unsigned int unifiedShader, const std::string& text, float x, float y,
        float scale, const glm::vec3& color);

private:
    void loadCharacters(FT_Face face);
    void setupBuffers();
    void cleanupBuffers();

    unsigned int VAO, VBO;
    std::map<char, Character> Characters;
    glm::mat4 projection;
    unsigned int windowWidth;
    unsigned int windowHeight;
};

#endif // TEXT_RENDERER_H