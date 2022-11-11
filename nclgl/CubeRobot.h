#pragma once
#include "..\nclgl\SceneNode.h"

class CubeRobot : public SceneNode {
public:
	CubeRobot(Mesh * cube);
	~CubeRobot(void) {};
	void Update(float dt) override;
	void SetTransform(Matrix4 transformMatrix);
protected:
	SceneNode * head;
	SceneNode * leftArm;
	SceneNode * rightArm;
};

