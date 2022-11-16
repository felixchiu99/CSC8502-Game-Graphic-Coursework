#version 330 core
/*
layout(std140) uniform ObjectMatrices{
	mat4 mvp[5]; //model, view, proj
};
*/

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;

out Vertex{
	vec3 worldPos;
} OUT;

void main(void) {
	vec3 scale = vec3(lightRadius);
	vec3 worldPos = (position * scale) + lightPos;
	gl_Position = (projMatrix * viewMatrix) * vec4(worldPos, 1.0);
	vec4 worldPos2 = (modelMatrix * vec4(position, 1));
	OUT.worldPos = worldPos2.xyz;
}