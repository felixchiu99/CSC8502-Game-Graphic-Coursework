#version 330 core

uniform sampler2D diffuseTex;
uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform float lightRadius;
uniform float lightIntensity;
uniform float lightType;
uniform float lightAngle;

in Vertex{
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
	vec3 incident;
	if(lightType == 1){
		incident = normalize(lightDirection);
	}else{
		incident = normalize(lightPos - IN.worldPos);
	}
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	float lambert = max(dot(incident, IN.normal), 0.0f);

	float distance;
	if (lightType == 1)
		distance = 2000;
	else
		distance = length(lightPos - IN.worldPos);

	float attenuation = lightIntensity / (distance * distance);

	float specFactor = clamp(dot(halfDir, IN.normal), 0.0, 1.0);
	specFactor = pow(specFactor, 60.0); 

	vec3 surface = (diffuse.rgb * lightColour.rgb);
	fragColour.rgb = surface * lambert * attenuation;
	fragColour.rgb += (lightColour.rgb * specFactor) * attenuation * 0.33;
	fragColour.rgb += surface * 0.13f; // ambient!
	fragColour.a = diffuse.a;

}