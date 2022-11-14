#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>


class SceneNode {
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1), Shader* shader = NULL, GLuint texture = NULL, GLuint bumpMap = NULL);
	~SceneNode(void);
	
	void SetTransform(const Matrix4 & matrix) { transform = matrix; }
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

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }
	GLuint GetTexture() { return textureList.front(); }
	std::vector<GLuint> * GetTextureList() { return &textureList; }
	void SetTexture(GLuint tex) {
		textureList.push_back(tex);
	}
	
	GLuint GetBumpMap() const { return bumpMap; }
	void SetBumpMap(GLuint bump) {
		bumpMap = bump;
	}
	
	Shader* GetShader() const { return shader; }
	void SetShader(Shader* shader) { this->shader = shader;}
	
	static bool CompareByCameraDistance(SceneNode * a, SceneNode * b) {
		return (a->distanceFromCamera <
			b->distanceFromCamera) ? true : false;
	}

protected:
	SceneNode * parent;
	Mesh * mesh;
	Shader* shader;
	std::vector<GLuint> textureList;
	GLuint bumpMap;

	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector <SceneNode*> children;

	float distanceFromCamera;
	float boundingRadius;
};