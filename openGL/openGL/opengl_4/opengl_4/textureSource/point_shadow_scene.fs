#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
}fs_in;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D diffuseTexture;
uniform samplerCube shadowMap;
uniform float far_plane;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float ShadowCalculation(vec3 fragPos){
	vec3 fragToLight = fragPos - lightPos;
	float currentDepth = length(fragToLight);
	float shadow = 0.0;
	int samples = 20;
	float viewDistance = length(viewPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
	for(int i = 0; i < samples; ++i)
	{
    		float closestDepth = texture(shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
    		closestDepth *= far_plane;   // Undo mapping [0;1]
    		if(currentDepth > closestDepth)
        			shadow += 1.0;
	}
	return shadow /= float(samples);
}

void main()
{             
	vec3 color = texture(diffuseTexture,fs_in.TexCoords).rgb;
	vec3 lightColor = vec3(0.3f);
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 normal = normalize(fs_in.Normal);
	vec3 ambient = 0.3 * lightColor;
	float diff = max(dot(lightDir,normal),0.0f);
	vec3 diffuse = diff * lightColor;
	float spec = pow(max(dot(normalize(lightDir + viewDir),normal),0.0f),64.0f); 	
	vec3 specular = spec * lightColor;
	float shadow = ShadowCalculation(fs_in.FragPos);
	vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color;
	FragColor = vec4(lighting,1.0f);
}
