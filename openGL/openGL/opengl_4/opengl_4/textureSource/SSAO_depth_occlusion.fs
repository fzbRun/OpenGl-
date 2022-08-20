#version 330 core

out float FragColor;

in vec2 texCoords;

uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform vec3 samples[64];
uniform mat4 projection;

vec2 noiseSize = vec2(800.0f/4.0f,600.0f/4.0f);

void main(){
	float FragDepth = texture(gDepth,texCoords).r;
	vec3 normal = texture(gNormal,texCoords).rgb;
	vec3 randomVec = texture(texNoise,texCoords * noiseSize).rgb;
	vec3 tangent = normalize(randomVec - normal * dot(randomVec,normal));
	vec3 bitangent = cross(normal,tangent);
	mat3 TBN = mat3(tangent,bitangent,normal);
	
	mat4 unProjection = inverse(projection);
	
	vec4 clipSpaceLocation;
	/*
    	clipSpaceLocation.xy = (texCoords*2.0-1.0) * -FragDepth;
	//clipSpaceLocation.z  = FragDepth * 2.0-1.0; 
	//clipSpaceLocation.z = (projection * vec4(0.0f,0.0f,FragDepth,1.0f)).z;
	clipSpaceLocation.w  = -FragDepth; 
	vec4 homogenousLocation = unProjection * clipSpaceLocation;
	//vec3 FragPos = homogenousLocation.xyz/homogenousLocation.w;
	vec3 FragPos = vec3((unProjection * clipSpaceLocation).xy,FragDepth);
	*/

	clipSpaceLocation.xy = texCoords*2.0-1.0;
    	clipSpaceLocation.z  = FragDepth * 2.0-1.0; 
    	//clipSpaceLocation.z = (projection * vec4(0.0f,0.0f,FragDepth,1.0f)).z / -FragDepth;
    	clipSpaceLocation.w  = 1.0; 
    	vec4 homogenousLocation = unProjection * clipSpaceLocation;
    	vec3 FragPos = homogenousLocation.xyz/homogenousLocation.w;
	
	float occlusion = 0.0f;
	for(int i = 0;i < 64;i++){
		vec3 sample = TBN * samples[i];
		sample = FragPos + sample;
		vec4 offset = projection * vec4(sample,1.0f);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float depth = texture(gDepth,offset.xy).r;
		
		float rangeCheck = smoothstep(0.0f, 1.0f, 1.0f / abs(offset.z - depth));
		occlusion += (depth < offset.z ? 1.0f : 0.0f) * rangeCheck;
	}
	occlusion = 1.0f - occlusion / 64;
	FragColor = pow(occlusion,5);
}
