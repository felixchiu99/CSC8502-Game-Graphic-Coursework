#pragma once
#include <string >
#include "mesh.h"

class HeightMap : public Mesh {
public:
	HeightMap(const std::string & name, Vector3 vertexScale = Vector3(16.0f, 1.0f, 16.0f));
	~HeightMap(void) {};
	
	Vector3 GetHeightmapSize() const { return heightmapSize; }
protected:
	Vector3 heightmapSize;

};