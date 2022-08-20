#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D lightColorTexture;
uniform sampler2D unlightColorTexture;

void main(){
	vec3 lightColor = texture(lightColorTexture,TexCoords).rgb;
	vec3 unlightColor = texture(unlightColorTexture,TexCoords).rgb;
	FragColor = vec4(lightColor + unlightColor,1.0f);
}