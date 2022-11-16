#include "SceneNode.h"

SceneNode::SceneNode(Mesh * mesh, Vector4 colour, Shader * shader , GLuint texture, GLuint bumpMap) {
	this->renderPrior = 0;

	this->mesh = mesh;
	this->colour = colour;

	this->shader = shader;
	textureList.push_back(texture);
	this->bumpMap = bumpMap;
	this->light = NULL;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
}

SceneNode ::~SceneNode(void) {
	if (light != NULL) {
		delete light;
		light = NULL;
	}
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode * s) {
	if (s == this) {
		return;
	}
	children.push_back(s);
	s->parent = this;
}

void SceneNode::RemoveChild(SceneNode* s) {
	for (vector <SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		if (*i == s) {
			children.erase(i);
		}
	}
}

void SceneNode::Draw(const OGLRenderer& r) {
	if (mesh) { mesh->Draw(); };
}
void SceneNode::Update(float dt) {
	if (parent) { //This node has a parent ...
		worldTransform = parent->worldTransform * transform;
	}
	else { //Root node , world transform is local transform!
		worldTransform = transform;
	}
	for (vector <SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(dt);
	}
}