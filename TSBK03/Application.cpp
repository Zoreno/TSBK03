#include "Application.h"

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
#include "KeyEvent.h"

#include "Frame.h"
#include "MouseMovedEvent.h"
#include "MouseButtonEvent.h"
#include "Game.h"

float FOV = 70.f;
float nearPlane = 0.1f;
float farPlane = 200.f;

unsigned int windowWidth = 1280;
unsigned int windowHeight = 768;
bool frameBufferSizeChanged = false;

glm::vec3 cameraPos{ -4.f, 0.f, -4.f };
glm::vec3 cameraCenter{ 0.f, 0.f, -2.f };
glm::vec3 cameraUp{ 0.f, 1.f, 0.f };

bool debugWindowActive = false;
int currentTab = 0;

void mouse_pos_callback(GLFWwindow *window, double xpos, double ypos)
{
	if (!ImGui::GetIO().WantCaptureMouse)
	{
		Application *app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));

		app->windowMousePositionCallback(static_cast<float>(xpos), static_cast<float>(ypos));
	}
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	if (!ImGui::GetIO().WantCaptureMouse)
	{
		Application *app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));

		app->windowMouseButtonCallback(button, action, mods);
	}
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

bool fullScreen = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
	{
		if (fullScreen)
		{
			glfwSetWindowMonitor(window, NULL, 20, 40, 1280, 768, 60);
			glViewport(0, 0, 1280, 768);
		}
		else
		{
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, 60);
			glViewport(0, 0, 1920, 1080);
		}

		fullScreen = !fullScreen;
	}

	if (key == GLFW_KEY_F12 && action == GLFW_PRESS)
	{
		debugWindowActive = !debugWindowActive;
	}

	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

	if(!ImGui::GetIO().WantCaptureKeyboard)
	{
		Application *app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));

		app->windowKeyCallback(key, scancode, action, mods);
	}
}

void char_callback(GLFWwindow *window, unsigned int c)
{
	ImGui_ImplGlfw_CharCallback(window, c);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	frameBufferSizeChanged = true;

	windowWidth = width;
	windowHeight = height;
}

void drawSceneNode(SceneNode *scene, AssetManager *assetManager)
{
	static int i = 0;
	ImGui::PushID(scene->getID());
	if (ImGui::Button("Add"))
	{
		scene->addChild(new StaticModelSceneNode("palm1", std::string{ "object" } +std::to_string(i++)));
	}

	SceneNode *parent = scene->getParent();

	if (parent)
	{
		ImGui::SameLine();

		if (ImGui::Button("Remove"))
		{
			parent->removeChild(scene);

			ImGui::PopID();

			return;
		}
	}

	if (ImGui::TreeNode(scene->getTag().c_str()))
	{
		ImGui::Text("Name: %s", scene->getTag().c_str());
		ImGui::Text("Children: %d", scene->getChildrenCount());
		ImGui::Text("ID: %d", scene->getID());

		glm::vec3 position = scene->getPosition();
		glm::vec3 rotation = scene->getRotation();
		glm::vec3 scaling = scene->getScale();

		switch (scene->getSceneNodeType())
		{
		case SceneNodeType::INVALID:
			break;
		case SceneNodeType::ROOT:
			if (ImGui::InputFloat3("Position", &position[0]))
			{
				scene->setPosition(position);
			}

			break;
		case SceneNodeType::STATIC_MODEL:
		{
			StaticModelSceneNode *node = static_cast<StaticModelSceneNode *>(scene);

			if (ImGui::InputFloat3("Position", &position[0]))
			{
				scene->setPosition(position);
			}

			if (ImGui::InputFloat3("Rotation", &rotation[0]))
			{
				scene->setRotation(rotation);
			}

			if (ImGui::InputFloat3("Scale", &scaling[0]))
			{
				scene->setScale(scaling);
			}

			ImGui::Text("Model: %s", node->getModel().c_str());

			std::vector<std::string> availableModels;

			for (auto it : assetManager->getPool<Model>()->getAssets())
			{
				availableModels.push_back(it.first);
			}

			std::string currentItem = node->getModel();

			if (ImGui::BeginCombo("Select Model", currentItem.c_str(), 0))
			{
				for (int i = 0; i < availableModels.size(); ++i)
				{
					bool is_selected = (currentItem == availableModels.at(i));

					if (ImGui::Selectable(availableModels.at(i).c_str(), is_selected))
					{
						currentItem = availableModels.at(i);
						node->setModel(availableModels.at(i));
						Model *model = assetManager->fetch<Model>(availableModels.at(i));
						if (model->isSkinned())
						{
							node->setCurrentAnimation(model->getDefaultAnimation());
						}
					}

					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			std::vector<std::string> availableTextures;

			availableTextures.push_back("");

			for (auto it : assetManager->getPool<Texture2D>()->getAssets())
			{
				availableTextures.push_back(it.first);
			}

			std::string currentTextureItem = node->getTexture();

			if (ImGui::BeginCombo("Select Texture", currentTextureItem.c_str(), 0))
			{
				for (int i = 0; i < availableTextures.size(); ++i)
				{
					bool is_selected = (currentTextureItem == availableTextures.at(i));

					if (ImGui::Selectable(availableTextures.at(i).c_str(), is_selected))
					{
						currentTextureItem = availableTextures.at(i);
						node->setTexture(availableTextures.at(i));
					}

					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			Model *model = assetManager->fetch<Model>(node->getModel());

			bool wireframe = model->getWireframe();

			if (ImGui::Checkbox("Wireframe", &wireframe))
			{
				model->setWireframe(wireframe);
			}

			if (ImGui::TreeNode("Extents"))
			{
				ImGui::Text("minX: %f", model->getExtents().minX);
				ImGui::Text("minY: %f", model->getExtents().minY);
				ImGui::Text("minZ: %f", model->getExtents().minZ);
				ImGui::Text("maxX: %f", model->getExtents().maxZ);
				ImGui::Text("maxY: %f", model->getExtents().maxY);
				ImGui::Text("maxZ: %f", model->getExtents().maxZ);

				ImGui::TreePop();
			}

			if (model->isSkinned())
			{
				ImGui::Text("Current animation: %s", node->getCurrentAnimation().c_str());

				std::vector<std::string> availableAnimations{};

				availableAnimations.push_back(std::string{ "idle" });

				for (auto it : model->getAnimations())
				{
					availableAnimations.push_back(it.name);
				}

				std::string currentAnimationItem = node->getCurrentAnimation();

				if (ImGui::BeginCombo("Select Animation", currentAnimationItem.c_str(), 0))
				{
					for (int i = 0; i < availableAnimations.size(); ++i)
					{
						bool is_selected = (currentAnimationItem == availableAnimations.at(i));

						if (ImGui::Selectable(availableAnimations.at(i).c_str(), is_selected))
						{
							currentAnimationItem = availableAnimations.at(i);
							node->setCurrentAnimation(availableAnimations.at(i));
						}

						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}

				if (ImGui::TreeNode("Animations"))
				{

					for (unsigned int i = 0; i < model->getAnimations().size(); ++i)
					{
						ImGui::PushID(i);
						if (ImGui::TreeNode(model->getAnimations().at(i).name.c_str()))
						{
							const Animation& anim = model->getAnimations().at(i);

							for (unsigned int j = 0; j < anim.channels.size(); ++j)
							{
								const AnimationChannel& channel = anim.channels.at(j);

								ImGui::PushID(j);

								if (ImGui::TreeNode(channel.name.c_str()))
								{
									if (ImGui::TreeNode("Position keys"))
									{
										for (unsigned int k = 0; k < channel.positionKeys.size(); ++k)
										{
											const AnimationChannelVectorKey& key = channel.positionKeys.at(k);

											ImGui::Text("[%d] T:%.3f x:%.3f y:%.3f z:%.3f", k, key.time, key.value.x, key.value.y, key.value.z);
										}

										ImGui::TreePop();
									}

									if (ImGui::TreeNode("Rotation keys"))
									{
										for (unsigned int k = 0; k < channel.rotationKeys.size(); ++k)
										{
											const AnimationChannelQuaternionKey& key = channel.rotationKeys.at(k);

											ImGui::Text("[%d] T:%.3f w:%.3f x:%.3f y:%.3f z:%.3f", k, key.time, key.value.w, key.value.x, key.value.y, key.value.z);
										}

										ImGui::TreePop();
									}

									if (ImGui::TreeNode("Scaling keys"))
									{
										for (unsigned int k = 0; k < channel.scalingKeys.size(); ++k)
										{
											const AnimationChannelVectorKey& key = channel.scalingKeys.at(k);

											ImGui::Text("[%d] T:%.3f x:%.3f y:%.3f z:%.3f", k, key.time, key.value.x, key.value.y, key.value.z);
										}

										ImGui::TreePop();
									}

									ImGui::TreePop();
								}

								ImGui::PopID();
							}

							ImGui::TreePop();
						}

						ImGui::PopID();
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Bones"))
				{

					for (unsigned int i = 0; i < model->getBoneInfo().size(); ++i)
					{
						ImGui::PushID(1000 + i);

						const BoneInfo& boneInfo = model->getBoneInfo().at(i);

						if (ImGui::TreeNode("BoneOffset", "Boneoffset %d", i))
						{

							for (unsigned int j = 0; j < 4; ++j)
							{
								ImGui::Text("%.3f %.3f %.3f %.3f",
									boneInfo.boneOffset[0][j],
									boneInfo.boneOffset[1][j],
									boneInfo.boneOffset[2][j],
									boneInfo.boneOffset[3][j]);
							}

							ImGui::TreePop();
						}

						ImGui::PopID();
					}

					ImGui::TreePop();
				}
			}
		}
		break;
		case SceneNodeType::SKINNED_MODEL:
			if (ImGui::InputFloat3("Position", &position[0]))
			{
				scene->setPosition(position);
			}

			break;
		case SceneNodeType::POINT_LIGHT:
		{
			PointLightSceneNode *node = static_cast<PointLightSceneNode *>(scene);

			if (ImGui::InputFloat3("Position", &position[0]))
			{
				scene->setPosition(position);
			}

			glm::vec3 ambient = node->getPointLight().getAmbient();
			glm::vec3 diffuse = node->getPointLight().getDiffuse();
			glm::vec3 specular = node->getPointLight().getSpecular();

			float constant = node->getPointLight().getConstant();
			float linear = node->getPointLight().getLinear();
			float quadratic = node->getPointLight().getQuadratic();

			if (ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient)))
			{
				node->getPointLight().setAmbient(ambient);
			}

			if (ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse)))
			{
				node->getPointLight().setDiffuse(diffuse);
			}

			if (ImGui::ColorEdit3("Specular", glm::value_ptr(specular)))
			{
				node->getPointLight().setSpecular(specular);
			}

			if (ImGui::InputFloat("Constant", &constant))
			{
				node->getPointLight().setConstant(constant);
			}

			if (ImGui::InputFloat("Linear", &linear))
			{
				node->getPointLight().setLinear(linear);
			}

			if (ImGui::InputFloat("Quadratic", &quadratic))
			{
				node->getPointLight().setQuadratic(quadratic);
			}
		}
		break;
		case SceneNodeType::DIRECTIONAL_LIGHT:
		{
			DirectionalLightSceneNode *node = static_cast<DirectionalLightSceneNode *>(scene);

			glm::vec3 direction = node->getDirectionalLight().getDirection();
			glm::vec3 ambient = node->getDirectionalLight().getAmbient();
			glm::vec3 diffuse = node->getDirectionalLight().getDiffuse();
			glm::vec3 specular = node->getDirectionalLight().getSpecular();

			if (ImGui::InputFloat3("Direction", glm::value_ptr(direction)))
			{
				node->getDirectionalLight().setDirection(direction);
			}

			if (ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient)))
			{
				node->getDirectionalLight().setAmbient(ambient);
			}

			if (ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse)))
			{
				node->getDirectionalLight().setDiffuse(diffuse);
			}

			if (ImGui::ColorEdit3("Specular", glm::value_ptr(specular)))
			{
				node->getDirectionalLight().setSpecular(specular);
			}
		}
		break;
		case SceneNodeType::TERRAIN:
			if (ImGui::InputFloat3("Position", &position[0]))
			{
				scene->setPosition(position);
			}

			break;
		case SceneNodeType::AUDIO_SOURCE:
			if (ImGui::InputFloat3("Position", &position[0]))
			{
				scene->setPosition(position);
			}
			break;
		case SceneNodeType::NUMTYPES:
			break;
		default:;
		}

		ImGui::Separator();

		for (auto it : scene->getChildren())
		{
			drawSceneNode(it, assetManager);
		}

		ImGui::TreePop();
	}

	ImGui::PopID();
}

void drawAssetList(AssetManager *assetManager)
{
	ImGui::Text("Models");

	auto modelPool = assetManager->getPool<Model>();

	for (auto it : modelPool->getAssets())
	{
		if (ImGui::TreeNode(it.first.c_str()))
		{
			Model *T = it.second;

			ImGui::Text("Tag: %s", it.first.c_str());
			ImGui::Text("Filename: %s", it.second->getFileName().c_str());
			ImGui::Text("Vertices: %i", it.second->getVertexCount());
			ImGui::Text("Indices: %i", it.second->getIndicesCount());

			ImGui::TreePop();
		}
	}

	ImGui::Separator();

	ImGui::Text("Textures");

	auto texturePool = assetManager->getPool<Texture2D>();

	ImGui::PushID(1);

	for (auto it : texturePool->getAssets())
	{
		if (ImGui::TreeNode(it.first.c_str()))
		{
			Texture2D *T = it.second;

			ImGui::Text("Tag: %s", it.first.c_str());
			ImGui::Text("Filename: %s", it.second->getFilename().c_str());
			ImGui::Text("Size: %dx%d", it.second->getWidth(), it.second->getHeight());
			ImGui::Text("OpenGL handle: %d", it.second->getHandle());


			if (ImGui::ImageButton((void *)(intptr_t)T->getHandle(), ImVec2{ 150.f, 150.f }))
			{
				ImGui::OpenPopup("Menu");
			}

			if (ImGui::IsItemHovered())
			{
				if (ImGui::Begin("1", NULL, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
				{
					ImGui::TextColored(ImVec4{ 0.64f, 0.21f, 0.93f, 1.0f }, "Corrupted Ashbringer");
					ImGui::Text("Soulbound");
					ImGui::Text("Unique");
					ImGui::Text("Two-Hand                                  Sword");
					ImGui::Text("32 - 54 Damage                       Speed 3.60");
					ImGui::Text("(12.03 damage per second)");
					ImGui::TextColored(ImVec4{ 0.12f, 1.f, 0.f, 1.0f }, "+14 Critical Strike");
					ImGui::TextColored(ImVec4{ 0.12f, 1.f, 0.f, 1.0f }, "+5 Haste");
					ImGui::Text("Durability %.0f / 120", 120.f);
					ImGui::TextColored(ImVec4{ 0.12f, 1.f, 0.f, 1.0f }, "Equip: Inflicts the will of Ashbringer upon the");
					ImGui::TextColored(ImVec4{ 0.12f, 1.f, 0.f, 1.0f }, "wielder.");
					ImGui::TextColored(ImVec4{ 0.12f, 1.f, 0.f, 1.0f }, "Chance on hit: Steals 28 life from target enemy.");
					ImGui::Text("Requires Level 60");
					ImGui::TextColored(ImVec4{ 1.f, 0.82f, 0.07f, 1.0f }, "\"Blade of the Scarlet Highlord\"");
					ImGui::Text("Sell price: 11g 6s 32c");
					ImGui::End();
				}

				//ImGui::SetTooltip("This is a test tooltip\nThis is on a new line");
			}

			if (ImGui::BeginPopup("Menu"))
			{
				if (ImGui::MenuItem("Use"))
				{
					std::cout << "Use clicked" << std::endl;
				}

				if (ImGui::MenuItem("Destroy"))
				{
					std::cout << "Destroy clicked" << std::endl;
				}

				if (ImGui::MenuItem("Sell"))
				{
					std::cout << "Sell clicked" << std::endl;
				}

				if (ImGui::MenuItem("Cancel"))
				{
					std::cout << "Cancel clicked" << std::endl;
				}

				ImGui::EndPopup();
			}

			ImGui::TreePop();
		}
	}

	ImGui::PopID();
}

Application::Application()
{
	glm::mat4 proj = glm::perspective(
		glm::radians(FOV),
		static_cast<float>(windowWidth) / static_cast<float>(windowHeight),
		nearPlane,
		farPlane);

	glfwInit();

	int monitorCount;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
	for (int i = 0; i < monitorCount; ++i)
	{
		GLFWmonitor *monitor = monitors[i];
		std::cout << glfwGetMonitorName(monitor) << std::endl;

		int videoModeCount;
		const GLFWvidmode *modes = glfwGetVideoModes(monitor, &videoModeCount);

		for (int j = 0; j < videoModeCount; ++j)
		{
			//std::cout << "[Mode " << j << "] Width: " << modes[j].width << " Height: " << modes[j].height << std::end
		}
	}
	const char* glsl_version = "#version 430 core";
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(1280, 768, "Title", NULL, NULL);

	if (!_window)
	{
		std::cerr << "Could not create window" << std::endl;

		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(_window);
	glfwSwapInterval(1);
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Could not initialize GLEW" << std::endl;
		glfwTerminate();
		exit(1);
	}

	glViewport(0, 0, 1260, 768);

	glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetMouseButtonCallback(_window, mouse_button_callback);
	glfwSetCursorPosCallback(_window, mouse_pos_callback);
	glfwSetScrollCallback(_window, scroll_callback);
	glfwSetKeyCallback(_window, key_callback);
	glfwSetCharCallback(_window, char_callback);
	glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);

	// Make sure that our callbacks may access this instance
	glfwSetWindowUserPointer(_window, this);

	/*
	* IMGUI initailization
	*/

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplGlfw_InitForOpenGL(_window, false);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::StyleColorsDark();

	glEnable(GL_DEPTH_TEST);

	_assetManager.load<Model>("skyboxSphere", "sphere.obj");

	_assetManager.load<Model>("terrain", "testscene2.obj");

	_assetManager.load<Model>("palm1", "Prop_Tree_Palm_1.obj");
	_assetManager.load<Model>("palm2", "Prop_Tree_Palm_2.obj");
	_assetManager.load<Model>("palm3", "Prop_Tree_Palm_3.obj");

	Model *characterModel = _assetManager.load<Model>("warrior", "model.dae");

	characterModel->setCorrectionRotation(glm::vec3(-90.f, 0.f, -90.f));

	std::vector<std::string> terrains;
	terrains.push_back("NvF5e.tga");

	std::vector<glm::vec3> offsets;

	offsets.push_back(glm::vec3{ 0.f, 0.f, 0.f });

	Terrain *terrain = _assetManager.load<Terrain>("terrain", terrains, offsets);

	_assetManager.load<Texture2D>("grass", "grass.tga");
	_assetManager.load<Texture2D>("char", "diffuse.tga");

	TextureCubeMap *texCubeMap = _assetManager.load<TextureCubeMap>("skyboxTexture",
		"stormydays_ft.tga",
		"stormydays_bk.tga",
		"stormydays_dn.tga",
		"stormydays_up.tga",
		"stormydays_rt.tga",
		"stormydays_lf.tga");

	_assetManager.load<TextureCubeMap>("skyboxTexture1",
		"purplenebula_ft.tga",
		"purplenebula_bk.tga",
		"purplenebula_dn.tga",
		"purplenebula_up.tga",
		"purplenebula_rt.tga",
		"purplenebula_lf.tga");

	_renderer = new Renderer{ static_cast<int>(windowWidth), static_cast<int>(windowHeight), &_assetManager , proj };

	_currentFrame = new Game(this);
}

Application::~Application()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void Application::run()
{
	while (!glfwWindowShouldClose(_window) && !_appShouldClose)
	{
		static bool wireframe = false;

		float time = static_cast<float>(glfwGetTime());

		if (frameBufferSizeChanged)
		{
			if (windowWidth > 0 && windowHeight > 0)
			{
				glm::mat4 proj = glm::perspective(
					glm::radians(FOV),
					static_cast<float>(windowWidth) / static_cast<float>(windowHeight),
					nearPlane,
					farPlane);

				_renderer->setProjection(proj);

				_renderer->changeResolution(windowWidth, windowHeight);
			}

			frameBufferSizeChanged = false;
		}

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(0.0f, 0.0f, 0.0f, 1.f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (debugWindowActive)
		{
			ImGui::Begin("Debug", &debugWindowActive, ImGuiWindowFlags_MenuBar);

			if (ImGui::Button("OpenGL Info"))
			{
				currentTab = 0;
			}

			ImGui::SameLine();

			if (ImGui::Button("Renderer"))
			{
				currentTab = 1;
			}

			ImGui::SameLine();

			if (ImGui::Button("Scene Graph"))
			{
				currentTab = 2;
			}

			ImGui::SameLine();

			if (ImGui::Button("Assets"))
			{
				currentTab = 3;
			}

			ImGui::Separator();

			if (currentTab == 0)
			{

			}
			else if (currentTab == 1)
			{
				int majorVersion;
				int minorVersion;

				glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
				glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

				const char *str;

				if (majorVersion >= 3)
				{
					str = reinterpret_cast<const char *>(glGetString(GL_VERSION));

					ImGui::Text("OpenGL Version: %i.%i (%s)", majorVersion, minorVersion, str);
				}
				else
				{
					ImGui::Text("OpenGL Version: %i.%i", majorVersion, minorVersion);
				}

				str = reinterpret_cast<const char *>(glGetString(GL_VENDOR));

				ImGui::Text("Vendor: %s", str);

				str = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

				ImGui::Text("Renderer: %s", str);

				str = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));

				ImGui::Text("GLSL Version: %s", str);

				ImGui::ColorEdit4("Color", &_renderer->_color[0]);

				ImGui::SliderFloat3("Cel Thresholds", &_renderer->_celThresholds[0], 0.f, 1.f);

				ImGui::Separator();

				bool hdr = _renderer->getHdr();
				bool bloom = _renderer->getBloom();

				float exposure = _renderer->getExposure();
				float gamma = _renderer->getGamma();

				if (ImGui::Checkbox("HDR", &hdr))
				{
					_renderer->setHdr(hdr);
				}

				ImGui::SameLine();

				if (ImGui::Checkbox("Bloom", &bloom))
				{
					_renderer->setBloom(bloom);
				}

				if (ImGui::InputFloat("Exposure", &exposure))
				{
					_renderer->setExposure(exposure);
				}

				if (ImGui::InputFloat("Gamma", &gamma))
				{
					_renderer->setGamma(gamma);
				}

				ImGui::Separator();

				bool fxaa = _renderer->getFXAA();
				bool showEdges = _renderer->getShowEdges();

				float lumaThreshold = _renderer->getLumaThreshold();
				float mulReduce = _renderer->getMulReduce();
				float minReduce = _renderer->getMinReduce();
				float maxSpan = _renderer->getMaxSpan();

				if (ImGui::Checkbox("FXAA", &fxaa))
				{
					_renderer->setFXAA(fxaa);
				}

				ImGui::SameLine();

				if (ImGui::Checkbox("Show Edges", &showEdges))
				{
					_renderer->setShowEdges(showEdges);
				}

				if (ImGui::InputFloat("Luma Threshold", &lumaThreshold))
				{
					_renderer->setLumaThreshold(lumaThreshold);
				}

				if (ImGui::InputFloat("Mul Reduce", &mulReduce))
				{
					_renderer->setMulReduce(mulReduce);
				}

				if (ImGui::InputFloat("Min Reduce", &minReduce))
				{
					_renderer->setMinReduce(minReduce);
				}

				if (ImGui::InputFloat("Max Span", &maxSpan))
				{
					_renderer->setMaxSpan(maxSpan);
				}

				ImGui::Separator();

				double xpos;
				double ypos;

				glfwGetCursorPos(_window, &xpos, &ypos);

				RendererPickingInfo info = _renderer->getPickingInfo(static_cast<int>(xpos), static_cast<int>(ypos));

				int objectID = static_cast<int>(info.objectID);

				ImGui::Text("Object ID: %d", objectID);
				ImGui::Text("Cursor Pos: %f,%f", xpos, ypos);

				ImGui::Separator();

				float fps = 1000.f / _renderer->getLastGPURenderTime();

				ImGui::Text("Last Rendering Time [GPU]: %.3fms (%.0f FPS)", _renderer->getLastGPURenderTime(), fps);

				static float timeAccumulator = 0.f;
				static float lastTime = static_cast<float>(glfwGetTime());

				static float lastMinute[60] = { 0 };

				static int numSamples = 0;
				static float renderingTimeAccumulator = 0.f;

				// Store the current sample
				// TODO: This must be done outside the ImGui context
				renderingTimeAccumulator += _renderer->getLastGPURenderTime();
				++numSamples;

				// Update the time accumulator
				timeAccumulator += time - lastTime;
				lastTime = time;

				if (timeAccumulator >= 1.f)
				{
					timeAccumulator -= 1.f;

					// Shift all values to the left
					for (int i = 1; i < 60; ++i)
					{
						lastMinute[i - 1] = lastMinute[i];
					}

					// Insert the new value.
					lastMinute[59] = renderingTimeAccumulator / numSamples;

					renderingTimeAccumulator = 0.f;
					numSamples = 0;
				}

				ImGui::PlotHistogram("Frame times", lastMinute, 60, 0, nullptr, 0.f, 16.f, ImVec2(0, 100));

				ImGui::Separator();

				float godrayDensity = _renderer->getGodrayDensity();
				float godrayWeight = _renderer->getGodrayWeight();
				float godrayDecay = _renderer->getGodrayDecay();
				float godrayExposure = _renderer->getGodrayExposure();
				float godrayNumSamples = _renderer->getGodrayNumSamples();

				bool enableGodrays = _renderer->getEnableGodrays();

				if (ImGui::Checkbox("Enable Godrays", &enableGodrays))
				{
					_renderer->setEnableGodrays(enableGodrays);
				}

				if (ImGui::InputFloat("Godray Density", &godrayDensity))
				{
					_renderer->setGodrayDensity(godrayDensity);
				}

				if (ImGui::InputFloat("Godray Weight", &godrayWeight))
				{
					_renderer->setGodrayWeight(godrayWeight);
				}

				if (ImGui::InputFloat("Godray Decay", &godrayDecay))
				{
					_renderer->setGodrayDecay(godrayDecay);
				}

				if (ImGui::InputFloat("Godray Exposure", &godrayExposure))
				{
					_renderer->setGodrayExposure(godrayExposure);
				}

				if (ImGui::InputFloat("Godray Num Samples", &godrayNumSamples))
				{
					_renderer->setGodrayNumSamples(godrayNumSamples);
				}

				ImGui::Separator();


				bool drawNormals = _renderer->getDrawNormals();

				if(ImGui::Checkbox("Draw Normals", &drawNormals))
				{
					_renderer->setDrawNormals(drawNormals);
				}

			}
			else if (currentTab == 2)
			{
				//drawSceneNode(_scene, &_assetManager);
			}
			else if (currentTab == 3)
			{
				drawAssetList(&_assetManager);
			}

			ImGui::End();
		}
#if 0
		static float phi = 0.f;
		static float theta = 0.f;

		static float radius = 5.f;

		static float oldX = 0.f;
		static float oldY = 0.f;

		double currentMousePosX;
		double currentMousePosY;

		bool lockFacing = false;

		glfwGetCursorPos(_window, &currentMousePosX, &currentMousePosY);

		bool lClick = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_1) != 0;
		bool rClick = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_2) != 0;

		if (!ImGui::GetIO().WantCaptureMouse)
		{
			if (lClick || rClick)
			{
				glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

				float deltaX = static_cast<float>(currentMousePosX) - oldX;
				float deltaY = static_cast<float>(currentMousePosY) - oldY;

				phi += 0.01f * deltaX;
				theta += 0.005f * deltaY;

				if (theta < -glm::radians(89.f))
				{
					theta = -glm::radians(89.f);
				}

				if (theta > glm::radians(89.f))
				{
					theta = glm::radians(89.f);
				}
			}
			else
			{
				glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			if (rClick)
			{
				lockFacing = true;
			}
		}

		static float x = 210;
		static float y = 0;
		static float z = 215;
		static float y_vel = 0;
		bool walking = false;
		glm::vec3 delta = glm::vec3{ 0.f };
		static float speed = 0.03f;
		static bool touchingGround = true;
		static float rotationSpeed = 0.05f;

		static float facing = 0;

		if (!ImGui::GetIO().WantCaptureKeyboard)
		{
			if (glfwGetKey(_window, GLFW_KEY_E))
			{
				radius -= 0.2f;

				if (radius < 1.f)
				{
					radius = 1.f;
				}
			}
			if (glfwGetKey(_window, GLFW_KEY_Q))
			{
				radius += 0.2f;

				if (radius > 50.f)
				{
					radius = 50.f;
				}
			}
			if (glfwGetKey(_window, GLFW_KEY_W) || (lClick && rClick))
			{
				delta.x -= glm::cos(facing);
				delta.z += glm::sin(facing);
				walking = true;
			}
			if (glfwGetKey(_window, GLFW_KEY_S))
			{
				delta.x += glm::cos(facing);
				delta.z -= glm::sin(facing);
				walking = true;
			}
			if (glfwGetKey(_window, GLFW_KEY_A))
			{
				if (!lockFacing)
				{
					facing += rotationSpeed;
				}
				else
				{
					delta.x += glm::sin(facing);
					delta.z += glm::cos(facing);
					walking = true;
				}
			}
			if (glfwGetKey(_window, GLFW_KEY_D))
			{
				if (!lockFacing)
				{
					facing -= rotationSpeed;
				}
				else
				{
					delta.x -= glm::sin(facing);
					delta.z -= glm::cos(facing);
					walking = true;
				}
			}
			if (glfwGetKey(_window, GLFW_KEY_SPACE))
			{
				if (touchingGround)
				{
					y_vel = 0.11f;
				}
				touchingGround = false;
			}
		}

		oldX = static_cast<float>(currentMousePosX);
		oldY = static_cast<float>(currentMousePosY);

		if (glm::length(delta) > 0.1f)
		{
			delta = speed * glm::normalize(delta);

			x += delta.x;
			z += delta.z;
		}

		static float lastHeight;

		float height = 0;//_assetManager.fetch<Terrain>(terrainNode->getTerrain())->getHeight(x, z);

		if (y < height)
		{
			y_vel = 0;
			y = height;
			touchingGround = true;
		}
		else if (!touchingGround || lastHeight > height)
		{
			y_vel -= 9.82f / (60.f * 20.f);
			y += y_vel;
		}

		lastHeight = height;

		sceneNode1->setPosition(glm::vec3{ x, y, z });

		if (lockFacing)
		{
			facing = -phi;
		}

		sceneNode1->setRotation(glm::vec3{ 0.f, glm::degrees(facing), 0.f });

		if (walking)
		{
			sceneNode1->setCurrentAnimation("anim0");
		}
		else
		{
			sceneNode1->setCurrentAnimation("idle");
		}

		glm::vec3 center = sceneNode1->getPosition() + glm::vec3{ 0.f, 1.f, 0.f };
		glm::vec3 eye;

		eye.x = center.x + radius * glm::cos(phi) * glm::cos(theta);
		eye.y = center.y + radius * glm::sin(theta);
		eye.z = center.z + radius * glm::sin(phi) * glm::cos(theta);

		float heightAtCamera = _assetManager.fetch<Terrain>(terrainNode->getTerrain())->getHeight(eye.x, eye.z);

		if (eye.y <= heightAtCamera + 1.f)
		{
			eye.y = heightAtCamera + 1.f;
		}

		//glm::mat4 view = glm::lookAt(cameraPos, cameraCenter, cameraUp);
		glm::mat4 view = glm::lookAt(eye, center, cameraUp);

		_renderer->setCameraTransform(view);
		_renderer->setCameraPosition(eye);
		_renderer->setCameraDirection(glm::normalize(eye - center));

#endif

		float deltaTime = _timer.restart();

		update(deltaTime);

		render();

		glfwSwapBuffers(_window);
	}
}

void Application::update(
	float dt)
{
	_eventManager.handleEvents();

	if (_currentFrame != nullptr)
	{
		_currentFrame->update(dt);
	}
}

void Application::render()
{
	if (_currentFrame != nullptr)
	{
		_currentFrame->renderUI();
	}

	ImGui::Render();

	if (_currentFrame != nullptr)
	{
		_currentFrame->render(_renderer);
	}

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Frame * Application::getCurrentFrame() const
{
	return _currentFrame;
}

void Application::changeFrame(
	Frame *newFrame)
{
	if (_currentFrame != nullptr)
	{
		delete _currentFrame;
	}

	_currentFrame = newFrame;
}

EventManager * Application::getEventManager()
{
	return &_eventManager;
}

void Application::windowKeyCallback(
	int key,
	int scancode,
	int action,
	int mods)
{
	KeyEvent keyEvent;

	keyEvent.key = key;
	keyEvent.scancode = scancode;
	keyEvent.action = action;
	keyEvent.mods = mods;

	_eventManager.postEvent(keyEvent);
}

void Application::windowMouseButtonCallback(
	int button,
	int action,
	int mods)
{
	MouseButtonEvent mouseButtonEvent;

	mouseButtonEvent.button = button;
	mouseButtonEvent.action = action;
	mouseButtonEvent.mods = mods;

	_eventManager.postEvent(mouseButtonEvent);
}

void Application::windowMousePositionCallback(
	float xpos,
	float ypos)
{
	MouseMovedEvent mouseMovedEvent;

	mouseMovedEvent.x = xpos;
	mouseMovedEvent.y = ypos;

	_eventManager.postEvent(mouseMovedEvent);
}

Renderer * Application::getRenderer()
{
	return _renderer;
}

AssetManager * Application::getAssetManager()
{
	return &_assetManager;
}

GLFWwindow * Application::getWindow() const
{
	return _window;
}

void Application::shutDown()
{
	_appShouldClose = true;
}
