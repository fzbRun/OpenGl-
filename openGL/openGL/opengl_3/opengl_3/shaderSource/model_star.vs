#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aModel;

out VS_OUT{
	vec2 texCoords;
}vs_out;

layout(std140) uniform camera{
	mat4 projection;
	mat4 view;
};


void main()
{
    	vs_out.texCoords = aTexCoords;    
    	gl_Position = projection * view * aModel * vec4(aPos, 1.0);
}