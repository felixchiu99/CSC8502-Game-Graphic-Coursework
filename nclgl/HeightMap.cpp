#include "HeightMap.h"
#include <iostream>

HeightMap::HeightMap(const std::string & name, Vector3 vertexScale ) {
	int iHeight, iChans;
	this->vertexScale = vertexScale;
	unsigned char* data = SOIL_load_image(name.c_str(),
		&iWidth, &iHeight, &iChans, 1);
	if (!data) {
		std::cout << "Heightmap can’t load file!\n";
		return;
	}

	std::cout << iWidth << iHeight << std::endl;

	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);
	
	for (int z = 0; z < iHeight; ++z) {
		for (int x = 0; x < iWidth; ++x) {
			int offset = (z * iWidth) + x;
			//vertices[offset] = Vector3(x, 0, z) * vertexScale; // height map shader test
			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
			
		}
		
	}
	SOIL_free_image_data(data);
	int i = 0;

	for (int z = 0; z < iHeight - 1; ++z) {
		for (int x = 0; x < iWidth - 1; ++x) {
			int a = (z * (iWidth)) + x;
			int b = (z * (iWidth)) + (x + 1);
			int c = ((z + 1) * (iWidth)) + (x + 1);
			int d = ((z + 1) * (iWidth)) + x;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;
			
			indices[i++] = c;
			indices[i++] = a;
			indices[i++] = d;
		}
	}
	GenerateNormals();
	GenerateTangents();
	BufferData();
	
	heightmapSize.x = vertexScale.x * (iWidth - 1);
	heightmapSize.y = vertexScale.y * 255.0f;//each height is a byte!
	heightmapSize.z = vertexScale.z * (iHeight - 1);
}
float HeightMap::GetHeight(Vector3 pos, Vector3 worldOffset) {
	pos = Vector3(pos.x + (GetHeightmapSize().x/2), 0 , pos.x + (GetHeightmapSize().z / 2));
	pos = Vector3(pos.x / vertexScale.x, 0, pos.z / vertexScale.z);
	int offset = ((int)pos.z * iWidth) + (int)pos.x ;
	return vertices[offset].y;

}