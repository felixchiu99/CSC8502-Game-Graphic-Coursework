#version 330 core

uniform sampler2D diffuseTex0; // Diffuse texture map
uniform sampler2D bumpTex0; //Bump map
uniform sampler2D diffuseTex1; // Diffuse texture map
uniform sampler2D bumpTex1; //Bump map
uniform sampler2D diffuseTex2; // Diffuse texture map
uniform sampler2D bumpTex2; //Bump map
uniform sampler2D diffuseTex3; // Diffuse texture map
uniform sampler2D bumpTex3; //Bump map

//sampler2D sample2DTable[];

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour[2]; //Our final outputted colours!

void main(void) {
	mat3 TBN = mat3(normalize(IN.tangent),
		normalize(IN.binormal),
		normalize(IN.normal));

	vec3 normal = texture2D(bumpTex1, IN.texCoord).rgb * 2.0 - 1.0;
	//vec3 normal = vec3(1, 1, 1);

	//vec3 normal = mix(texture2D(bumpTex0, IN.texCoord).rgb,texture2D(bumpTex1, IN.texCoord).rgb,0.5) * 2.0 - 1.0;
	normal = normalize(TBN * normalize(normal));

	//fragColour[0] = mix(texture2D(diffuseTex0, IN.texCoord), texture2D(diffuseTex1, IN.texCoord),0.5);
	//fragColour[0] = vec4(normal.x, normal.y, normal.z, 1);
	//fragColour[0] = vec4(1- IN.normal.y, IN.normal.y, 0, 1);
	fragColour[0] = texture2D(diffuseTex1, IN.texCoord);
	fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}