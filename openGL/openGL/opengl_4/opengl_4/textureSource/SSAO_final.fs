#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec2 Position;

uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};
uniform Light light;
uniform mat4 projection;

void main()
{             
    float FragDepth = texture(gDepth, TexCoords).r;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;

    mat4 unProjection = inverse(projection);
    vec4 clipSpaceLocation;
    clipSpaceLocation.xy = TexCoords*2.0-1.0;
    clipSpaceLocation.z  = FragDepth * 2.0-1.0; 
    //clipSpaceLocation.z = (projection * vec4(0.0f,0.0f,FragDepth,1.0f)).z / -FragDepth;
    clipSpaceLocation.w  = 1.0; 
    vec4 homogenousLocation = unProjection * clipSpaceLocation;
    vec3 FragPos = homogenousLocation.xyz/homogenousLocation.w;

    vec3 ambient = 0.3f * Diffuse * AmbientOcclusion; // <-- this is where we use ambient occlusion
    vec3 lighting  = ambient; 
    vec3 viewDir  = normalize(-FragPos); // Viewpos is (0.0.0)
    // Diffuse
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0f);
    vec3 specular = light.Color * spec;
    // Attenuation
    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;

    FragColor = vec4(lighting, 1.0f);
}