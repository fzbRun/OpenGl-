#version 330 core
layout(triangles) in;
layout(triangle_strip,max_vertices = 3) out;

in VS_OUT{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 normal;
	mat4 model;
}gs_in[];

out GS_OUT{
	vec3 FragPos;
    	vec2 TexCoords;
    	vec3 TangentLightPos;
    	vec3 TangentViewPos;
    	vec3 TangentFragPos;
}gs_out;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main(){

	vec3 edge1 = gs_in[1].FragPos - gs_in[0].FragPos;
	vec3 edge2 = gs_in[2].FragPos - gs_in[0].FragPos;
	vec2 delta1 = gs_in[1].TexCoords - gs_in[0].TexCoords;
	vec2 delta2 = gs_in[2].TexCoords - gs_in[0].TexCoords;

	float f = 1.0f / (delta1.x * delta2.y - delta1.y * delta2.x);
	
	vec3 tangent;
       	tangent.x = f * (edge1.x * delta2.y - edge2.x * delta1.y);
        	tangent.y = f * (edge1.y * delta2.y - edge2.y * delta1.y);
        	tangent.z = f * (edge1.z * delta2.y - edge2.z * delta1.y);
        	tangent = normalize(tangent);
	
	/*
	vec3 bitangent;
        	bitangent.x = f * (edge2.x * delta1.x - edge1.x * delta2.x);
        	bitangent.y = f * (edge2.y * delta1.x - edge1.y * delta2.x);
        	bitangent.z = f * (edge2.z * delta1.x - edge1.z * delta2.x);
        	bitangent = normalize(bitangent);
	*/
	
	for(int i = 0;i < 3;i++){
		gs_out.FragPos = vec3(gs_in[i].model * vec4(gs_in[i].FragPos,1.0f));
		gs_out.TexCoords = gs_in[i].TexCoords;
		mat3 normalMatrix = transpose(inverse(mat3(gs_in[i].model)));
		vec3 T = normalize(normalMatrix * tangent);
    		//vec3 B = normalize(normalMatrix * bitangent);
		vec3 N = normalize(normalMatrix * gs_in[i].normal);
		
		T = normalize(T - dot(T,N) * N);
		vec3 B = cross(T,N);

		mat3 TBN = transpose(mat3(T, B, N));  
    		gs_out.TangentLightPos = TBN * lightPos;
    		gs_out.TangentViewPos  = TBN * viewPos;
    		gs_out.TangentFragPos  = TBN * gs_in[i].FragPos;
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
	
}