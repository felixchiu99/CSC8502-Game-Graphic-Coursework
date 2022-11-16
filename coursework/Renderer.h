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
	void DrawNodes(vector <SceneNode*>* RenderNodeList);
	void DrawNode(SceneNode* n);

	void DrawSkybox();
	void DrawWater();

	void RenderNode(vector <SceneNode*>* RenderNodeList);
	void FillBuffers(); //G-Buffer Fill Render Pass
	void DrawLights(); // Lighting Render Pass
	void DrawPointLights(); // Point Lighting Render Pass
	void DrawDirectionalLights(); // Directional Lighting Render Pass
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
	Shader * directionalLightShader; // Shader to calculate directional lighting
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
	Light* directionLight;
	Mesh * lightSphere; // Light volume
	Mesh * screenQuad; //To draw a full -screen quad
	Matrix4 projMatrixOriginal;

	GLuint skyboxFBO;
	GLuint skyboxTex;

	Camera * camera; //Our usual camera
	GLuint earthTex;
	GLuint earthBump;

	//scenenode related
	SceneNode* root;
	SceneNode* unlitRoot;
	SceneNode* test;

	Frustum frameFrustum;

	vector <SceneNode*> transparentNodeList;
	vector <SceneNode*> nodeList;
	vector <SceneNode*> unlitNodeList;
	vector <SceneNode*> lightNodeList;
	// END scenenode related



};