#pragma once
#include "Renderer.h"
#include "Timer.h"
#include "EventManager.h"
#include "KeyEvent.h"

class Frame;

class Application
{
public:
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	~Application();

	void run();

	void update(float dt);
	void render();

	Frame *getCurrentFrame() const;

	void changeFrame(Frame *newFrame);

	EventManager *getEventManager();

	void windowKeyCallback(
		int key, 
		int scancode, 
		int action, 
		int mods);

	void windowMouseButtonCallback(
		int button,
		int action,
		int mods);

	void windowMousePositionCallback(
		float xpos,
		float ypos);

	Renderer *getRenderer();

	AssetManager *getAssetManager();

private:


	EventManager _eventManager;
	AssetManager _assetManager;
	Renderer *_renderer;

	GLFWwindow *_window;

	Timer _timer;

	Frame *_currentFrame;
};
