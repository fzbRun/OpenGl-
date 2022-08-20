#version 460 core
layout(early_fragment_tests) in;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture;

void main(){
	vec3 color = texture(texture,TexCoords).rgb * 0.6;
	FragColor = vec4(color,1.0f);
}