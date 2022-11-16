#version 330 core

uniform sampler2D depthTex;
uniform sampler2D normTex;

uniform vec2 pixelSize; // reciprocal of resolution
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform mat4 inverseProjView;
uniform vec3 lightDirection;

uniform float lightIntensity;

out vec4 diffuseOutput;
out vec4 specularOutput;

in Vertex{
	vec3 worldPos;
} IN;


void main(void) {
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
	float depth = texture(depthTex, texCoord.xy).r;

	float atten = lightIntensity;
	
	vec3 normal = normalize(texture(normTex, texCoord.xy).xyz * 2.0 - 1.0);
	vec3 incident;

	incident = normalize(lightDirection);

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float lambert = clamp(dot(incident, normal), 0.0, 1.0);
	float rFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
	float specFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
	specFactor = pow(specFactor, 60.0);
	vec3 attenuated = lightColour.xyz * atten;
	diffuseOutput = vec4(attenuated * lambert, 1.0);
	specularOutput = vec4(attenuated * specFactor * 0.33, 1.0);
}