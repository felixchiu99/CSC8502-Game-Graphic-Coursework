#include "Renderer.h"
#include "../nclgl/CubeRobot.h"
#include "../nclgl/Camera.h"
#include <algorithm > //For std::sort ...

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera(0.0f, 0.0f, (Vector3(0, 100, 750.0f)));
	shaderList = ShaderList();
	textureList = TextureList();
	meshList = MeshList();
	meshList.addMesh("quad", Mesh::GenerateQuad());
	meshList.addMesh("cube",Mesh::LoadFromMeshFile("OffsetCubeY.msh"));

	(&shaderList)->addShader("default", "SceneVertex.glsl", "SceneFragment.glsl");

	(&textureList)->addTexture("default", SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	(&textureList)->addTexture("brick", SOIL_load_OGL_texture(TEXTUREDIR"brick.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	//shader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	shader = shaderList.getShader("default");
	//texture = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga",
		//SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	texture = textureList.getTexture("default");


	if (!shader->LoadSuccess() || !texture) {
		return;
	}
	
	root = new SceneNode();

	for (int i = 0; i < 5; ++i) {
		SceneNode * s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(
			Vector3(0, 100.0f, -300.0f + 100.0f + 100 * i)));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetBoundingRadius(100.0f);
		s->SetMesh(meshList.getMesh("quad"));
		if(i == 4)
			texture = textureList.getTexture("brick");
		s->SetTexture(texture);
		root->AddChild(s);
	}
	SceneNode* s2 = new SceneNode();
	s2->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	s2->SetTransform(Matrix4::Translation(
		Vector3(0, 100.0f, -300.0f + 100.0f + 100 * 5)));
	s2->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	s2->SetBoundingRadius(100.0f);
	s2->SetMesh(meshList.getMesh("cube"));
	s2->SetTexture(texture);
	root->AddChild(s2);

	root->AddChild(new CubeRobot(meshList.getMesh("cube"), shaderList.getShader("default")));

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
			(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init = true;
}

Renderer ::~Renderer(void) {
	delete root;
	delete camera;
	//delete shader;
	glDeleteTextures(1, &texture);
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	root->Update(dt);
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));
		
		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);

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
		Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shaderList.getShader("default")->GetProgram(),
			"modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(shaderList.getShader("default")->GetProgram(),
			"nodeColour"), 1, (float*)& n->GetColour());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, n->GetTexture());

		glUniform1i(glGetUniformLocation(shaderList.getShader("default")->GetProgram(),
			"useTexture"), n->GetTexture());
		n->Draw(*this);
	}
}
void Renderer::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(shaderList.getShader("default"));
	UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(shaderList.getShader("default")->GetProgram(),
			"diffuseTex"), 0);
	DrawNodes();
	
	ClearNodeLists();
}
void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}