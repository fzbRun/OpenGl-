#version 330 core
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;
  
uniform float lightStrength;
uniform float specularStrength;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
    	vec3 ambient = lightStrength * lightColor;
  	
    	// diffuse 
	vec3 norm = normalize(Normal);
   	vec3 lightDir = normalize(lightPos - FragPos);
  	float diff = max(dot(norm, lightDir), 0.0);
   	vec3 diffuse = diff * lightColor;

	vec3 cameraDir = normalize(cameraPos - FragPos);
	//vec3 cameraDir = normalize(-FragPos);
	float spec = pow(max(dot(normalize(lightDir + cameraDir),norm),0.0),32);
	vec3 specular = spec * specularStrength * lightColor;
            
    	vec3 result = (ambient + diffuse + specular) * objectColor;
    	FragColor = vec4(result, 1.0);
}