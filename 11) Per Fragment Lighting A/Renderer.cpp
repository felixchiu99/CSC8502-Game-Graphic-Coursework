#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/Light_Directional.h"
#include "../nclgl/Light_Spot.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	heightMap = new HeightMap(TEXTUREDIR"ridgeTest.png",Vector3(16.0f,2.0f,16.0f));
	texture = SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	shader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");

	if (!shader->LoadSuccess() || !texture) {
		return;
	}
	SetTextureRepeating(texture, true);
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	camera = new Camera(-45.0f, 0.0f,
		heightmapSize * Vector3(0.5f, 2.0f, 0.5f));

	//light = new Light(heightmapSize * Vector3(0.5f, 1.5f, 0.5f) + Vector3(0.0f, 50.0f, 0.0f),
	//	Vector4(1, 1, 1, 1), heightmapSize.x * 0.5f);

	light = new Light_Directional(heightmapSize * Vector3(0.5f, 0.0f, 0.5f) + Vector3(-1000.0f, 300.0f, 800.0f),
		Vector4(1, 1, 1, 1), heightmapSize.x * 0.5f, heightmapSize * Vector3(0.5f, 0.0f, 0.5f));

	//light = new Light_Spot(heightmapSize * Vector3(0.5f, 1.0f, 0.5f) + Vector3(0.0f, 50.0f, 0.0f),
		//Vector4(1, 1, 1, 1), heightmapSize.x * 0.5f, 
		//heightmapSize * Vector3(0.7f, 0.0f, 0.5f), 30);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
			(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete shader;
	delete light; //This bit is new ...
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	//light->SetPosition(light->GetPosition() + Vector3( dt* 50, 0, 0)); //moving lights
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(shader);

	glUniform1i(glGetUniformLocation(
			shader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(),
			"cameraPos"), 1, (float*)& camera->GetPosition());

	UpdateShaderMatrices();
	SetShaderLight(*light);

	heightMap->Draw();
}
