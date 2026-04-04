#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aUV;

layout(location = 0) out vec3 vertexColor;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	vertexColor = aColor.xyz;
}