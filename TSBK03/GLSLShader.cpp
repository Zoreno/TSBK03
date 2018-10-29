#include "GLSLShader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Utils.h"

#include <glm/gtc/type_ptr.hpp>

#include "GLSLVertexShader.h"
#include "GLSLFragmentShader.h"
#include <iostream>
#include "GLSLGeometryShader.h"
#include "GLSLTessEvalShader.h"
#include "GLSLTessControlShader.h"

GLSLShader::GLSLShader()
{

}

GLSLShader::~GLSLShader()
{
	if (glIsProgram(_programId))
	{
		glDeleteProgram(_programId);
	}
}

void GLSLShader::setVertexShaderSource(
	const std::string &source)
{
	_vertexShaderSource = source;
}

void GLSLShader::setTessellationControlSource(
	const std::string &source)
{
	_tessellationControlSource = source;
}

void GLSLShader::setTessellationEvaluationSource(
	const std::string &source)
{
	_tessellationEvaluationSource = source;
}

void GLSLShader::setGeometryShaderSource(
	const std::string &source)
{
	_geometryShaderSource = source;
}

void GLSLShader::setFragmentShaderSource(
	const std::string &source)
{
	_fragmentShaderSource = source;
}

void GLSLShader::compile()
{
	if (glIsProgram(_programId))
	{
		glDeleteShader(_programId);
	}

	_programId = glCreateProgram();

	GLSLVertexShader vertexShader{ _vertexShaderSource };
	GLSLFragmentShader fragmentShader{ _fragmentShaderSource };
	GLSLGeometryShader geometryShader{ _geometryShaderSource };
	GLSLTessEvalShader tessEvalShader{ _tessellationEvaluationSource };
	GLSLTessControlShader tessControlShader{ _tessellationControlSource };

	if (!_vertexShaderSource.empty())
	{
		int success = vertexShader.compile();

		if (!success)
		{
			throw GLSLShaderCompilationException{ std::string{ "[SHADER][" + _vertexShaderSource + "]Vertex Shader Compilation Error: " } +vertexShader.getInfoLog() };
		}

		glAttachShader(_programId, vertexShader.getHandle());
	}

	if (!_fragmentShaderSource.empty())
	{
		int success = fragmentShader.compile();

		if (!success)
		{
			throw GLSLShaderCompilationException{ std::string{ "[SHADER][" + _fragmentShaderSource+ "] Fragment Shader Compilation Error: " } +fragmentShader.getInfoLog() };
		}

		glAttachShader(_programId, fragmentShader.getHandle());
	}

	if (!_geometryShaderSource.empty())
	{
		int success = geometryShader.compile();

		if (!success)
		{
			throw GLSLShaderCompilationException{ std::string{ "[SHADER][" + _geometryShaderSource + "] Geometry Shader Compilation Error: " } +geometryShader.getInfoLog() };
		}

		glAttachShader(_programId, geometryShader.getHandle());
	}

	if (!_tessellationEvaluationSource.empty())
	{
		int success = tessEvalShader.compile();

		if (!success)
		{
			throw GLSLShaderCompilationException{ std::string{ "[SHADER][" + _tessellationEvaluationSource + "] Fragment Shader Compilation Error: " } +tessEvalShader.getInfoLog() };
		}

		glAttachShader(_programId, tessEvalShader.getHandle());
	}

	if (!_tessellationControlSource.empty())
	{
		int success = tessControlShader.compile();

		if (!success)
		{
			throw GLSLShaderCompilationException{ std::string{ "[SHADER][" + _tessellationControlSource + "] Fragment Shader Compilation Error: " } +tessControlShader.getInfoLog() };
		}

		glAttachShader(_programId, tessControlShader.getHandle());
	}

	glLinkProgram(_programId);

	GLint success;
	glGetProgramiv(_programId, GL_LINK_STATUS, &success);

	if (!success)
	{
		GLchar *infoLog;
		GLint infoLogSize;
		glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &infoLogSize);

		infoLog = static_cast<GLchar *>(malloc(infoLogSize));

		glGetProgramInfoLog(_programId, infoLogSize, NULL, infoLog);

		_infoLog = std::string{ infoLog };

		free(infoLog);

		throw GLSLShaderCompilationException{ std::string{"[SHADER] Link Error: "} +_infoLog };
	}
}

void GLSLShader::use() const
{
	glUseProgram(_programId);
}

void GLSLShader::use(
	unsigned int program)
{
	glUseProgram(program);
}

GLSLShaderProgramHandle GLSLShader::getProgramHandle() const
{
	return _programId;
}

void GLSLShader::uploadUniform(
	const std::string &name,
	const float &value)
{
	glUniform1f(
		glGetUniformLocation(_programId, name.c_str()),
		value);
}

void GLSLShader::uploadUniform(
	const std::string &name,
	const int &value)
{
	glUniform1i(
		glGetUniformLocation(_programId, name.c_str()),
		value);
}

void GLSLShader::uploadUniform(
	const std::string &name,
	const glm::vec2 &value)
{
	glUniform2fv(
		glGetUniformLocation(_programId, name.c_str()),
		1,
		glm::value_ptr(value));
}

void GLSLShader::uploadUniform(
	const std::string &name,
	const glm::vec3 &value)
{
	glUniform3fv(
		glGetUniformLocation(_programId, name.c_str()),
		1,
		glm::value_ptr(value));
}

void GLSLShader::uploadUniform(
	const std::string &name,
	const glm::vec4 &value)
{
	glUniform4fv(
		glGetUniformLocation(_programId, name.c_str()),
		1,
		glm::value_ptr(value));
}

void GLSLShader::uploadUniform(
	const std::string &name,
	const glm::mat2 &value)
{
	glUniformMatrix2fv(
		glGetUniformLocation(_programId, name.c_str()),
		1,
		GL_FALSE,
		glm::value_ptr(value));
}

void GLSLShader::uploadUniform(
	const std::string &name,
	const glm::mat3 &value)
{
	glUniformMatrix3fv(
		glGetUniformLocation(_programId, name.c_str()),
		1,
		GL_FALSE,
		glm::value_ptr(value));
}

void GLSLShader::uploadUniform(
	const std::string &name,
	const glm::mat4 &value)
{
	glUniformMatrix4fv(
		glGetUniformLocation(_programId, name.c_str()),
		1,
		GL_FALSE,
		glm::value_ptr(value));
}

void GLSLShader::uploadUniform(
	const std::string &name,
	const Color &color)
{
	float c[4];
	color.getRGBA(c);

	glUniform4fv(
		glGetUniformLocation(_programId, name.c_str()),
		1,
		c);
}
