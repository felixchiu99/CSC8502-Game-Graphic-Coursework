#pragma once
#include "OGLRenderer.h"
class TextureWBump
{
public:
	TextureWBump(GLuint texture = NULL, GLuint bumpMap = NULL) {
		this->texture = texture;
		this->bumpMap = bumpMap;
	};
	GLuint getTexture() {
		return this->texture;
	}
	GLuint getBumpMap() {
		return this->bumpMap;
	}
protected:
	GLuint texture;
	GLuint bumpMap;
};

