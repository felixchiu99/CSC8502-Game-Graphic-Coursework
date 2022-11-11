#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	virtual void RenderScene();
	void UpdateTextureMatrix(float rotation);
	void ToggleRepeating();
	void ToggleFiltering();
	virtual void UpdateScene(float dt);
	void SwitchToPerspective();
	void SwitchToOrthographic();
protected:
	Shader * shader;
	Mesh * triangle;
	GLuint texture [2];
	bool filtering;
	bool repeating;
	Camera* camera;
};