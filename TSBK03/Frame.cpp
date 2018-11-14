#include "Frame.h"

Frame::Frame(
	Application *application)
	:_application{application}
{
	
}

Frame::~Frame()
{
	
}

Application * Frame::getApplication() const
{
	return _application;
}
