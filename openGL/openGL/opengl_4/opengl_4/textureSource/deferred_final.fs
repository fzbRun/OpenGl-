#version 460 core
layout(early_fragment_tests) in;

out vec4 FragColor;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};

in vec2 TexCoords;

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

void main(){
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
   	vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
    float specular = texture(gAlbedoSpec, TexCoords).a;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 ambient = 0.6 * color;
	vec3 result = ambient;
	for(int i = 0;i < NR_LIGHTS;i++){
		vec3 lightDir = normalize(lights[i].Position - FragPos);
		float distance = length(lights[i].Position - FragPos);
		float attenuation = 1.0f / (1.0f + distance * lights[i].Linear + distance * distance * lights[i].Quadratic);
		float diffuse = max(dot(lightDir,normal),0.0f);
		float specular = pow(max(dot(normalize(viewDir + lightDir),normal),0.0f),32.0f) * specular;
		result += (diffuse + specular) * color * attenuation * lights[i].Color;
	}
	FragColor = vec4(result,1.0f);
}


