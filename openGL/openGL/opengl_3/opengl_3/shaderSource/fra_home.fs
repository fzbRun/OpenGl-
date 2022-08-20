#version 330 core

out vec4 FragColor;

in vec3 gColor;

void main(){
	FragColor = vec4(gColor,1.0f);
}