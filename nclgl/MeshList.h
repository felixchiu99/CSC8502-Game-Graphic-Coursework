#pragma once
#include <map>
#include <string>
#include "OGLRenderer.h"
class MeshList
{
public:
	MeshList(void) {
	}
	MeshList(std::string name, Mesh* value) {
		addMesh(name, value);
	}
	~MeshList(void) {
		for (auto const& x : meshMap)
		{
			if (x.second != NULL)
				delete x.second;
		}
	};
	Mesh* getMesh(std::string name) {
		return this->meshMap[name];
	}
	void addMesh(std::string name, Mesh* value) {
		this->meshMap[name] = value;
	}
	void deleteMesh(std::string name) {
		delete meshMap[name];
		this->meshMap.erase(name);
	}

private:
	std::map<std::string, Mesh*> meshMap;


};

