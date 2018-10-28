#include "Model.h"


#include <iostream>

Model::Model(
	const std::string &fileName)
	:_fileName{ fileName }
{
	loadModel(fileName);
}

Model::~Model()
{
	while (!_meshes.empty())
	{
		delete _meshes.back();
		_meshes.pop_back();
	}
}

const std::string & Model::getFileName() const
{
	return _fileName;
}

bool Model::isSkinned() const
{
	return _numBones > 0;
}

bool Model::getWireframe() const
{
	return _wireframe;
}

void Model::setWireframe(
	bool wireframe)
{
	_wireframe = wireframe;
}

const std::map<std::string, unsigned> & Model::getBoneMapping() const
{
	return _boneMapping;
}

const std::vector<BoneInfo> & Model::getBoneInfo() const
{
	return _boneInfo;
}

const std::map<std::string, unsigned> & Model::getAnimationMapping() const
{
	return _boneMapping;
}

const std::vector<Animation> & Model::getAnimations() const
{
	return _animations;
}

const std::vector<Material> & Model::getMaterials() const
{
	return _materials;
}

void Model::getBoneTransforms(
	float time,
	std::vector<glm::mat4> &transforms,
	const std::string& currentAnimation)
{
	glm::mat4 identity{ 1.f };

	float ticksPerSecond = _animations[_animationMapping[currentAnimation]].ticksPerSecond;
	if (ticksPerSecond == 0.f)
	{
		ticksPerSecond = 25.f;
	}

	float timeInTicks = time * ticksPerSecond;
	float animationTime = fmod(timeInTicks, _animations[_animationMapping[currentAnimation]].duration);

	recurseGetTransforms(animationTime, 0, identity, currentAnimation);

	transforms.resize(_numBones);

	for (unsigned int i = 0; i < _numBones; ++i)
	{
		transforms[i] = _boneInfo[i].finalTransformation;
	}
}

AABB Model::getExtents() const
{
	return _extents;
}

int Model::getVertexCount() const
{
	int i = 0;

	for(auto it : _meshes)
	{
		i += it->getVertices().size();
	}

	return i;
}

int Model::getIndicesCount() const
{
	int i = 0;

	for (auto it : _meshes)
	{
		i += it->getIndices().size();
	}

	return i;
}

glm::vec3 Model::getCorrectionRotation() const
{
	return _correctionRotation;
}

void Model::setCorrectionRotation(
	const glm::vec3 &newRotation)
{
	_correctionRotation = newRotation;
}

glm::mat4 Model::getCorrectionTransform() const
{
	glm::mat4 result;

	result = glm::rotate(result, glm::radians(_correctionRotation.x), glm::vec3{ 1.f, 0.f, 0.f });
	result = glm::rotate(result, glm::radians(_correctionRotation.y), glm::vec3{ 0.f, 1.f, 0.f });
	result = glm::rotate(result, glm::radians(_correctionRotation.z), glm::vec3{ 0.f, 0.f, 1.f });

	return result;
}

const std::string & Model::getDefaultAnimation() const
{
	return _animationMapping.begin()->first;
}

const std::vector<Mesh *> & Model::getMeshes() const
{
	return _meshes;
}

void Model::loadModel(
	const std::string &fileName)
{
	Assimp::Importer importer;

	const aiScene *scene = importer.ReadFile(
		fileName.c_str(),
		aiProcess_Triangulate /*| aiProcess_FlipUVs*/ | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "[ASSIMP] Error: " << importer.GetErrorString() << std::endl;
		return;
	}

	_extents.minX = std::numeric_limits<float>::max();
	_extents.minY = std::numeric_limits<float>::max();
	_extents.minZ = std::numeric_limits<float>::max();

	_extents.maxX = std::numeric_limits<float>::min();
	_extents.maxY = std::numeric_limits<float>::min();
	_extents.maxZ = std::numeric_limits<float>::min();

	_directory = fileName.substr(0, fileName.find_last_of('/'));

	// There has to be a better way to do this...
	_globalTransform = glm::mat4{
		scene->mRootNode->mTransformation.a1,
		scene->mRootNode->mTransformation.a2,
		scene->mRootNode->mTransformation.a3,
		scene->mRootNode->mTransformation.a4,

		scene->mRootNode->mTransformation.b1,
		scene->mRootNode->mTransformation.b2,
		scene->mRootNode->mTransformation.b3,
		scene->mRootNode->mTransformation.b4,

		scene->mRootNode->mTransformation.c1,
		scene->mRootNode->mTransformation.c2,
		scene->mRootNode->mTransformation.c3,
		scene->mRootNode->mTransformation.c4,

		scene->mRootNode->mTransformation.d1,
		scene->mRootNode->mTransformation.d2,
		scene->mRootNode->mTransformation.d3,
		scene->mRootNode->mTransformation.d4
	};

	_globalTransform = glm::transpose(_globalTransform);

	_globalInverseTransform = glm::inverse(_globalTransform);

	processNode(scene->mRootNode, scene, -1);

	processAnimations(scene->mRootNode, scene);

	processMaterials(scene);

	importer.FreeScene();
}

unsigned int Model::processNode(
	aiNode *node,
	const aiScene *scene,
	unsigned int parentID)
{
	ModelNode modelNode;
	modelNode.name = std::string{ node->mName.data };

	modelNode.id = _nodes.size();
	modelNode.parent = parentID;
	modelNode.transform = glm::mat4{
		node->mTransformation.a1,
		node->mTransformation.a2,
		node->mTransformation.a3,
		node->mTransformation.a4,

		node->mTransformation.b1,
		node->mTransformation.b2,
		node->mTransformation.b3,
		node->mTransformation.b4,

		node->mTransformation.c1,
		node->mTransformation.c2,
		node->mTransformation.c3,
		node->mTransformation.c4,

		node->mTransformation.d1,
		node->mTransformation.d2,
		node->mTransformation.d3,
		node->mTransformation.d4
	};

	modelNode.transform = glm::transpose(modelNode.transform);

	_nodeMapping.emplace(modelNode.name, modelNode.id);
	_nodes.push_back(modelNode);

	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		unsigned int childID = processNode(node->mChildren[i], scene, modelNode.id);

		_nodes.at(modelNode.id).children.push_back(childID);
	}

	return modelNode.id;
}

void Model::processAnimations(
	aiNode *node,
	const aiScene *scene)
{
	for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
	{
		Animation animation;

		const aiAnimation *animPtr = scene->mAnimations[i];

		animation.name = std::string{ animPtr->mName.data };

		if (animation.name.empty())
		{
			animation.name = std::string{ "anim" } +std::to_string(i);
		}

		animation.ticksPerSecond = animPtr->mTicksPerSecond;
		animation.duration = animPtr->mDuration;

		for (unsigned int j = 0; j < animPtr->mNumChannels; ++j)
		{
			const aiNodeAnim *nodeAnimPtr = animPtr->mChannels[j];

			AnimationChannel channel;
			channel.name = std::string{ nodeAnimPtr->mNodeName.data };

			for (unsigned int k = 0; k < nodeAnimPtr->mNumPositionKeys; ++k)
			{
				AnimationChannelVectorKey posKey;

				posKey.value = glm::vec3(
					nodeAnimPtr->mPositionKeys[k].mValue.x,
					nodeAnimPtr->mPositionKeys[k].mValue.y,
					nodeAnimPtr->mPositionKeys[k].mValue.z);

				posKey.time = nodeAnimPtr->mPositionKeys[k].mTime;

				channel.positionKeys.push_back(posKey);
			}

			for (unsigned int k = 0; k < nodeAnimPtr->mNumRotationKeys; ++k)
			{
				AnimationChannelQuaternionKey rotKey;

				rotKey.value = glm::quat(
					nodeAnimPtr->mRotationKeys[k].mValue.w,
					nodeAnimPtr->mRotationKeys[k].mValue.x,
					nodeAnimPtr->mRotationKeys[k].mValue.y,
					nodeAnimPtr->mRotationKeys[k].mValue.z);

				rotKey.time = nodeAnimPtr->mRotationKeys[k].mTime;

				channel.rotationKeys.push_back(rotKey);
			}

			for (unsigned int k = 0; k < nodeAnimPtr->mNumScalingKeys; ++k)
			{
				AnimationChannelVectorKey scaleKey;

				scaleKey.value = glm::vec3(
					nodeAnimPtr->mScalingKeys[k].mValue.x,
					nodeAnimPtr->mScalingKeys[k].mValue.y,
					nodeAnimPtr->mScalingKeys[k].mValue.z);

				scaleKey.time = nodeAnimPtr->mScalingKeys[k].mTime;

				channel.scalingKeys.push_back(scaleKey);
			}

			animation.channels.push_back(channel);
		}

		_animationMapping.emplace(animation.name, i);
		_animations.push_back(animation);
	}
}

void Model::processMaterials(
	const aiScene *scene)
{
	for(unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		const aiMaterial *material = scene->mMaterials[i];

		Material newMaterial;

		aiString name;
		aiGetMaterialString(material, AI_MATKEY_NAME, &name);
		newMaterial.name = std::string{ name.C_Str() };

		aiColor4D ambient;
		aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
		newMaterial.ambientColor = glm::vec3{ ambient.r, ambient.g, ambient.b };

		aiColor4D diffuse;
		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		newMaterial.diffuseColor = glm::vec3{ diffuse.r, diffuse.g, diffuse.b };

		aiColor4D specular;
		aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
		newMaterial.specularColor = glm::vec3{ specular.r, specular.g, specular.b };

		float exponent;
		aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &exponent);
		newMaterial.exponent = exponent;

		_materials.push_back(newMaterial);
	}
}

Mesh *Model::processMesh(
	aiMesh *mesh,
	const aiScene *scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::vector<VertexBoneData> boneData{};

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		vertex._position = glm::vec3{
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z };

		if (vertex._position.x < _extents.minX)
		{
			_extents.minX = vertex._position.x;
		}

		if (vertex._position.y < _extents.minY)
		{
			_extents.minY = vertex._position.y;
		}

		if (vertex._position.z < _extents.minZ)
		{
			_extents.minZ = vertex._position.z;
		}

		if(vertex._position.x > _extents.maxX)
		{
			_extents.maxX = vertex._position.x;
		}

		if (vertex._position.y > _extents.maxY)
		{
			_extents.maxY = vertex._position.y;
		}

		if (vertex._position.z > _extents.maxZ)
		{
			_extents.maxZ = vertex._position.z;
		}

		vertex._normal = glm::vec3{
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z };

		vertex._normal = glm::normalize(vertex._normal);

		if (mesh->mTextureCoords[0] != nullptr)
		{
			vertex._texCoord = glm::vec2{
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y };
		}
		else
		{
			vertex._texCoord = glm::vec2{ 0.f, 0.f };
		}

		if (mesh->mTangents != nullptr)
		{
			vertex._tangent = glm::vec3{
				mesh->mTangents[i].x,
				mesh->mTangents[i].y,
				mesh->mTangents[i].z };

			vertex._bitangent = glm::vec3{
				mesh->mBitangents[i].x,
				mesh->mBitangents[i].y,
				mesh->mBitangents[i].z };
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0;i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->HasBones())
	{
		boneData.resize(vertices.size());

		// For each bone in this vertex
		for (unsigned int i = 0; i < mesh->mNumBones; ++i)
		{
			unsigned int boneIndex = 0;
			std::string boneName(mesh->mBones[i]->mName.data);

			// Check if this bone doesn't already have an ID
			if (_boneMapping.find(boneName) == _boneMapping.end())
			{
				boneIndex = _numBones;
				++_numBones;
				BoneInfo bi;

				// ...
				bi.boneOffset = glm::mat4{
					mesh->mBones[i]->mOffsetMatrix.a1,
					mesh->mBones[i]->mOffsetMatrix.a2,
					mesh->mBones[i]->mOffsetMatrix.a3,
					mesh->mBones[i]->mOffsetMatrix.a4,

					mesh->mBones[i]->mOffsetMatrix.b1,
					mesh->mBones[i]->mOffsetMatrix.b2,
					mesh->mBones[i]->mOffsetMatrix.b3,
					mesh->mBones[i]->mOffsetMatrix.b4,

					mesh->mBones[i]->mOffsetMatrix.c1,
					mesh->mBones[i]->mOffsetMatrix.c2,
					mesh->mBones[i]->mOffsetMatrix.c3,
					mesh->mBones[i]->mOffsetMatrix.c4,

					mesh->mBones[i]->mOffsetMatrix.d1,
					mesh->mBones[i]->mOffsetMatrix.d2,
					mesh->mBones[i]->mOffsetMatrix.d3,
					mesh->mBones[i]->mOffsetMatrix.d4 };

				bi.boneOffset = glm::transpose(bi.boneOffset);

				_boneInfo.push_back(bi);

				_boneMapping[boneName] = boneIndex;
			}
			else
			{
				boneIndex = _boneMapping[boneName];
			}

			// Go through each of the bone's weights and add it to the correct vertex
			for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
			{
				unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
				float weight = mesh->mBones[i]->mWeights[j].mWeight;
				boneData[vertexID].addBoneData(boneIndex, weight);
			}

		}
	}

	unsigned int materialIndex = mesh->mMaterialIndex;

	return new Mesh(vertices, indices, boneData, materialIndex);
}

void Model::recurseGetTransforms(
	float animationTime,
	unsigned int node,
	const glm::mat4 &parentTransform,
	const std::string& currentAnimation)
{
	std::string nodeName = _nodes[node].name;

	glm::mat4 nodeTransform = _nodes[node].transform;

	AnimationChannel channel;
	bool found = findAnimationChannel(_animations[_animationMapping[currentAnimation]], nodeName, &channel);

	if (found)
	{
		glm::vec3 scaling = channel.getInterpolatedScaling(animationTime);
		glm::mat4 scalingM = glm::scale(glm::mat4{ 1.f }, scaling);

		glm::quat rotation = channel.getInterpolatedRotation(animationTime);
		glm::mat4 rotationM = glm::mat4{ rotation };

		glm::vec3 translation = channel.getInterpolatedPosition(animationTime);
		glm::mat4 translationM = glm::translate(glm::mat4{ 1.f }, translation);

		nodeTransform = translationM * rotationM * scalingM;
	}

	glm::mat4 globalTransform = parentTransform * nodeTransform;

	if (_boneMapping.find(nodeName) != _boneMapping.end())
	{
		unsigned int boneIndex = _boneMapping[nodeName];
		_boneInfo[boneIndex].finalTransformation = _globalInverseTransform * globalTransform * _boneInfo[boneIndex].boneOffset;
	}

	for (unsigned int i = 0; i < _nodes[node].children.size(); ++i)
	{
		recurseGetTransforms(animationTime, _nodes[node].children[i], globalTransform, currentAnimation);
	}
}

Animation & Model::findAnimation(
	const std::string &name)
{
	return _animations[_animationMapping[name]];
}

bool Model::findAnimationChannel(
	const Animation &animation,
	const std::string &channelName,
	AnimationChannel *out)
{
	for (auto it : animation.channels)
	{
		if (it.name == channelName)
		{
			*out = it;

			return true;
		}
	}

	return false;
}
