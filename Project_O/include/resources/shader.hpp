#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

#include "math/math.hpp"

namespace Resources
{
	class Shader
	{
	private:
		GLuint program = 0;

	public:
		Shader(const char* shaderName);
		~Shader();

		void use() const; //use shader program

		void uniformBool(const bool& b, const char* name);		//pass bool uniform to shader
		void uniformFloat(const float& f, const char* name);	//pass float uniform to shader
		void uniformVec3(const vec3& v, const char* name);		//pass vec3 uniform to shader
		void uniformMat4(const mat4& m, const bool transpose, const char* name);	//pass mat4 uniform to shader
	};
}

using namespace Resources;