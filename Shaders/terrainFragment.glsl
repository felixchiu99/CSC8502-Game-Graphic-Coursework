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
	vec4 sloped = texture2D(diffuseTex0, IN.texCoord);
	vec4 flatTex = texture2D(diffuseTex3, IN.texCoord);
	vec3 normal = texture2D(bumpTex3, IN.texCoord).rgb * 2.0 - 1.0;
	float blendRange = 3;
	float flatBlend = 0;
	if (IN.worldPos.y < 30) {
		flatTex = texture2D(diffuseTex1, IN.texCoord);
		normal = texture2D(bumpTex1, IN.texCoord).rgb * 2.0 - 1.0;
	}
	else if (IN.worldPos.y < 45) {
		flatBlend = (IN.worldPos.y - 30) / blendRange;
		flatBlend = flatBlend > 1 ? 1 : flatBlend;
		normal = mix(texture2D(bumpTex1, IN.texCoord).rgb * 2.0 - 1.0 , texture2D(bumpTex2, IN.texCoord).rgb * 2.0 - 1.0 , flatBlend);
		flatTex = mix(texture2D(diffuseTex1, IN.texCoord),texture2D(diffuseTex2, IN.texCoord), flatBlend);
	}else {
		flatBlend = (IN.worldPos.y - 45) / blendRange;
		flatBlend = flatBlend > 1 ? 1 : flatBlend;
		normal = mix(texture2D(bumpTex2, IN.texCoord).rgb * 2.0 - 1.0, texture2D(bumpTex3, IN.texCoord).rgb * 2.0 - 1.0, flatBlend);
		flatTex = mix(texture2D(diffuseTex2, IN.texCoord), texture2D(diffuseTex3, IN.texCoord), flatBlend);
	}
	
	fragColour[0] = flatTex;
	if (IN.normal.y < 0.74) {
		fragColour[0] = sloped;
	}
	normal = normalize(TBN * normalize(normal));
	fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}