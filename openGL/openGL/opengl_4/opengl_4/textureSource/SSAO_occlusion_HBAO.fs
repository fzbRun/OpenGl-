#version 330 core

out float FragColor;

in vec2 texCoords;

uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform vec3 samples_lu[16];
uniform vec3 samples_lr[16];
uniform vec3 samples_ru[16];
uniform vec3 samples_rb[16];
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
	
	float occlusion = 0.0f;
	vec3 maxHeight;
	maxHeight = vec3(0.0f);
	for(int i = 0;i < 16;i++){
		vec3 sample = TBN * samples_lu[i];
		sample = FragPos + sample;
		vec4 offset = projection * vec4(sample,1.0f);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;
		sample.z = texture(gDepth,offset.xy).r;

		vec3 Tsample = transport(TBN) * sample;//切线空间
		
		if(Tsample.z > maxHeight.z){
			maxHeight = Tsample;
		}	
	}
	float sinH = normalize(maxHeight).z;
	float sinT = sqrt(normal.x*normal.x + normal.y*normal.y);
	occlusion += sinH / sinT;

	maxHeight = vec3(0.0f);
	for(int i = 0;i < 16;i++){
		vec3 sample = TBN * sampleslb[i];
		sample = FragPos + sample;
		vec4 offset = projection * vec4(sample,1.0f);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;
		sample.z = texture(gDepth,offset.xy).r;

		vec3 Tsample = transport(TBN) * sample;//切线空间
		
		if(Tsample.z > maxHeight.z){
			maxHeight = Tsample;
		}	
	}
	sinH = normalize(maxHeight).z;
	sinT = sqrt(normal.x*normal.x + normal.y*normal.y);
	occlusion += sinH / sinT;

	maxHeight = vec3(0.0f);
	for(int i = 0;i < 16;i++){
		vec3 sample = TBN * samples_ru[i];
		sample = FragPos + sample;
		vec4 offset = projection * vec4(sample,1.0f);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;
		sample.z = texture(gDepth,offset.xy).r;

		vec3 Tsample = transport(TBN) * sample;//切线空间
		
		if(Tsample.z > maxHeight.z){
			maxHeight = Tsample;
		}	
	}
	sinH = normalize(maxHeight).z;
	sinT = sqrt(normal.x*normal.x + normal.y*normal.y);
	occlusion += sinH / sinT;

	maxHeight = vec3(0.0f);
	for(int i = 0;i < 16;i++){
		vec3 sample = TBN * samples_rb[i];
		sample = FragPos + sample;
		vec4 offset = projection * vec4(sample,1.0f);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;
		sample.z = texture(gDepth,offset.xy).r;

		vec3 Tsample = transport(TBN) * sample;//切线空间
		
		if(Tsample.z > maxHeight.z){
			maxHeight = Tsample;
		}	
	}
	sinH = normalize(maxHeight).z;
	sinT = sqrt(normal.x*normal.x + normal.y*normal.y);
	occlusion += sinH / sinT;

	occlusion = 1.0f - occlusion / 4;
	FragColor = pow(occlusion,5);
}