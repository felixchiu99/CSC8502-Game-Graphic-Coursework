#include "Renderer.h"
#include "../nclgl/camera.h"
#include "../nclgl/HeightMap.h"

#include <algorithm > //For std::sort ...

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	heightMap = new HeightMap(TEXTUREDIR"noise.png");
	camera = new Camera(-40, 270, Vector3());

	Vector3 dimensions = heightMap->GetHeightmapSize();
	camera->SetPosition(dimensions * Vector3(0.5, 2, 0.5));

	shader = new Shader("TexturedVertex.glsl",
			"TexturedFragment.glsl");

	particleShader = new Shader("particleVertex.glsl", "particleFrag.glsl");

	nodeShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");

	if (!shader->LoadSuccess()|| !particleShader->LoadSuccess()) {
		return;
	}

	terrainTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG",
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!terrainTex) {
			return;
	}
		
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	
	SetTextureRepeating(terrainTex, true);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
			(float)width / (float)height, 45.0f);

	root = new SceneNode();
	SceneNode* r = new SceneNode(sphere, Vector4(1.0, 0.0, 1.0, 1), nodeShader, terrainTex);
	//SceneNode * r = new SceneNode(meshList.getMesh("cube"), Vector4(1.0, 1.0, 1.0, 1), shaderList.getShader("heightMap"));
	r->SetTransform(Matrix4::Translation(
		Vector3(dimensions * Vector3(0.5f, 0.0f, 0.5f) + Vector3(0.0f, 100.0f, 0.0f))));
	r->SetModelScale(Vector3(20.0, 20.0, 20.0));
	r->SetBoundingRadius(10000);
	root->AddChild(r);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	init = true;
}

Renderer ::~Renderer(void) {
	delete heightMap;
	delete camera;
	delete shader;
	delete root;
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	root->Update(dt);
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(shader->GetProgram(),
			"diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTex);
	heightMap->Draw();

	//
	BuildNodeLists(root);
	SortNodeLists();


	BindShader(nodeShader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(nodeShader->GetProgram(),
		"diffuseTex"), 0);
	DrawNodes();

	ClearNodeLists();

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
		glUniformMatrix4fv(glGetUniformLocation(nodeShader->GetProgram(),
			"modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(nodeShader->GetProgram(),
			"nodeColour"), 1, (float*)& n->GetColour());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, n->GetTexture());

		glUniform1i(glGetUniformLocation(nodeShader->GetProgram(),
			"useTexture"), n->GetTexture());
		n->Draw(*this);
	}
}
void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}