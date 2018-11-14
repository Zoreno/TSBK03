#pragma once

#include <string>

using GLSLFragmentShaderHandle = unsigned int;

class GLSLFragmentShader
{
public:
	explicit GLSLFragmentShader(const std::string& path);

	~GLSLFragmentShader();

	int compile();

	const std::string& getInfoLog() const;

	GLSLFragmentShaderHandle getHandle() const;
private:
	std::string _path;

	GLSLFragmentShaderHandle _handle;

	std::string _infoLog{};
};
