#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>

#include "TextureWBump.h"


class SceneNode {
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1), Shader* shader = NULL, TextureWBump* textures = NULL);
	~SceneNode(void);
	
	void SetTransform(const Matrix4 & matrix) { 
		transform = matrix; 
		if (this->light != NULL) {
			this->light->SetPosition(matrix.GetPositionVector());
		}
	}
	const Matrix4 & GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }
	
	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }
	
	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }
	
	Mesh * GetMesh() const { return mesh; }
	void SetMesh(Mesh * m) { mesh = m; }
	
	void AddChild(SceneNode * s);
	void RemoveChild(SceneNode* s);

	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer & r);
	
	std::vector <SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();}
	std::vector <SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();}

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	int GetRenderPrior() const { return renderPrior; }
	void SetRenderPrior(int p) { renderPrior = p; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }
	GLuint GetTexture() { return textureList.front()->getTexture(); }
	GLuint GetTexture(int index) { return textureList[index]->getTexture(); }
	std::vector<TextureWBump*> * GetTextureList() { return &textureList; }
	void AddTexture(TextureWBump* pair) {
		textureList.push_back(pair);
	}
	
	GLuint GetBumpMap() const { return textureList.front()->getBumpMap(); }
	GLuint GetBumpMap(int index) const { return textureList[index]->getBumpMap(); }
	Shader* GetShader() const { return shader; }
	void SetShader(Shader* shader) { this->shader = shader;}
	

	Light* GetLight() const { return light; }
	void AddLight(Vector4 Colour, float radius) {
		this->light = new Light;
		this->light->SetColour(Colour);
		this->light->SetRadius(radius);
	}
	void SetLightPos(Vector3 pos) {
		this->light->SetPosition(pos);
	}
	void DeleteLight() {
		delete light;
	}


	static bool CompareByCameraDistance(SceneNode * a, SceneNode * b) {
		return (a->distanceFromCamera <
			b->distanceFromCamera) ? true : false;
	}

protected:
	SceneNode * parent;
	Mesh * mesh;
	Shader* shader;
	Light* light;
	std::vector<TextureWBump*> textureList;

	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector <SceneNode*> children;

	float distanceFromCamera;
	float boundingRadius;
	int renderPrior; // 0 deferred, 1 unlit, 2 animated character
};