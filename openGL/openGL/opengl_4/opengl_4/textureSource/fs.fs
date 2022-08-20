#version 330 core

uniform sampler2D diffuseMap;
uniform vec3 lightPos[4];
uniform vec3 lightColor[4];
uniform vec3 viewPos;

in VS_OUT{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 normal;
}fs_in;

out vec4 FragColor;

void main(){
	vec3 color = texture(diffuseMap,fs_in.TexCoords).rgb;
	vec3 normal = normalize(fs_in.normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 ambient = 0.3 * color;
	vec3 result;
	for(int i = 0;i < 4;i++){
		vec3 lightDir = normalize(lightPos[i] - fs_in.FragPos);
	
		vec3 diffuse = max(dot(lightDir,normal),0.0f) * color * lightColor[i];
		vec3 specular = pow(max(dot(normalize(lightDir + viewDir),normal),0.0f),32.0f) * color * lightColor[i];
	
		result += ambient + diffuse + specular;
	}
	result = vec3(1.0f) - exp(-result * 0.05f);
	FragColor = vec4(result,1.0f);
}