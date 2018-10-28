#pragma once
#include <string>

class Skybox
{
public:
	Skybox(
		const std::string& skyboxModel,
		const std::string& skyboxTexture);

	~Skybox();

	const std::string& getTexture() const;
	const std::string& getModel() const;
private:

	std::string _skyboxModel;
	std::string _skyboxTexture;
};
