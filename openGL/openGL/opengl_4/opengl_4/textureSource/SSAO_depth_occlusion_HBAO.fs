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
	clipSpaceLocation.xy = texCoords*2.0-1.0;
    	clipSpaceLocation.z  = FragDepth * 2.0-1.0; 
    	clipSpaceLocation.w  = 1.0; 
    	vec4 homogenousLocation = unProjection * clipSpaceLocation;
    	vec3 FragPos = homogenousLocation.xyz/homogenousLocation.w;
	
	normal = normalize(normal);
	float occlusion = 0.0f;
	float sinH;
	vec3 faceNormal = normalize(cross(dFdx(FragPos),dFdy(FragPos)));
	float sinT = sqrt(faceNormal.x * faceNormal.x + faceNormal.y * faceNormal.y);

	for(int f = 0;f < 4;f++){
		float currentAo;
		float maxAo;
		
		for(int i = 0;i < 64;i+=4){
			vec3 sample = TBN * samples[i + f];
			sample = FragPos + sample;
			vec4 offset = projection * vec4(sample,1.0f);
			offset.xyz /= offset.w;
			offset.xyz = offset.xyz * 0.5f + 0.5f;
			float depth = texture(gDepth,offset.xy).r;

			vec4 viewPoint = vec4(offset.xy,depth,1.0f);
			viewPoint * 2.0f -1.0f;
			homogenousLocation = unProjection * viewPoint;
			sample = homogenousLocation.xyz/homogenousLocation.w;
			vec3 h = sample - FragPos;
			float d2 = dot(h,h);

			h = normalize(h);
			sinH = h.z;
			currentAo = sinH - sinT;

			if(currentAo > maxAo){
				float rangeCheck = smoothstep(0.0f, 1.0f, 1.0f / d2);
				occlusion += currentAo * rangeCheck;
				maxAo = currentAo;
			}

		}

	}

	occlusion = 1.0f - occlusion / 4;
	FragColor = occlusion;
}