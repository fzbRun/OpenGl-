#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 WorldPos;

void main()
{
    	WorldPos = mat3(model) * aPos;

	mat4 rotView = mat4(mat3(view));
	//mat4 rotView = view;
	vec4 clipPos = projection * rotView * model * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}