#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
}fs_in;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 FragPosLightSpace){
	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;
	if(projCoords.z > 1.0f){
		return 0.0f;	
	}
	float currentdepth = projCoords.z;
	float shadow = 0.0f;
	vec2 texelSize = 1.0 / textureSize(shadowMap,0);
	for(int x = -1;x < 2;x++){
		for(int y = -1;y < 2;y++){
			float pcfDepth = texture(shadowMap,projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentdepth > pcfDepth ? 1.0 : 0.0f;
		}
	}
	return shadow/9;
}

void main()
{             
	vec3 color = texture(diffuseTexture,fs_in.TexCoords).rgb;
	vec3 lightColor = vec3(0.3f);
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 normal = normalize(fs_in.Normal);
	vec3 ambient = 0.15 * lightColor;
	float diff = max(dot(lightPos,normal),0.0f);
	vec3 diffuse = diff * lightColor;
	float spec = pow(max(dot(normalize(lightDir + viewDir),normal),0.0f),64.0f); 	
	vec3 specular = spec * lightColor;
	float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
	vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color;
	FragColor = vec4(lighting,1.0f);
}