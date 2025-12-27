#pragma once

#include "Camera.h"
#include "RenderState.h"
#include "Light.h"

struct RenderContext
{
	ID3D11DeviceContext*	deviceContext;
	const RenderState*		renderState;
	const Camera*			camera;
	LightManager*		lightManager = nullptr;

	// š PBR’²®—p‚É’Ç‰Á
	float pbrMetalness = 0.0f;
	float pbrRoughness = 0.0f;
};
