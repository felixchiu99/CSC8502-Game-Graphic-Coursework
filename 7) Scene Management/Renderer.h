#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/ShaderList.h"
#include "../nclgl/TextureList.h"
#include "../nclgl/MeshList.h"
class Camera;
class SceneNode;
class Mesh;
class Shader;

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);

	void UpdateScene(float msec) override;
	void RenderScene() override;
	
protected:
	void BuildNodeLists(SceneNode * from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode * n);
	
	SceneNode * root;
	Camera * camera;
	Shader * shader;
	GLuint texture;
	ShaderList shaderList;
	TextureList textureList;
	MeshList meshList;

	Frustum frameFrustum;

	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;

};