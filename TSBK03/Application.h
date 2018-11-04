#pragma once
#include "Renderer.h"
#include "Timer.h"
#include "EventManager.h"
#include "KeyEvent.h"

class Frame;

class Application : public Subscriber<KeyEvent>
{
public:
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	~Application() override;

	void run();

	void update(float dt);
	void render();

	Frame *getCurrentFrame() const;

	void changeFrame(Frame *newFrame);

	void handleEvent(const KeyEvent &ev) override;

private:
	EventManager _eventManager;
	AssetManager _assetManager;
	Renderer *_renderer;

	GLFWwindow *_window;

	Scene *_scene;

	Timer _timer;

	Frame *_currentFrame;
};
