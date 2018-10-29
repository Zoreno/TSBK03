#pragma once

#include <string>

using GLSLTessControlShaderHandle = unsigned int;

class GLSLTessControlShader
{
public:
	explicit GLSLTessControlShader(const std::string& path);

	~GLSLTessControlShader();

	int compile();

	const std::string& getInfoLog() const;

	GLSLTessControlShaderHandle getHandle() const;
private:
	std::string _path;

	GLSLTessControlShaderHandle _handle;

	std::string _infoLog{};
};
