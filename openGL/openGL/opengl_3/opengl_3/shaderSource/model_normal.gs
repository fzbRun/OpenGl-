#version 330 core
layout(triangles) in;
layout(line_strip,max_vertices = 6) out;

in VS_OUT{
	vec3 normal;
}gs_in[];

layout(std140) uniform camera{
	mat4 projection;
	mat4 view;
};

void generateLine(int index){
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal * 0.4f,0.0f);
	EmitVertex();
	EndPrimitive();
}

void main(){
	generateLine(0);
	generateLine(1);
	generateLine(2);
}