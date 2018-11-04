#pragma once
#include "Application.h"

class Frame
{
public:
	explicit Frame(Application *application);

	Frame(const Frame&) = delete;
	Frame& operator=(const Frame&) = delete;

	virtual void update(float dt) = 0;
	virtual void render(Renderer *renderer) = 0;

	virtual ~Frame();

	Application *getApplication() const;
private:

	Application *_application;
};
