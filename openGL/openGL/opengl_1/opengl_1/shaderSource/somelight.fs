#version 330 core

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;	
};

struct WorldLight{
	vec3 direction;
	vec3 color;
	vec3 strength;
};

struct PointLight{
	vec3 position;
	vec3 color;
	float constant;
	float linear;
	float quadratic;
};

struct FlashLight{
	vec3 direction;
	vec3 color;
	float constant;
	float linear;
	float quadratic;
	float cutoff;
	float outerCutoff;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform Material material;
uniform WorldLight worldLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform FlashLight flashLight;
uniform float ambientStrength;
uniform vec3 cameraPos;

vec3 CalcWorldLight(WorldLight light,vec3 normal,vec3 view,vec3 dmap,vec3 smap){
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(lightDir,normal),0.0);
	vec3 diffuse = diff *  dmap * light.color * light.strength;
	float spec = pow(max(dot(normalize(view + lightDir),normal),0.0),material.shininess);
	vec3 specular = spec * light.strength * smap * light.color;
	return diffuse + specular;
}

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 view,vec3 dmap,vec3 smap){
	
	float distance = length(light.position - FragPos);
	float strength = 1.0f / (light.constant + distance * light.linear + distance * distance * light.quadratic);

	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(lightDir,normal),0.0);
	vec3 diffuse = diff * dmap * light.color * strength;
	float spec = pow(max(dot(normalize(lightDir + view),normal),0.0),material.shininess);
	vec3 specular = spec * smap * strength * light.color;
	return diffuse + specular;
}

vec3 CalcFlashLight(FlashLight light,vec3 normal,vec3 dmap,vec3 smap){
	float distance = length(cameraPos - FragPos);
	float strength = 1.0f / (light.constant + distance * light.linear + distance * distance * light.quadratic);
	float theta = dot(light.direction,normalize(FragPos - cameraPos));
	strength *= clamp((theta - light.outerCutoff) / (light.cutoff - light.outerCutoff),0.0,1.0);	

	vec3 lightDir = normalize(cameraPos - FragPos);
	float diff = max(dot(lightDir,normal),0.0);
	vec3 diffuse = diff * dmap * light.color * strength;
	float spec = pow(max(dot(lightDir,normal),0.0),material.shininess);
	vec3 specular = spec * smap * strength * light.color;
	return diffuse + specular;
}

void main(){
	vec3 dmap = texture(material.diffuse,TexCoords).rgb;
	vec3 smap = texture(material.specular,TexCoords).rgb;
	vec3 view = normalize(cameraPos - FragPos);
	vec3 normal = normalize(Normal);
	
	vec3 ambient = ambientStrength * vec3(1.0f) * dmap;
	vec3 result = ambient + CalcWorldLight(worldLight,normal,view,dmap,smap);
	for(int i = 0;i < NR_POINT_LIGHTS;i++){
		result += CalcPointLight(pointLights[i],normal,view,dmap,smap);
	}
	result += CalcFlashLight(flashLight,normal,dmap,smap);
	FragColor = vec4(result,1.0f);
}