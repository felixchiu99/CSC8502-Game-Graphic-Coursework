#pragma once
#include <map>
#include <string>
#include "OGLRenderer.h"
#include "TextureWBump.h"

class TextureList
{
public:
	TextureList(void) {
	}
	TextureList(std::string name, GLuint texture, GLuint bumpMap) {
		addTexture(name, texture, bumpMap);
	}
	~TextureList(void) {};
	GLuint getTexture(std::string name) {
		return this->textureMap[name].getTexture();
	}
	GLuint getBumpMap(std::string name) {
		return this->textureMap[name].getBumpMap();
	}
	TextureWBump* getTexturePair(std::string name) {
		return &this->textureMap[name];
	}
	void addTexture(std::string name, GLuint texture, GLuint bumpMap) {
		this->textureMap[name] = TextureWBump(texture, bumpMap);
	}
	void deleteTexture(std::string name) {
		this->textureMap.erase(name);
	}

private:
	std::map<std::string, TextureWBump> textureMap;

};

