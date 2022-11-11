#include "Renderer.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();
	camera = new Camera();

	texture[0] = SOIL_load_OGL_texture(TEXTUREDIR "brick.tga",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!texture[0]) {
		return;
	}
	texture[1] = SOIL_load_OGL_texture(TEXTUREDIR "stainedglass.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!texture[1]) {
		return;
	}
	shader = new Shader("TexturedVertex.glsl", "texturedfragment.glsl");
	if (!shader->LoadSuccess()) {
		return;
	}
	filtering = true;
	repeating = false;
	glEnable(GL_BLEND);
	init = true;
}
Renderer ::~Renderer(void) {
	delete triangle;
	delete shader;
	delete camera;
	glDeleteTextures(1, texture);
}
void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
			"diffuseTex"), 0); //this last parameter
	glActiveTexture(GL_TEXTURE0); // should match this number!
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex2"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	triangle->Draw();
}
void Renderer::UpdateTextureMatrix(float value) {
	Matrix4 push = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 pop = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pop * rotation * push;
}
void Renderer::ToggleRepeating() {
	repeating = !repeating;
	SetTextureRepeating(texture[0], repeating);
}
void Renderer::ToggleFiltering() {
	filtering = !filtering;
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		filtering ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
}
void Renderer::SwitchToPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45);
}
void Renderer::SwitchToOrthographic() {
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,
		width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}