#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT{
	vec3 position;
	vec3 normal;
	vec2 texCoords;
}vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
	vs_out.position = vec3(model * vec4(aPos,1.0f));
	vs_out.normal = vec3(transpose(inverse(model)) * vec4(aNormal,1.0f));
	vs_out.texCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos,1.0f);
}