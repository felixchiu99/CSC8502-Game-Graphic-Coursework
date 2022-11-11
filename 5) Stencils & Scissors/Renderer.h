#pragma once

#include "../nclgl/OGLRenderer.h"
class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);
	
	void RenderScene() override;
	
	void ToggleScissor();
	void ToggleStencil();
	
protected:
	GLuint textures[2];
	Mesh * meshes[2];
	Shader * shader;
	
	bool usingScissor;
	bool usingStencil;

};