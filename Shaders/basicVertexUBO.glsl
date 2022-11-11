#version 330 core

layout(std140, binding = 2) uniform TestBlock{
	vec3 someVec[2];
	float someFlt;
};
in vec3 position;
in vec4 colour;

out Vertex{
	vec4 colour;
} OUT;

void main(void) {
	gl_Position = vec4(position, 1.0);
	OUT.colour = vec4(colours[0], colour.g, colour.b, colour.a);
}