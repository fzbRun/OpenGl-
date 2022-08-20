#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi,vec3 N,float roughness){
	float a = roughness * roughness;
	float phi = 2.0f * PI * Xi.x;
	float cosTheta = sqrt((1-Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
	
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
		
	vec3 up = abs(N.z) < 0.999 ? vec3(0.0f,0.0f,1.0f) : vec3(1.0f,0.0f,0.0f);
	vec3 tangent = normalize(cross(N,up));
	vec3 bitangent = normalize(cross(N,tangent));

	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}

void main(){
	vec3 N = normalize(WorldPos);
	vec3 R = N;
	vec3 V = R;

	const uint SAMPLE_COUNT = 1024u;
	vec3 prefilteredColor = vec3(0.0f);
	float totalWeight = 0.0f;

	for(uint i = 0u;i < SAMPLE_COUNT;i++){
		vec2 Xi = Hammersley(i,SAMPLE_COUNT);
		vec3 H = ImportanceSampleGGX(Xi,N,roughness);
		vec3 L = normalize(2.0 * dot(V,H) * H - V);
		
		float NdotL = max(dot(N,L),0.0f);
		if(NdotL > 0.0f){
			float D = DistributionGGX(N,H,roughness);
			float NdotH = max(dot(N,H),0.0f);
			float HdotV = max(dot(H,V),0.0);
			float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;
			
			float resolution = 512.0f;
			float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
			float saSample = 1.0f / (float(SAMPLE_COUNT) * pdf + 0.0001);
			
			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
			
			prefilteredColor += texture(environmentMap,L,mipLevel).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	prefilteredColor = prefilteredColor / totalWeight;
	FragColor = vec4(prefilteredColor,1.0);
}














