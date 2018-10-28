#include "Skybox.h"

Skybox::Skybox(
	const std::string &skyboxModel,
	const std::string &skyboxTexture)
	:_skyboxModel{ skyboxModel },
	_skyboxTexture{ skyboxTexture }
{
	
}

Skybox::~Skybox()
{
	
}

const std::string & Skybox::getTexture() const
{
	return _skyboxTexture;
}

const std::string & Skybox::getModel() const
{
	return _skyboxModel;
}
