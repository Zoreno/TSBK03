#pragma once

#include <string>
#include <glm/glm.hpp>
#include <vector>

#include "GLSLShaderMethodNotImplementedException.h"
#include "GLSLShaderCompilationException.h"
#include "Color.h"

using GLSLTessellationControlShaderHandle = unsigned int;
using GLSLTessellationEvaluationShaderHandle = unsigned int;
using GLSLGeometryShaderHandle = unsigned int;

using GLSLShaderProgramHandle = unsigned int;

class GLSLShader
{
public:
	GLSLShader();
	
	GLSLShader(const GLSLShader& other) = delete;
	GLSLShader(GLSLShader&& other) = delete;

	GLSLShader& operator=(const GLSLShader& other) = delete;
	GLSLShader& operator=(GLSLShader&& other) = delete;

	~GLSLShader();

	void setVertexShaderSource(const std::string& source);
	void setTessellationControlSource(const std::string& source);
	void setTessellationEvaluationSource(const std::string& source);
	void setGeometryShaderSource(const std::string& source);
	void setFragmentShaderSource(const std::string& source);

	void compile();

	void use() const;

	static void use(unsigned int program);

	GLSLShaderProgramHandle getProgramHandle() const;

	void uploadUniform(
		const std::string &name,
		const float &value);

	void uploadUniform(
		const std::string &name,
		const int &value);

	void uploadUniform(
		const std::string &name,
		const glm::vec2 &value);

	void uploadUniform(
		const std::string &name,
		const glm::vec3 &value);

	void uploadUniform(
		const std::string &name,
		const glm::vec4 &value);

	void uploadUniform(
		const std::string &name,
		const glm::mat2 &value);

	void uploadUniform(
		const std::string &name,
		const glm::mat3 &value);

	void uploadUniform(
		const std::string &name,
		const glm::mat4 &value);

	void uploadUniform(
		const std::string& name,
		const Color& color);

	template <typename TYPE>
	void uploadUniformArray(
		const std::string &name,
		unsigned int count,
		const std::vector<TYPE> &value);
private:
	std::string _vertexShaderSource{};
	std::string _tessellationControlSource{};
	std::string _tessellationEvaluationSource{};
	std::string _geometryShaderSource{};
	std::string _fragmentShaderSource{};

	GLSLShaderProgramHandle _programId{ 0 };

	std::string _infoLog{};
};

template <typename TYPE>
void GLSLShader::uploadUniformArray(
	const std::string &name,
	unsigned int count,
	const std::vector<TYPE>& value)
{
	for (unsigned int i = 0; i < count; ++i)
	{
		uploadUniform(
			name + std::string{ "[" } + std::to_string(i) + std::string{ "]" },
			value[i]);
	}
}
