#version 330 core

out float FragColor;

in vec2 texCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform vec3 samples[64];
uniform mat4 projection;

vec2 noiseSize = vec2(800.0f/4.0f,600.0f/4.0f);

void main(){
	vec3 FragPos = texture(gPositionDepth,texCoords).rgb;
	vec3 normal = texture(gNormal,texCoords).rgb;
	vec3 randomVec = texture(texNoise,texCoords * noiseSize).rgb;
	vec3 tangent = normalize(randomVec - normal * dot(randomVec,normal));
	vec3 bitangent = cross(normal,tangent);
	mat3 TBN = mat3(tangent,bitangent,normal);
	float occlusion = 0.0f;
	for(int i = 0;i < 64;i++){
		vec3 sample = TBN * samples[i];
		sample = FragPos + sample;
		vec4 offset = projection * vec4(sample,1.0f);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float depth = texture(gPositionDepth,offset.xy).w;
		
		float rangeCheck = smoothstep(0.0f, 1.0f, 1.0f / abs(FragPos.z - depth));
		occlusion += (depth < offset.z ? 1.0f : 0.0f);
	}
	occlusion = 1.0f - occlusion / 64;
	FragColor = pow(occlusion,5);
}
