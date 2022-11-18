#include "Renderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/Light_Directional.h"
#include "../nclgl/nodeCharacter.h"
#include <algorithm > //For std::sort ...

const int LIGHT_NUM = 30;
const int TREE_NUM = 100;
const float RENDER_DIST = 10000.0f;
const int POST_PASSES = 10;

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	usingBlur = false;
	usingFreeLook = false;
	nextIndex = 0;
	SetTrack();
	shaderList = ShaderList();
	textureList = TextureList();
	meshList = MeshList();

	{
		meshList.addMesh("quad", Mesh::GenerateQuad());
		meshList.addMesh("sphere", Mesh::LoadFromMeshFile("Sphere.msh"));
		meshList.addMesh("cube", Mesh::LoadFromMeshFile("OffsetCubeY.msh"));
		meshList.addMesh("cylinder", Mesh::LoadFromMeshFile("Cylinder.msh"));
		meshList.addMesh("cone", Mesh::LoadFromMeshFile("Cone.msh"));
		meshList.addMesh("tree", Mesh::LoadFromMeshFile("green_leaf_tree.msh"));
		meshList.addMesh("man01", Mesh::LoadFromMeshFile("Role_T.msh"));
	}
	{
		shaderList.addShader("sceneNode", "bumpVertex.glsl", "bufferFragment.glsl");
		shaderList.addShader("terrain", "terrainVertex.glsl", "terrainFragment.glsl");
		shaderList.addShader("pointlightShader", "pointlightvert.glsl",
			"pointlightfrag.glsl");
		shaderList.addShader("directionalLightShader", "dirlightvert.glsl",
			"dirlightfrag.glsl");
		shaderList.addShader("combineShader", "combinevert.glsl",
			"combinefrag.glsl");
		shaderList.addShader("skybox", "skyboxVertex.glsl", "skyboxFragmentDeferred.glsl");
		shaderList.addShader("water", "reflectVertex.glsl", "reflectFragment.glsl");
		shaderList.addShader("particle", "particleVertex.glsl", "particleFrag.glsl");
		shaderList.addShader("man01", "SkinningVertex.glsl", "texturedFragment.glsl");
		shaderList.addShader("processShader", "TexturedVertex.glsl", "processfrag.glsl");
		shaderList.addShader("processCombineShader", "TexturedVertex.glsl","TexturedFragment.glsl");
	}
	{
		textureList.addTexture("earth", SOIL_load_OGL_texture(
			TEXTUREDIR"Barren Reds.JPG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS),
			SOIL_load_OGL_texture(
			TEXTUREDIR"Barren RedsDOT3.JPG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)
		);

		textureList.addTexture("treeStem", SOIL_load_OGL_texture(
			TEXTUREDIR"tree_stem1_Base_Color.JPG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS),
			SOIL_load_OGL_texture(
			TEXTUREDIR"tree_stem1_Normal_OpenGL.JPG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)
		);
		SetTextureRepeating(textureList.getTexture("treeStem"), true);
		SetTextureRepeating(textureList.getBumpMap("treeStem"), true);

		textureList.addTexture("gravel", SOIL_load_OGL_texture(
			TEXTUREDIR"Gravel01.PNG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS),
			SOIL_load_OGL_texture(
			TEXTUREDIR"Gravel01Norm.PNG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)
		);
		SetTextureRepeating(textureList.getTexture("gravel"), true);
		SetTextureRepeating(textureList.getBumpMap("gravel"), true);

		textureList.addTexture("grass", SOIL_load_OGL_texture(
			TEXTUREDIR"Grass.PNG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS),
			SOIL_load_OGL_texture(
				TEXTUREDIR"GrassNorm.PNG",
				SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)
		);
		SetTextureRepeating(textureList.getTexture("grass"), true);
		SetTextureRepeating(textureList.getBumpMap("grass"), true);


		textureList.addTexture("sand", SOIL_load_OGL_texture(
			TEXTUREDIR"Sand.PNG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS),
			SOIL_load_OGL_texture(
				TEXTUREDIR"SandNorm.PNG",
				SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)
		);
		SetTextureRepeating(textureList.getTexture("sand"), true);
		SetTextureRepeating(textureList.getBumpMap("sand"), true);


		textureList.addTexture("leafTex", SOIL_load_OGL_texture(
			TEXTUREDIR"Leaf_colour.JPG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS),
			NULL
		);
		textureList.addTexture("leafAlphaTex", SOIL_load_OGL_texture(
			TEXTUREDIR"Leaf_alpha.JPG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS),
			NULL
		);


		textureList.addTexture("brickTex", SOIL_load_OGL_texture(
			TEXTUREDIR"brick.tga",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS),
			NULL
		);
		textureList.addTexture("glass", SOIL_load_OGL_texture(
			TEXTUREDIR"stainedglass.tga",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0),
			NULL
		);
		SetTextureRepeating(textureList.getTexture("glass"), true);
		textureList.addTexture("skybox", SOIL_load_OGL_cubemap(
			TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
			TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
			TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
			SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0),
			NULL
		);
		textureList.addTexture("water", SOIL_load_OGL_texture(
			TEXTUREDIR"water.TGA",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS),
			NULL
		);
	}


	lightSphere = meshList.getMesh("sphere");
	screenQuad = meshList.getMesh("quad");

	earthTex = textureList.getTexture("earth");
	earthBump = textureList.getBumpMap("earth");

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);

	heightMap = new HeightMap(TEXTUREDIR"ridge300mod.png", Vector3(24.0f, 2.0f, 24.0f));

	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	//Vector3 heightmapSize = Vector3(4096, 255, 4096);
	
	{
		root = new SceneNode();
		unlitRoot = new SceneNode();

		SceneNode* heightMapScene = new SceneNode(heightMap, Vector4(1.0, 1.0, 1.0, 1.0), shaderList.getShader("terrain"), textureList.getTexturePair("earth"));
		heightMapScene->AddTexture(textureList.getTexturePair("sand"));
		heightMapScene->AddTexture(textureList.getTexturePair("gravel"));
		heightMapScene->AddTexture(textureList.getTexturePair("grass"));
		heightMapScene->SetTransform(Matrix4::Translation(
			Vector3(heightmapSize* Vector3(-0.5f, 0.0f, -0.5f))));
		heightMapScene->SetBoundingRadius(heightmapSize.x * 2);
		heightMapScene->SetRenderPrior(-1);
		root->AddChild(heightMapScene);

		SceneNode* r = new SceneNode(meshList.getMesh("cube"), Vector4(1.0, 0.0, 1.0, 1), shaderList.getShader("sceneNode"), textureList.getTexturePair("gravel"));
		//SceneNode * r = new SceneNode(meshList.getMesh("cube"), Vector4(1.0, 1.0, 1.0, 1), shaderList.getShader("heightMap"));
		r->SetTransform(Matrix4::Translation(
			Vector3( 0.0f, 100.0f, 0.0f)));
		r->SetModelScale(Vector3(20.0, 20.0, 20.0));
		r->SetBoundingRadius(RENDER_DIST);
		r->SetRenderPrior(1);
		root->AddChild(r);

		r = new SceneNode(meshList.getMesh("sphere"), Vector4(1.0, 1.0, 1.0, 1), shaderList.getShader("sceneNode"), textureList.getTexturePair("earth"));
		r->SetTransform(Matrix4::Translation(
			Vector3(0.0f, 100.0f, 0.0f)));
		r->SetBoundingRadius(RENDER_DIST);
		r->SetModelScale(Vector3(20.0, 20.0, 20.0));
		root->AddChild(r);

		r = new SceneNode(meshList.getMesh("cone"), Vector4(1.0, 1.0, 1.0, 1), shaderList.getShader("sceneNode"), textureList.getTexturePair("earth"));
		r->SetTransform(Matrix4::Translation(
			Vector3( 50.0f, 100.0f, 0.0f)));
		r->SetBoundingRadius(RENDER_DIST);
		r->SetModelScale(Vector3(20.0, 20.0, 20.0));
		root->AddChild(r);

		r = new SceneNode(meshList.getMesh("tree"), Vector4(1.0, 1.0, 1.0, 1), shaderList.getShader("sceneNode"), textureList.getTexturePair("treeStem"));
		r->AddTexture(textureList.getTexturePair("leafTex"));
		r->SetTransform(Matrix4::Translation(
			Vector3( 0.0f, 220.0f, 0.0f)));
		r->SetBoundingRadius(RENDER_DIST);
		r->SetModelScale(Vector3(100.0, 100.0, 100.0));
		root->AddChild(r);

		r = new SceneNode(meshList.getMesh("sphere"), Vector4(1.0, 1.0, 1.0, 1), shaderList.getShader("sceneNode"), textureList.getTexturePair("earth"));
		r->SetTransform(Matrix4::Translation(
			Vector3( 0.0f, 220.0f, 400.0f)));
		r->SetBoundingRadius(RENDER_DIST);
		r->SetModelScale(Vector3(100.0, 100.0, 100.0));
		r->AddLight(Vector4 (1,0.7,0.7,1), 250);
		//r->SetRenderPrior(1);
		root->AddChild(r);
		test = r;

		r = new SceneNode(meshList.getMesh("tree"), Vector4(1.0, 1.0, 1.0, 1), shaderList.getShader("sceneNode"), textureList.getTexturePair("earth"));
		r->SetTransform(Matrix4::Translation(
			Vector3(100.0f, 40.0f, 0.0f)));
		r->SetBoundingRadius(RENDER_DIST);
		r->SetModelScale(Vector3(100.0, 100.0, 100.0));
		r->AddLight(Vector4(0.5, 0.5, 1, 1), 250);
		r->SetRenderPrior(1);
		root->AddChild(r);

		r = new SceneNode(meshList.getMesh("tree"), Vector4(1.0, 1.0, 1.0, 1), shaderList.getShader("processCombineShader"), textureList.getTexturePair("glass"));
		r->SetTransform(Matrix4::Translation(
			Vector3(400.0f, (*heightMap).GetHeight(Vector3(400.0f,0, 400.0f), Vector3(0.5f, 0.0f, 0.5f)), 400.0f)));
		r->SetBoundingRadius(RENDER_DIST);
		r->SetModelScale(Vector3(100.0, 100.0, 100.0));
		r->AddLight(Vector4(0.5, 0.5, 1, 1), 250);
		r->SetRenderPrior(1);
		root->AddChild(r);

		for(int i = 0; i < TREE_NUM; i++){
			r = new SceneNode(meshList.getMesh("tree"), Vector4(1.0, 1.0, 1.0, 1), shaderList.getShader("sceneNode"), textureList.getTexturePair("treeStem"));
			Vector3 Translation = Vector3(rand() % (int)heightmapSize.x - (int)heightmapSize.x / 2,  0, rand() % (int)heightmapSize.z - (int)heightmapSize.z / 2);
			r->SetTransform(Matrix4::Translation(
				Vector3(Translation.x, (*heightMap).GetHeight(Translation, Vector3(-0.5f, 0.0f, -0.5f)), Translation.z)));
			r->SetBoundingRadius(RENDER_DIST);
			r->SetModelScale(Vector3(100.0, 100.0, 100.0));
			Vector4 colour = Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
				0.5f + (float)(rand() / (float)RAND_MAX),
				0.5f + (float)(rand() / (float)RAND_MAX),
				1);
			if(rand() % 3)
				r->AddLight(colour, 250);
			r->SetColour(colour);
			root->AddChild(r);
		}

		r = new nodeCharacter("Role_T.anm", "Role_T.mat", meshList.getMesh("man01"), shaderList.getShader("man01"));
		r->SetTransform(Matrix4::Translation(
			Vector3(100.0f, 220.0f, 0.0f)));
		r->SetBoundingRadius(RENDER_DIST);
		r->SetModelScale(Vector3(50.0, 50.0, 50.0));
		r->AddLight(Vector4(0.5, 0, 0, 1), 250);
		r->SetRenderPrior(2);
		root->AddChild(r);
		man = r;

	}
	projMatrixOriginal = Matrix4::Perspective(1.0f, 100000.0f,
		(float)width / (float)height, 45.0f);
	projMatrix = projMatrixOriginal;

	camera = new Camera(-7.0f, 180, Vector3(-168, 212, -1261));
	pointLights = new Light[LIGHT_NUM+1];

	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light & l = pointLights[i];
		l.SetPosition(Vector3(rand() % (int)heightmapSize.x - (int)heightmapSize.x/2,
			150.0f,
			rand() % (int)heightmapSize.z - (int)heightmapSize.z / 2));
		
		l.SetColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
				0.5f + (float)(rand() / (float)RAND_MAX),
				0.5f + (float)(rand() / (float)RAND_MAX),
				1));
		l.SetRadius(250.0f + (rand() % 250));
	}
	directionLight = new Light_Directional(Vector3(200.0f, 200.0f, 200.0f),
		Vector4(1, 1, 1, 1), heightmapSize.x * -0.5f, heightmapSize * Vector3(0.0f, 0.0f, 0.0f));
	directionLight->SetRadius(5000);
	//directionLight->SetIntensity(0.7);
	directionLight->SetIntensity(0.5);
	std::cout << directionLight->GetRadius() << std::endl;
	
	if (shaderList.shaderLoadError()) {
		return;
	}
	sceneShader = shaderList.getShader("heightMap");
	pointlightShader = shaderList.getShader("pointlightShader");
	directionalLightShader = shaderList.getShader("directionalLightShader");
	combineShader = shaderList.getShader("combineShader");

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &processFBO);//And do post processing in this
	
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
	
	//And our post process colour texture ...
	glGenTextures(1, &postBufferColourTex);
	glBindTexture(GL_TEXTURE_2D, postBufferColourTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);

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
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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
	glDeleteTextures(1, &postBufferColourTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteFramebuffers(1, &processFBO);
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
	camera->setFreeLook(usingFreeLook);
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	Vector3 Translation = man->GetTransform().GetPositionVector() + Vector3(0, 0, 1);
	man->SetTransform(Matrix4::Translation(
		Vector3(Translation.x, (*heightMap).GetHeight(Translation, Vector3(-0.5f, 0.0f, -0.5f)), Translation.z)));

	if (intensity >= 0.8) {
		lightDim = true;
	}
	if (intensity <= 0) {
		lightDim = false;
	}
	if (lightDim) {
		intensity -= 0.0005; 
		directionLight->SetPosition(directionLight->GetDirection() + Vector3(0.01, 0.02, 0.01));
		test->SetTransform(test->GetTransform() * Matrix4::Translation(
			Vector3(0.5f, 0.01f, 0.0f)));
	}
	else {
		intensity += 0.0005;
		directionLight->SetPosition(directionLight->GetDirection() - Vector3(0.01, 0.02, 0.01));
		test->SetTransform(test->GetTransform() * Matrix4::Translation(
			Vector3(-0.5f, 0.01f, 0.0f)));
	}
	directionLight->SetIntensity(intensity);

	root->Update(dt);

	std::cout <<"pos "<< camera->GetPosition() << std::endl;
	std::cout <<"Yaw "<< camera->GetYaw() << std::endl;
	std::cout <<"Pitch "<< camera->GetPitch() << std::endl;

	if(!usingFreeLook)
		MoveCamera(dt);
}
void Renderer::SetTrack() {
	trackCheckpoint.push_back(
		CameraTrack(
			Vector3(-168, 212, -1261),
			-7, 180, 3, 0.01
	));
	trackCheckpoint.push_back(
		CameraTrack(
			Vector3(-130, 245.07, 0),
			-7, 180, 3, 0.01
		));
	trackCheckpoint.push_back(
			CameraTrack(
				Vector3(-109.158, 245.07, 1001.02),
				-7, 222, 3, 0.002
			));
	trackCheckpoint.push_back(
		CameraTrack(
			Vector3(95.3522, 236.726, 2614.89),
			-7, 330, 3, 0.005
		));
	trackCheckpoint.push_back(
		CameraTrack(
			Vector3(1690.98, 300, 1962.43),
			-10, 3, 3, 0.01
		));
	trackCheckpoint.push_back(
		CameraTrack(
			Vector3(360, 500, -661),
			-20, 100, 3, 0.01
		));
}
float compareAngle(float target, float current) {
	float currentOpposite;
	if (current < 180)
		currentOpposite = current + 180;
	else
		currentOpposite = current - 180;
	if ((target - current) > 180)
		return -(target - currentOpposite);
	if ((target - current) < -180)
		return -(target - currentOpposite);
	return (target - current);
}
void Renderer::MoveCamera(float dt) {
	CameraTrack target = trackCheckpoint[nextIndex];

	Vector3 targetPos = target.GetPosition();
	Vector3 currentPos = camera->GetPosition();
	Vector3 next = targetPos - currentPos; 
	float dist = next.Length();
	std::cout << next.Length() << std::endl;
	next = next.Normalised();
	next = next * target.GetPositionSpeed();
	if(dist > (1 * target.GetPositionSpeed()))
		camera->SetPosition(currentPos + next );
	else {
		if (nextIndex < trackCheckpoint.size() - 1)
			nextIndex++;
		else
			nextIndex = 0;
	}

	float yawDiff = compareAngle(target.GetYaw(), camera->GetYaw());
	float nextYaw = compareAngle(target.GetYaw(), camera->GetYaw())* target.GetViewSpeed();
	if (yawDiff > fabs(nextYaw))
		camera->SetYaw(camera->GetYaw() + nextYaw);


	float pitchDiff = compareAngle(target.GetPitch(), camera->GetPitch());
	float nextPitch = compareAngle(target.GetPitch(), camera->GetPitch()) * target.GetViewSpeed();
	if (pitchDiff > fabs(nextPitch))
		camera->SetPitch(camera->GetPitch() + nextPitch);
}

void Renderer::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();

	//deferred rendering
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	FillBuffers();
	DrawLights();
	CombineBuffers();

	//forward rendering
	glBlitNamedFramebuffer(bufferFBO, 0,
		0, 0, width, height,
		0, 0, width, height,
		GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = projMatrixOriginal;
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	DrawWater();

	RenderNode(&unlitNodeList);

	if (usingBlur) {
		DrawPostProcess();
		CombinePostProcessScene();
	}

	ClearNodeLists();

}
//*/
void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(shaderList.getShader("skybox"));
	UpdateShaderMatrices();
	glUniform1f(glGetUniformLocation(shaderList.getShader("skybox")->GetProgram(),
		"intensity"), intensity);
	meshList.getMesh("quad")->Draw();

	glDepthMask(GL_TRUE);
}
void Renderer::DrawWater() {
	BindShader(shaderList.getShader("water"));

	glUniform3fv(glGetUniformLocation(shaderList.getShader("water")->GetProgram(),
		"cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(
		shaderList.getShader("water")->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(
		shaderList.getShader("water")->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureList.getTexture("water"));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureList.getTexture("skybox"));

	glUniform1f(glGetUniformLocation(shaderList.getShader("water")->GetProgram(),
		"intensity"), intensity);

	Vector3 hSize = heightMap->GetHeightmapSize();

	modelMatrix =
		Matrix4::Translation( Vector3(0.0f, 21.5f, 0.0f)) *
		Matrix4::Scale(hSize * 0.5f) *
		Matrix4::Rotation(-90, Vector3(1, 0, 0));

	UpdateShaderMatrices();
	// SetShaderLight (* light); //No lighting in this shader!
	meshList.getMesh("quad")->Draw();
}


void Renderer::RenderNode(vector <SceneNode*> *RenderNodeList) {
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	DrawNodes(RenderNodeList);

	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

}

void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = projMatrixOriginal;

	UpdateShaderMatrices();

	DrawSkybox();

	DrawTerrain(terrain);

	RenderNode(&nodeList);

	DrawAnimatedNodes();

	//DrawWater();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	DrawPointLights();
	DrawDirectionalLights();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::DrawPointLights() {
	BindShader(pointlightShader);
	
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

	for (int i = 0; i < LIGHT_NUM; ++i) {
		Light & l = pointLights[i];
		SetShaderLight(l);
		lightSphere->Draw();
	}
	for (const auto& i : lightNodeList) {
		Light* l = i->GetLight();
		SetShaderLight(*l);
		lightSphere->Draw();
	}

}

void Renderer::DrawDirectionalLights() {
	BindShader(directionalLightShader);

	glUniform1i(glGetUniformLocation(
		directionalLightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(
		directionalLightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(directionalLightShader->GetProgram(),
		"cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform2f(glGetUniformLocation(directionalLightShader->GetProgram(),
		"pixelSize"), 1.0f / width, 1.0f / height);
	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(
		directionalLightShader->GetProgram(), "inverseProjView"),
		1, false, invViewProj.values);
	UpdateShaderMatrices();

	SetShaderLight(*directionLight);
	lightSphere->Draw();
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
///*
void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, postBufferColourTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	Shader* processShader = shaderList.getShader("processShader");
	BindShader(processShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(
		processShader->GetProgram(), "sceneTex"), 0);
	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, postBufferColourTex, 0);
		glUniform1i(glGetUniformLocation(processShader->GetProgram(),
			"isVertical"), 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex);
		screenQuad->Draw();
		//Now to swap the colour buffers , and do the second blur pass
		glUniform1i(glGetUniformLocation(processShader->GetProgram(),
			"isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex, 0);
		glBindTexture(GL_TEXTURE_2D, postBufferColourTex);
		screenQuad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}
void Renderer::CombinePostProcessScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	Shader* sceneShader = shaderList.getShader("processCombineShader");
	BindShader(sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);
	glUniform1i(glGetUniformLocation(
		sceneShader->GetProgram(), "diffuseTex"), 0);
	screenQuad->Draw();
}
//*/

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));
		Light* thisLight = from->GetLight();
		if (thisLight != NULL) {
			from->SetLightPos(from->GetWorldTransform().GetPositionVector());
			lightNodeList.push_back(from);
		}
		if (from->GetRenderPrior() == 1) {
			unlitNodeList.push_back(from);
		}else if (from->GetRenderPrior() == 2) {
			animatedCharacterList.push_back(from);
		}else if (from->GetRenderPrior() == -1) {
			terrain = from;
		}
		else if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}else {
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
	std::sort(unlitNodeList.begin(),
		unlitNodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes(vector <SceneNode*> *RenderNodeList) {
	for (const auto& i : *RenderNodeList) {
		DrawNode(i);
	}
}
void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Shader* shader = n->GetShader();
		BindShader(shader);
		UpdateShaderMatrices();

		/*Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());*/
		Matrix4 model = n->GetWorldTransform();
		model.SetScalingVector(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(),
			"modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(shader->GetProgram(),
			"colour"), 1, (float*)& n->GetColour());

		glUniform1i(glGetUniformLocation(shader->GetProgram(),
			"diffuseTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, n->GetTexture());

		glUniform1i(
			glGetUniformLocation(shader->GetProgram(), "bumpTex"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, n->GetBumpMap());

		n->Draw(*this);
	}
}

void Renderer::DrawAnimatedNodes() {
	for (const auto& i : animatedCharacterList) {
		DrawCharacter(i);
	}
}
void Renderer::DrawCharacter(SceneNode* n) {
	BindShader(n->GetShader());
	glUniform1i(glGetUniformLocation(n->GetShader()->GetProgram(),
		"diffuseTex"), 0);
	UpdateShaderMatrices();
	Matrix4 model = n->GetWorldTransform();
	model.SetScalingVector(n->GetModelScale());
	glUniformMatrix4fv(glGetUniformLocation(n->GetShader()->GetProgram(),
		"modelMatrix"), 1, false, model.values);
	n->Draw(*this);
}

void Renderer::DrawTerrain(SceneNode* n) {
	if (n->GetMesh()) {
		Shader* shader = n->GetShader();
		BindShader(shader);
		UpdateShaderMatrices();

		Matrix4 model = n->GetWorldTransform();
		model.SetScalingVector(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(),
			"modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(shader->GetProgram(),
			"colour"), 1, (float*)& n->GetColour());

		int count = 0;
		std::vector<TextureWBump*>* textureList = n->GetTextureList();

		for (auto& texture : *textureList) // access by reference to avoid copying
		{
			std::string text = "diffuseTex";
			text += std::to_string(count);
			glUniform1i(glGetUniformLocation(shader->GetProgram(),
				text.c_str()), count*2);
			glActiveTexture(GL_TEXTURE0 + count*2);
			glBindTexture(GL_TEXTURE_2D, texture->getTexture());

			text = "bumpTex";
			text += std::to_string(count);
			glUniform1i(glGetUniformLocation(shader->GetProgram(), 
				text.c_str()), count * 2 + 1);
			glActiveTexture(GL_TEXTURE0 + count*2 + 1);
			glBindTexture(GL_TEXTURE_2D, texture->getBumpMap());
			
			count++;
		}

		n->Draw(*this);
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear(); 
	unlitNodeList.clear();
	lightNodeList.clear();
	animatedCharacterList.clear();
}