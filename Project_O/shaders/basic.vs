#version 450 core

layout(location = 0) in vec3 aPos;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main()
{
	mat4 mvp = uModel * uView * uProjection;
	gl_Position = vec4(aPos, 1.0) * mvp;
}