#version 330 core
layout(location = 0) out vec4 gPositionDepth;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedo;

in VS_OUT{
	vec3 position;
	vec3 normal;
	vec2 texCoords;
}fs_in; 

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth){
	depth = depth * 2.0f - 1.0f;
	return 2 * near * far / (near + far - depth * (far - near));
}

void main(){
	//float depth = LinearizeDepth(gl_FragCoord.z);
	float depth = gl_FragCoord.z;
	gPositionDepth = vec4(fs_in.position,depth);
	gNormal = fs_in.normal;
	//gAlbedo.rgb = texture(texture_diffuse1,fs_in.texCoords).rgb;
	gAlbedo.rgb = vec3(0.95f);
}