#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();
	
	basicShader = new Shader("basicVertex.glsl", "colourFragment.glsl");
	if (!basicShader->LoadSuccess()) {
		return;
	}

	int* someMemory = new int[3];
	someMemory[0] = 255;
	someMemory[1] = 255;
	someMemory[2] = 255;

	init = true;
}
Renderer ::~Renderer(void) {
	delete triangle;
	delete basicShader;
}
void Renderer::RenderScene() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	BindShader(basicShader);
	triangle->Draw();
}