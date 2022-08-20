#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main(){
	float distance = length(FragPos.xyz - lightPos);
	gl_FragDepth = distance / far_plane;
}