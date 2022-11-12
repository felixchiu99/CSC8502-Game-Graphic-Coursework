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
class HeightMap;

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);
	
	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	//NodeLists
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	void FillBuffers(); //G-Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass
	//Make a new texture ...
	void GenerateScreenTexture(GLuint & into, bool depth = false);
	
	//mesh, shaders, texture maps
	ShaderList shaderList;
	TextureList textureList;
	MeshList meshList;
	//END mesh, shaders, texture maps


	Shader * sceneShader; // Shader to fill our GBuffers
	Shader * pointlightShader; // Shader to calculate lighting
	Shader * combineShader; // shader to stick it all together

	GLuint bufferFBO; //FBO for our G-Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here

	GLuint pointLightFBO; //FBO for our lighting pass
	GLuint lightDiffuseTex; // Store diffuse lighting
	GLuint lightSpecularTex; // Store specular lighting
	HeightMap* heightMap; // Terrain!
	Light * pointLights; // Array of lighting data
	Mesh * sphere; // Light volume
	Mesh * quad; //To draw a full -screen quad

	Camera * camera; //Our usual camera
	GLuint earthTex;
	GLuint earthBump;

	//scenenode related
	SceneNode* root;

	Frustum frameFrustum;

	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;
	// END scenenode related



};