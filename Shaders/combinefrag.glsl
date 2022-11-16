#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;


void main(void) {
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	float alpha = texture(diffuseTex, IN.texCoord).a;
	vec3 light = texture(diffuseLight, IN.texCoord).xyz;
	vec3 specular = texture(specularLight, IN.texCoord).xyz;
	if(alpha != 1){
		fragColour.xyz = diffuse.xyz;
		fragColour.a = 1.0;
		return;
	}
	fragColour.xyz = diffuse * 0.1; // ambient
	fragColour.xyz += diffuse * light; // lambert
	fragColour.xyz += specular; // Specular
	fragColour.a = 1.0;
}