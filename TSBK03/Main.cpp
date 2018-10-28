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
 * Terrain
 * Collision/Intersection
 * Sound managing
 * UI
 * Water surfaces
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

float FOV = 70.f;
float nearPlane = 0.1f;
float farPlane = 1000.f;

unsigned int width = 1280;
unsigned int height = 768;

glm::vec3 cameraPos{ -4.f, 0.f, -4.f };
glm::vec3 cameraCenter{ 0.f, 0.f, -2.f };
glm::vec3 cameraUp{ 0.f, 1.f, 0.f };

bool debugWindowActive = true;
int currentTab = 0;

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
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
}

void char_callback(GLFWwindow *window, unsigned int c)
{
	ImGui_ImplGlfw_CharCallback(window, c);
}

void drawSceneNode(SceneNode *scene, AssetManager *assetManager)
{
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

int main()
{
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

	GLFWwindow *window = glfwCreateWindow(1280, 768, "Title", NULL, NULL);

	if (!window)
	{
		std::cerr << "Could not create window" << std::endl;

		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Could not initialize GLEW" << std::endl;
		glfwTerminate();
		exit(1);
	}

	glViewport(0, 0, 1260, 768);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, char_callback);

	/*
	* IMGUI initailization
	*/

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplGlfw_InitForOpenGL(window, false);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::StyleColorsDark();

	glEnable(GL_DEPTH_TEST);

	AssetManager assetManager{};

	assetManager.load<Model>("skyboxSphere", "sphere.obj");
	//assetManager.load<Model>("sphere", "groundsphere.obj");
	//assetManager.load<Model>("tree", "lowpolytree.obj");
	assetManager.load<Model>("terrain", "testscene2.obj");
	//Model *charModel = assetManager.load<Model>("char", "model.dae");
	//assetManager.load<Model>("sylvanas", "sylvanas_obj.obj");
	assetManager.load<Model>("palm1", "Prop_Tree_Palm_1.obj");
	//assetManager.load<Model>("palm2", "Prop_Tree_Palm_2.obj");
	//assetManager.load<Model>("palm3", "Prop_Tree_Palm_3.obj");
	assetManager.load<Model>("warrior", "model.dae");

	std::vector<std::string> terrains;
	terrains.push_back("fft-terrain.tga");

	std::vector<glm::vec3> offsets;
	offsets.push_back(glm::vec3{ 0.f, 0.f, 0.f });

	assetManager.load<Terrain>("terrain", terrains, offsets);

	assetManager.load<Texture2D>("grass", "grass.tga");
	assetManager.load<Texture2D>("char", "diffuse.tga");

	TextureCubeMap *texCubeMap = assetManager.load<TextureCubeMap>("skyboxTexture",
		"stormydays_ft.tga",
		"stormydays_bk.tga",
		"stormydays_dn.tga",
		"stormydays_up.tga",
		"stormydays_rt.tga",
		"stormydays_lf.tga");

	assetManager.load<TextureCubeMap>("skyboxTexture1",
		"midnight-silence_ft.tga",
		"midnight-silence_bk.tga",
		"midnight-silence_dn.tga",
		"midnight-silence_up.tga",
		"midnight-silence_rt.tga",
		"midnight-silence_lf.tga");

	glm::mat4 proj = glm::perspective(
		glm::radians(FOV),
		static_cast<float>(width) / static_cast<float>(height),
		nearPlane,
		farPlane);


	Renderer renderer{ static_cast<int>(width), static_cast<int>(height), &assetManager , proj };

	Scene scene{ SceneNodeType::ROOT, "ROOT" };

	TerrainSceneNode *terrainNode = new TerrainSceneNode("terrain", "terrain");

	scene.addChild(terrainNode);

	StaticModelSceneNode *sceneNode0 = new StaticModelSceneNode{ "terrain", "terrain" };

	sceneNode0->setPosition(glm::vec3{ 0.f, 0.f, 0.f });
	sceneNode0->setRotation(glm::vec3{ 0.f, 0.f, 0.f });
	sceneNode0->setScale(glm::vec3{ 1.f, 1.f, 1.f });
	sceneNode0->setTexture("");

	scene.addChild(sceneNode0);

	StaticModelSceneNode *sceneNode1 = new StaticModelSceneNode{ "palm1", "Player" };

	sceneNode1->setPosition(glm::vec3{ 0.f, 1.f, 0.f });
	sceneNode1->setRotation(glm::vec3{ 0.f, 0.f, 0.f });
	sceneNode1->setScale(glm::vec3{ 0.3f, 0.3f, 0.3f });
	sceneNode1->setTexture("");
	//sceneNode1->setCurrentAnimation(assetManager.fetch<Model>(sceneNode1->getModel())->getDefaultAnimation());

	scene.addChild(sceneNode1);

	SceneNode *pointLight1 = new PointLightSceneNode{
		PointLight{
			glm::vec3{ 0.2f, 0.2f, 0.2f },
			glm::vec3{ 0.2f, 0.2f, 0.8f },
			glm::vec3{ 0.8f, 0.8f, 0.8f },
			1.f,
			0.2f,
			0.003f} ,
		"PointLight1" };

	pointLight1->setPosition(glm::vec3(-3.f, 3.f, 72893.f));

	scene.addChild(pointLight1);

	DirectionalLightSceneNode *directionalLight1 = new DirectionalLightSceneNode{
		DirectionalLight{
			glm::vec3{0.f, -3.f, 0.f},
			glm::vec3{ 0.2f, 0.2f, 0.2f },
			glm::vec3{ 0.98f, 0.98f, 0.90f },
			glm::vec3{ 0.8f, 0.8f, 0.8f }},
		"DirectionalLight1" };

	scene.addChild(directionalLight1);

	while (!glfwWindowShouldClose(window))
	{
		static bool wireframe = false;

		float time = glfwGetTime();

		directionalLight1->getDirectionalLight().setDirection(
			3.f * glm::vec3{ glm::cos(time / 10.f), -0.5f - 0.5f * glm::cos(time / 20.f), glm::sin(time / 10.f) });

		directionalLight1->getDirectionalLight().setDiffuse(
			glm::vec3(
				0.95f,
				0.95f * (0.5f * glm::cos(time / 20.f) + 0.5f),
				0.95f * (0.5f * glm::cos(time / 20.f) + 0.5f)));

		if (wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

			if (ImGui::Button("General"))
			{
				currentTab = 0;
			}

			ImGui::SameLine();

			if (ImGui::Button("Renderer"))
			{
				currentTab = 1;
			}

			ImGui::SameLine();

			if (ImGui::Button("SceneGraph"))
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
				if (ImGui::TreeNode("Model"))
				{
					if (ImGui::Checkbox("Wireframe", &wireframe))
					{
						if (wireframe)
						{
							glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						}
						else
						{
							glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						}
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Camera"))
				{
					ImGui::InputFloat3("Camera Position", glm::value_ptr(cameraPos));
					ImGui::InputFloat3("Camera Target", glm::value_ptr(cameraCenter));
					ImGui::InputFloat3("Camera Up", glm::value_ptr(cameraUp));

					ImGui::TreePop();
				}
			}
			else if (currentTab == 1)
			{
				ImGui::ColorEdit4("Color", &renderer._color[0]);

				ImGui::SliderFloat3("Cel Thresholds", &renderer._celThresholds[0], 0.f, 1.f);

				ImGui::Separator();

				bool hdr = renderer.getHdr();
				bool bloom = renderer.getBloom();

				float exposure = renderer.getExposure();
				float gamma = renderer.getGamma();

				if (ImGui::Checkbox("HDR", &hdr))
				{
					renderer.setHdr(hdr);
				}

				ImGui::SameLine();

				if (ImGui::Checkbox("Bloom", &bloom))
				{
					renderer.setBloom(bloom);
				}

				if (ImGui::InputFloat("Exposure", &exposure))
				{
					renderer.setExposure(exposure);
				}

				if (ImGui::InputFloat("Gamma", &gamma))
				{
					renderer.setGamma(gamma);
				}

				ImGui::Separator();

				bool fxaa = renderer.getFXAA();
				bool showEdges = renderer.getShowEdges();

				float lumaThreshold = renderer.getLumaThreshold();
				float mulReduce = renderer.getMulReduce();
				float minReduce = renderer.getMinReduce();
				float maxSpan = renderer.getMaxSpan();

				if (ImGui::Checkbox("FXAA", &fxaa))
				{
					renderer.setFXAA(fxaa);
				}

				ImGui::SameLine();

				if (ImGui::Checkbox("Show Edges", &showEdges))
				{
					renderer.setShowEdges(showEdges);
				}

				if (ImGui::InputFloat("Luma Threshold", &lumaThreshold))
				{
					renderer.setLumaThreshold(lumaThreshold);
				}

				if (ImGui::InputFloat("Mul Reduce", &mulReduce))
				{
					renderer.setMulReduce(mulReduce);
				}

				if (ImGui::InputFloat("Min Reduce", &minReduce))
				{
					renderer.setMinReduce(minReduce);
				}

				if (ImGui::InputFloat("Max Span", &maxSpan))
				{
					renderer.setMaxSpan(maxSpan);
				}

				ImGui::Separator();

				double xpos;
				double ypos;

				glfwGetCursorPos(window, &xpos, &ypos);

				RendererPickingInfo info = renderer.getPickingInfo(static_cast<int>(xpos), static_cast<int>(height - ypos));

				int objectID = static_cast<int>(info.objectID);

				ImGui::Text("Object ID: %d", objectID);
				ImGui::Text("Cursor Pos: %f,%f", xpos, ypos);

				ImGui::Separator();

				ImGui::Text("Last Rendering Time [GPU]: %.3fms", renderer.getLastGPURenderTime());

				static float timeAccumulator = 0.f;
				static float lastTime = glfwGetTime();

				static float lastMinute[60] = { 0 };

				static int numSamples = 0;
				static float renderingTimeAccumulator = 0.f;

				// Store the current sample
				// TODO: This must be done outside the ImGui context
				renderingTimeAccumulator += renderer.getLastGPURenderTime();
				++numSamples;

				float time = glfwGetTime();

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

				float godrayDensity = renderer.getGodrayDensity();
				float godrayWeight = renderer.getGodrayWeight();
				float godrayDecay =renderer.getGodrayDecay();
				float godrayExposure = renderer.getGodrayExposure();
				float godrayNumSamples = renderer.getGodrayNumSamples();

				if (ImGui::InputFloat("Godray Density", &godrayDensity))
				{
					renderer.setGodrayDensity(godrayDensity);
				}

				if (ImGui::InputFloat("Godray Weight", &godrayWeight))
				{
					renderer.setGodrayWeight(godrayWeight);
				}

				if (ImGui::InputFloat("Godray Decay", &godrayDecay))
				{
					renderer.setGodrayDecay(godrayDecay);
				}

				if (ImGui::InputFloat("Godray Exposure", &godrayExposure))
				{
					renderer.setGodrayExposure(godrayExposure);
				}

				if (ImGui::InputFloat("Godray Num Samples", &godrayNumSamples))
				{
					renderer.setGodrayNumSamples(godrayNumSamples);
				}

			}
			else if (currentTab == 2)
			{
				drawSceneNode(&scene, &assetManager);
			}
			else if (currentTab == 3)
			{
				drawAssetList(&assetManager);
			}

			ImGui::End();
		}

		ImGui::Render();

		static float phi = 0.f;
		static float theta = 0.f;

		static float radius = 5.f;

		static float oldX = 0.f;
		static float oldY = 0.f;

		double currentMousePosX;
		double currentMousePosY;

		bool lockFacing = false;

		glfwGetCursorPos(window, &currentMousePosX, &currentMousePosY);

		if (!ImGui::GetIO().WantCaptureMouse)
		{
			bool lClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
			bool rClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2);

			if (lClick || rClick)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			if(rClick)
			{
				lockFacing = true;
			}
		}

		static float x = 0;
		static float z = 0;
		glm::vec3 delta = glm::vec3{ 0.f };
		static float speed = 0.1f;
		static float rotationSpeed = 0.05f;

		static float facing = 0;

		if (!ImGui::GetIO().WantCaptureKeyboard)
		{
			if (glfwGetKey(window, GLFW_KEY_E))
			{
				radius -= 0.5f;

				if (radius < 3.f)
				{
					radius = 3.f;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_Q))
			{
				radius += 0.5f;

				if (radius > 50.f)
				{
					radius = 50.f;
				}
			}
			if (glfwGetKey(window, GLFW_KEY_W))
			{
				delta.x -= glm::cos(facing);
				delta.z += glm::sin(facing);
			}
			if (glfwGetKey(window, GLFW_KEY_S))
			{
				delta.x += glm::cos(facing);
				delta.z -= glm::sin(facing);
			}
			if (glfwGetKey(window, GLFW_KEY_A))
			{
				if (!lockFacing)
				{
					facing += rotationSpeed;	
				}
				else
				{
					delta.x += glm::sin(facing);
					delta.z += glm::cos(facing);
				}
			}
			if (glfwGetKey(window, GLFW_KEY_D))
			{
				if (!lockFacing)
				{
					facing -= rotationSpeed;
				}
				else
				{
					delta.x -= glm::sin(facing);
					delta.z -= glm::cos(facing);
				}
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

		float height = assetManager.fetch<Terrain>(terrainNode->getTerrain())->getHeight(x, z);

		sceneNode1->setPosition(glm::vec3{ x, height, z });

		if(lockFacing)
		{
			facing = -phi;
		}

		sceneNode1->setRotation(glm::vec3{ 0.f, glm::degrees(facing), 1.f });

		glm::vec3 center = sceneNode1->getPosition() + glm::vec3{ 0.f, 2.f, 0.f };
		glm::vec3 eye;

		eye.x = center.x + radius * glm::cos(phi) * glm::cos(theta);
		eye.y = center.y + radius * glm::sin(theta);
		eye.z = center.z + radius * glm::sin(phi) * glm::cos(theta);

		//glm::mat4 view = glm::lookAt(cameraPos, cameraCenter, cameraUp);
		glm::mat4 view = glm::lookAt(eye, center, cameraUp);

		renderer.setCameraTransform(view);
		renderer.setCameraPosition(eye);
		renderer.setCameraDirection(glm::normalize(eye - center));

		renderer.render(&scene);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return 0;
}