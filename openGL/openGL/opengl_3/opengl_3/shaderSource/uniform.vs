#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

layout(std140) uniform camera{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

out VS_OUT{
	vec2 TexCoords;
}vs_out;

void main(){
	gl_Position = projection * view * model * vec4(aPos,1.0f);
	vs_out.TexCoords = aTexCoords;
}