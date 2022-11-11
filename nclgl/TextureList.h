#pragma once
#include <map>
#include <string>
#include "OGLRenderer.h"

class TextureList
{
public:
	TextureList(void) {
	}
	TextureList(std::string name, GLuint key) {
		addTexture(name, key);
	}
	~TextureList(void) {};
	GLuint getTexture(std::string name) {
		return this->textureMap[name];
	}
	void addTexture(std::string name, GLuint key) {
		this->textureMap[name] = key;
	}
	void deleteTexture(std::string name) {
		this->textureMap.erase(name);
	}

private:
	std::map<std::string, GLuint> textureMap;

};

