#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;
uniform float exposure;

void main(){
	float gamma = 2.2f;

	vec3 result = texture(sceneTexture,TexCoords).rgb;
	vec3 bloom = texture(bloomTexture,TexCoords).rgb;
	result += bloom;
	
	result = vec3(1.0f) - exp(-result * exposure);

	result = pow(result,vec3(1.0 / gamma));

	FragColor = vec4(result,1.0f);
}