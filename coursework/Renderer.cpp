#include "Renderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/CubeRobot.h"
#include "../nclgl/Light_Directional.h"
#include <algorithm > //For std::sort ...

const int LIGHT_NUM = 200;
const float RENDER_DIST = 10000.0f;


Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	shaderList = ShaderList();
	textureList = TextureList();
	meshList = MeshList();

	meshList.addMesh("quad", Mesh::GenerateQuad());
	meshList.addMesh("sphere", Mesh::LoadFromMeshFile("Sphere.msh"));
	meshList.addMesh("cube", Mesh::LoadFromMeshFile("OffsetCubeY.msh"));

	lightSphere = meshList.getMesh("sphere");
	screenQuad = meshList.getMesh("quad");

	shaderList.addShader("sceneNode", "SceneVertex.glsl", "SceneFragment.glsl");
	shaderList.addShader("heightMap", "bumpVertex.glsl", "bufferFragment.glsl");
	shaderList.addShader("pointlightShader", "pointlightvert.glsl",
		"pointlightfrag.glsl");
	shaderList.addShader("simpleLight","PerPixelVertex.glsl", "PerPixelFragment.glsl");
	shaderList.addShader("combineShader","combinevert.glsl",
		"combinefrag.glsl");

	heightMap = new HeightMap(TEXTUREDIR"ridge300mod.png", Vector3(24.0f, 2.0f, 24.0f));
	test = meshList.getMesh("sphere");


	textureList.addTexture("earthTex", SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	textureList.addTexture("brickTex", SOIL_load_OGL_texture(
		TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	textureList.addTexture("earthBump", SOIL_load_OGL_texture(
		TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	earthTex = textureList.getTexture("earthTex");
	
	earthBump = textureList.getTexture("earthBump");

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	//Vector3 heightmapSize = Vector3(4096, 255, 4096);
	
	root = new SceneNode();

	SceneNode * r = new SceneNode(meshList.getMesh("cube"), Vector4(1.0, 1.0, 1.0, 1), shaderList.getShader("heightMap"), textureList.getTexture("brickTex"));
	r->SetTransform(Matrix4::Translation(
		Vector3(heightmapSize * Vector3(0.5f, 0.0f, 0.5f) + Vector3(0.0f, 100.0f, 0.0f))));
	r->SetModelScale(Vector3(20.0, 20.0, 20.0));
	r->SetBoundingRadius(RENDER_DIST);
	root->AddChild(r);

	r = new SceneNode(meshList.getMesh("sphere"),Vector4(1.0,1.0,1.0,1), shaderList.getShader("heightMap"), textureList.getTexture("earthTex"));
	r->SetTransform(Matrix4::Translation(
		Vector3(heightmapSize * Vector3(0.55f, 0.0f, 0.5f) + Vector3(0.0f, 100.0f, 0.0f))));
	r->SetBoundingRadius(RENDER_DIST);
	r->SetModelScale(Vector3 (20.0, 20.0, 20.0));
	root->AddChild(r);

	SceneNode * heightMapScene = new SceneNode(heightMap, Vector4(1.0,1.0,1.0,1.0), shaderList.getShader("heightMap"), textureList.getTexture("earthTex"));
	heightMapScene->SetBoundingRadius(heightmapSize.x * 2);
	root->AddChild(heightMapScene);

	projMatrixOriginal = Matrix4::Perspective(1.0f, 100000.0f,
		(float)width / (float)height, 45.0f);
	projMatrix = projMatrixOriginal;

	camera = new Camera(-45.0f, 0.0f,
			heightmapSize * Vector3(0.5f, 5.0f, 0.5f));
	pointLights = new Light[LIGHT_NUM+1];

	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light & l = pointLights[i];
		l.SetPosition(Vector3(rand() % (int)heightmapSize.x,
			150.0f,
			rand() % (int)heightmapSize.z));
		
		l.SetColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
				0.5f + (float)(rand() / (float)RAND_MAX),
				0.5f + (float)(rand() / (float)RAND_MAX),
				1));
		l.SetRadius(250.0f + (rand() % 250));
	}
	directionLight = new Light_Directional(heightmapSize * Vector3(0.5f, 0.0f, 0.5f) + Vector3(200.0f, 300.0f, 200.0f),
		Vector4(1, 1, 1, 1), heightmapSize.x * 0.5f, heightmapSize * Vector3(0.5f, 0.0f, 0.5f));
	directionLight->SetRadius(5000);
	std::cout << directionLight->GetRadius() << std::endl;
	
	if (shaderList.shaderLoadError()) {
		return;
	}
	sceneShader = shaderList.getShader("heightMap");
	pointlightShader = shaderList.getShader("pointlightShader");
	combineShader = shaderList.getShader("combineShader");

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	
	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0 ,
		GL_COLOR_ATTACHMENT1
	};

	// Generate our scene depth texture ...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);

	//And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
			GL_FRAMEBUFFER_COMPLETE) {
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
			GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	init = true;
}
Renderer ::~Renderer(void) {
	//delete heightMap;
	delete root;
	delete camera;
	delete[] pointLights;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
}
void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;
	
	glTexImage2D(GL_TEXTURE_2D, 0,
			format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	root->Update(dt);
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BuildNodeLists(root);
	SortNodeLists();
	FillBuffers();
	DrawPointLights();
	CombineBuffers();
	
}
//*/

void Renderer::RenderNode() {
	BuildNodeLists(root);
	SortNodeLists();
	/*
	BindShader(shaderList.getShader("sceneNode"));
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shaderList.getShader("sceneNode")->GetProgram(),
		"diffuseTex"), 0);
	*/
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	DrawNodes();

	ClearNodeLists();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
/*
void Renderer::FillBuffers() {
	//std::cout << "d" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(sceneShader);
	glUniform1i(
		glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(
		glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = projMatrixOriginal;

	UpdateShaderMatrices();

	//heightMap->Draw();
	RenderNode();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
*/
void Renderer::FillBuffers() {
	//std::cout << "d" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = projMatrixOriginal;

	UpdateShaderMatrices();

	RenderNode();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointlightShader);
	
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);
	
	glUniform1i(glGetUniformLocation(
			pointlightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	
	glUniform1i(glGetUniformLocation(
			pointlightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);
	
	glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(),
			"cameraPos"), 1, (float*)& camera->GetPosition());
	
	glUniform2f(glGetUniformLocation(pointlightShader->GetProgram(),
			"pixelSize"), 1.0f / width, 1.0f / height);
	
	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(
		pointlightShader->GetProgram(), "inverseProjView"),
		1, false, invViewProj.values);
	UpdateShaderMatrices();
	SetShaderLight(*directionLight);
	lightSphere->Draw();
	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light & l = pointLights[i];
		SetShaderLight(l);
		lightSphere->Draw();
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffers() {
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(
			combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);
	
	glUniform1i(glGetUniformLocation(
			combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(
			combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	screenQuad->Draw();
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));
		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
			//std::cout << "c" << std::endl;
		}
		else {

			nodeList.push_back(from);
		}
	}
	for (vector <SceneNode*>::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));

	}
}
void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), //note the r! for reverse
		transparentNodeList.rend(), //note the r!
		SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}
void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}
void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Shader* shader = n->GetShader();
		BindShader(shader);
		UpdateShaderMatrices();

		glUniform1i(glGetUniformLocation(shader->GetProgram(),
			"diffuseTex"), 0);

		Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(),
			"modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(shader->GetProgram(),
			"nodeColour"), 1, (float*)& n->GetColour());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, n->GetTexture());
		//glBindTexture(GL_TEXTURE_2D, earthTex);

		glUniform1i(
			glGetUniformLocation(shader->GetProgram(), "bumpTex"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, earthBump);

		glUniform1i(glGetUniformLocation(shader->GetProgram(),
			"useTexture"), n->GetTexture());


		n->Draw(*this);
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}