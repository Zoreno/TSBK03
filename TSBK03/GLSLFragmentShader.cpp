#include "GLSLFragmentShader.h"

#include <GL/glew.h>

#include "Utils.h"
#include <iostream>

GLSLFragmentShader::GLSLFragmentShader(
	const std::string &path)
	:_path{ path }
{

}

GLSLFragmentShader::~GLSLFragmentShader()
{
	if (glIsShader(_handle))
	{
		glDeleteShader(_handle);
	}
}

int GLSLFragmentShader::compile()
{
	_handle = glCreateShader(GL_FRAGMENT_SHADER);

	std::string shaderSource = getStringFromFile(_path);
	const char *shaderSourcePtr = shaderSource.c_str();
	glShaderSource(_handle, 1, &shaderSourcePtr, NULL);
	glCompileShader(_handle);

	GLint success;
	glGetShaderiv(_handle, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		GLchar *infoLog;
		GLint infoLogSize;
		glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &infoLogSize);

		infoLog = static_cast<GLchar *>(malloc(infoLogSize));

		glGetShaderInfoLog(_handle, infoLogSize, NULL, infoLog);

		_infoLog = std::string{ infoLog };

		free(infoLog);
	}

	return success;
}

const std::string & GLSLFragmentShader::getInfoLog() const
{
	return _infoLog;
}

GLSLFragmentShaderHandle GLSLFragmentShader::getHandle() const
{
	return _handle;
}
