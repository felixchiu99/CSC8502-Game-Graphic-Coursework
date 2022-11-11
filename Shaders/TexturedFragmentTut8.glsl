#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex2;

in Vertex{
	vec2 texCoord;
	vec4 colour;
	float colourBlend;
} IN;

out vec4 fragColour;
void main(void) {
	float blend = IN.colourBlend;
	vec4 value = texture(diffuseTex, IN.texCoord)* blend + texture(diffuseTex2, IN.texCoord) * ((1-blend)*0.3) + IN.colour * ((1 - blend) * 0.7);
	if (value.a == 0.0) {
		discard;
	}
	fragColour = value;
}