#version 330 core

uniform samplerCube cubeTex;
uniform float intensity;
in Vertex{
	vec3 viewDir;
} IN;

out vec4 fragColour;

void main(void) {
	fragColour = texture(cubeTex, normalize(IN.viewDir));
	fragColour.rgb *= intensity;
	fragColour.a = 0.9f;
}