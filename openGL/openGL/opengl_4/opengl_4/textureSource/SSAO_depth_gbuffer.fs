#version 330 core
layout(location = 0) out float gDepth;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedo;

in VS_OUT{
	vec3 position;
	vec3 normal;
	vec2 texCoords;
}fs_in; 

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * 0.1f * 100.0f) / (100.0f + 0.1f - z * (100.0f - 0.1f));	
}

void main(){
	float depth = gl_FragCoord.z;
	//float depth = LinearizeDepth(gl_FragCoord.z);
	gDepth = depth;
	gNormal = fs_in.normal;
	gAlbedo.rgb = vec3(0.95f);
}