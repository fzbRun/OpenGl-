#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
}vs_out;

layout(std140) uniform camera{
	mat4 projection;
	mat4 view;
	mat4 lightSpaceMatrix;
};

uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos,1.0f);
	vs_out.FragPos = vec3(model * vec4(aPos,1.0f));
	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
	vs_out.TexCoords = aTexCoords;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos,1.0f);
}