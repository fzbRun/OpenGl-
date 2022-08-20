#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 normal;
    mat4 model;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
	gl_Position = projection * view * model * vec4(position,1.0f);
	vs_out.FragPos = position;
	vs_out.TexCoords = texCoords;
	vs_out.normal = normal;
	vs_out.model = model;
}