#pragma once

#include <string>

using GLSLGeometryShaderHandle = unsigned int;

class GLSLGeometryShader
{
public:
	explicit GLSLGeometryShader(const std::string& path);

	~GLSLGeometryShader();

	int compile();

	const std::string& getInfoLog() const;

	GLSLGeometryShaderHandle getHandle() const;
private:
	std::string _path;

	GLSLGeometryShaderHandle _handle;

	std::string _infoLog{};
};
