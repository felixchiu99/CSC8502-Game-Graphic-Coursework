#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
class HeightMap;
class Camera;

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;


protected:
	HeightMap * heightMap;
	Shader * shader;
	Shader * particleShader;
	Shader * nodeShader;
	Mesh* sphere;
	Camera * camera;
	GLuint terrainTex;
	SceneNode* root;

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	Frustum frameFrustum;

	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;
};

