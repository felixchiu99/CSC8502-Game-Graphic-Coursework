#pragma once
#include "..\nclgl\SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include <string>

class nodeCharacter : public SceneNode {
	public:
		nodeCharacter(std::string meshAni, std::string meshMat, Mesh* mesh, Shader * shader);
		~nodeCharacter(void);
		void Update(float dt) override;
		void Draw(const OGLRenderer& r);
	protected:
		MeshAnimation* anim;
		MeshMaterial* material;
		vector <GLuint > matTextures;
		int currentFrame;
		float frameTime;
};

