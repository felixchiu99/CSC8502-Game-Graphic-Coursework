#include "nodeCharacter.h"

nodeCharacter::nodeCharacter(std::string meshAni, std::string meshMat, Mesh * mesh, Shader * shader) {
	anim = new MeshAnimation(meshAni);
	material = new MeshMaterial(meshMat);
	this->mesh = mesh;
	this->shader = shader;
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry =
			material->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);

	}
}
nodeCharacter::~nodeCharacter(void) {
	delete anim;
	delete material;
};

void nodeCharacter::Draw(const OGLRenderer& r) {
	vector <Matrix4 > frameMatrices;

	const Matrix4* invBindPose = mesh->GetInverseBindPose();
	const Matrix4* frameData = anim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}

	int j = glGetUniformLocation(shader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false,
		(float*)frameMatrices.data());
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		mesh->DrawSubMesh(i);

	}
} 
void nodeCharacter::Update(float dt) {
	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0f / anim->GetFrameRate();
		break;
	}

	SceneNode::Update(dt);
}