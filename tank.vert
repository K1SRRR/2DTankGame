#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;
out vec2 chTex;
uniform vec2 uPos;
uniform mat4 uModel;
void main()
{
	gl_Position = uModel * vec4(inPos + uPos, 0.0, 1.0);
	chTex = inTex;
}