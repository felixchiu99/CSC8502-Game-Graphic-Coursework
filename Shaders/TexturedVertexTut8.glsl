#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
layout(std140) uniform TestBlock{
	vec3 someVec[2];
	float someFlt;
};

in vec3 position;
in vec2 texCoord;
in vec4 colour;

out Vertex{
	vec2 texCoord;
	vec4 colour;
	float colourBlend;
} OUT;
void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	vec3 modPosition = vec3(position.x, 1, position.z);
	gl_Position = mvp * vec4(modPosition, 1.0);
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.colourBlend = modPosition.y / 175;
	OUT.colour = colour;
	if (modPosition.y > 170) {
		OUT.colourBlend = (1-(modPosition.y - 170)/20);
		OUT.colour = vec4(1, 1, 1, 1);
	}
}