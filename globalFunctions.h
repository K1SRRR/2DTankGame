#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include "stb_image.h"

// Funkcija za kompajliranje šejdera
unsigned int compileShaderFromFile(GLenum type, const char* source);
unsigned int compileShader(GLenum type, const char* source);

// Funkcija za kreiranje objedinjene šejder programa
unsigned int createShaderFromFile(const char* vsSource, const char* fsSource);
unsigned int createShader(const char* vsSource, const char* fsSource);

// Funkcija za učitavanje slike u teksturu
unsigned int loadImageToTexture(const char* filePath);

#endif // GLOBAL_FUNCTIONS_H
