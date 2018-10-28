#pragma once

#include <vector>
#include <map>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Vertex.h"
#include "BoneInfo.h"
#include "Animation.h"
#include "ModelNode.h"

#include "AABB.h"
#include "Material.h"

class Model
{
public:
	explicit Model(const std::string& fileName);

	~Model();

	const std::string& getFileName() const;

	bool isSkinned() const;

	bool getWireframe() const;
	void setWireframe(bool wireframe);

	const std::map<std::string, unsigned int>& getBoneMapping() const;
	const std::vector<BoneInfo>& getBoneInfo() const;

	const std::map<std::string, unsigned int>& getAnimationMapping() const;
	const std::vector<Animation>& getAnimations() const;

	const std::vector<Material>& getMaterials() const;

	void getBoneTransforms(float time, std::vector<glm::mat4>& transforms, const std::string& currentAnimation);

	AABB getExtents() const;

	int getVertexCount() const;
	int getIndicesCount() const;

	glm::vec3 getCorrectionRotation() const;
	void setCorrectionRotation(const glm::vec3& newRotation);

	glm::mat4 getCorrectionTransform() const;

	const std::string& getDefaultAnimation() const;

	const std::vector<Mesh *>& getMeshes() const;
private:

	glm::vec3 _correctionRotation{ 0.f, 0.f, 0.f };
	
	void loadModel(const std::string& fileName);

	unsigned int processNode(aiNode *node, const aiScene *scene, unsigned int parentID);
	void processAnimations(aiNode *node, const aiScene *scene);

	void processMaterials(const aiScene *scene);

	Mesh *processMesh(aiMesh *mesh, const aiScene *scene);

	bool _wireframe{false};

	std::vector<Mesh *> _meshes;

	std::string _directory{};
	std::string _fileName{};

	unsigned int _numBones{ 0 };
	std::map<std::string, unsigned int> _boneMapping{};
	std::vector<BoneInfo> _boneInfo{};

	std::map<std::string, unsigned int> _animationMapping{};
	std::vector<Animation> _animations{};

	std::map<std::string, unsigned int> _nodeMapping{};
	std::vector<ModelNode> _nodes{};

	std::vector<Material> _materials{};

	glm::mat4 _globalTransform;
	glm::mat4 _globalInverseTransform;

	void recurseGetTransforms(
		float animationTime,
		unsigned int node,
		const glm::mat4 &parentTransform,
		const std::string &currentAnimation);

	Animation& findAnimation(const std::string& name);
	bool findAnimationChannel(
		const Animation &animation,
		const std::string &channelName,
		AnimationChannel *out);

	AABB _extents;
};
