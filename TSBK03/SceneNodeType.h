#pragma once

enum class SceneNodeType
{
	INVALID = 0,
	ROOT,
	STATIC_MODEL,
	SKINNED_MODEL, // This might not be used.

	POINT_LIGHT,
	DIRECTIONAL_LIGHT,

	TERRAIN,

	AUDIO_SOURCE,

	PARTICLE_EMITTER,

	NUMTYPES
};