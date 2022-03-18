#pragma once

#include <string>

#include <glad/glad.h>

#include "mathematics.hpp"

namespace Resources
{
	class Shader
	{
	private:
		GLuint program = 0;

	public:
		/**
		 * Create a shader with a shader files (.vs and .fs).
		 * 
		 * @param shaderName
		 */
		Shader(const char* shaderName);

		/**
		 * Destroy the shader.
		 * 
		 */
		~Shader();

		/**
		 * Use shader program.
		 */
		void use() const;

		/**
		 * Pass bool to shader as uniform.
		 */
		void uniformBool(const char* uniform, const bool& b) const;

		/**
		 * Pass float to shader as uniform.
		 */
		void uniformFloat(const char* uniform, const float& f) const;

		/**
		 * Pass vec3 to shader as uniform.
		 */
		void uniformVec3(const char* uniform, const vec3& v) const;

		/**
		 * Pass mat4 to shader as uniform.
		 */
		void uniformMat4(const char* uniform, const mat4& m, const bool transpose) const;
	};
}