#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SceneNode.h"
#include "Renderer.h"
#include "PointLightSceneNode.h"
#include "Model.h"
#include "StaticModelSceneNode.h"

#include "RandomGenerator.h"
#include "MersenneDevice.h"
#include "Color.h"
#include "DirectionalLightSceneNode.h"
#include "AssetManager.h"
#include "Terrain.h"

#include "Application.h"

#include "Utils.h"

/*
 * TODO List Engine:
 *
 * Have picking take skinning into account.
 *
 * Move engine to class
 *  - Renderer
 *  - Asset Manager
 *  - Scene Manager
 *  - Sound Manager
 *  - Physics Manager
 *  - UI Manager
 *  - Window Manager
 *
 * Window abstraction
 * Collision/Intersection
 * Sound managing
 * UI
 * Particles
 * Deferred rendering
 *
 * SSAO
 * SSDO
 * HBAO
 *
 * TODO List Game:
 *
 * All of it...
 *
 */

 /*
  * Application
  */

#ifdef TSBK03_NO_CONSOLE

#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
#else
int main(int argc, char **argv)
#endif
{
	Application app{};

	app.run();

	return 0;
}