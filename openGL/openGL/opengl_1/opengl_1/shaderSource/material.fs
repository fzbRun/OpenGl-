#version 330 core

struct Material{
	//vec3 ambient;
	//vec3 diffuse;
	sampler2D diffuse;
	//vec3 specular;
	sampler2D specular;
	//sampler2D glow;
	float shininess;
};

struct Light{
	//vec3 position;
	//vec3 worldDirection;

	vec3 flashPos;
	vec3 flashDir;
	float cutoff;
	float outerCutoff;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 color;
	
	float constant;
	float linear;
	float quadratic;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 cameraPos;

uniform Material material;

uniform Light light;

void main(){
	//计算点光源与物体顶点距离
	//float distance = length(light.position - FragPos);
	//计算聚光与物体顶点距离
	float distance = length(light.flashPos - FragPos);
	//计算光在物体顶点处的强度
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	//计算环境光
	vec3 ambient = light.ambient * light.color * texture(material.diffuse,TexCoords).rgb;
	//计算光的方向
	vec3 lightRay = normalize(light.flashPos - FragPos);
	//计算光与聚光的角度cos值；
	float theta = dot(light.flashDir,normalize(-lightRay));
	
	/*
	vec3 result;
	//判断物体顶点是否在聚光范围之外
	if(theta > light.cutoff){
		//计算法向量
		vec3 norm = normalize(Normal);	
		//计算漫反射夹角
		float diff = max(dot(lightRay,norm),0.0);
		//计算漫反射光
		vec3 diffuse = diff * light.color * texture(material.diffuse,TexCoords).rgb * light.diffuse;
		//计算相机方向
		vec3 cameraRay = normalize(cameraPos - FragPos);
		//计算镜面反射夹角
		float spec = pow(max(dot(normalize(cameraRay + lightRay),norm),0.0),material.shininess);
		//计算镜面反射光
		vec3 specular = spec * light.color * texture(material.specular,TexCoords).rgb * light.specular;
	
		result = ambient + (diffuse + specular) * attenuation;
	}else{
		result = ambient;
	}
	*/

	//平滑过渡
	//计算内外圆锥cos差
	float epslion = light.cutoff - light.outerCutoff;
	//计算聚光强度
	float intensity = clamp((theta - light.outerCutoff) / epslion,0.0,1.0);

	
	//计算法向量
	vec3 norm = normalize(Normal);	
	//计算点光源方向
	//vec3 lightRay = normalize(light.position - FragPos);
	//计算全局光方向
	//vec3 lightRay = normalize(-light.worldDirection);
	//计算漫反射夹角
	float diff = max(dot(lightRay,norm),0.0);
	//计算漫反射光
	vec3 diffuse = diff * light.color * texture(material.diffuse,TexCoords).rgb * light.diffuse;
	//计算相机方向
	vec3 cameraRay = normalize(cameraPos - FragPos);
	//计算镜面反射夹角
	float spec = pow(max(dot(normalize(cameraRay + lightRay),norm),0.0),material.shininess);
	//计算镜面反射光
	vec3 specular = spec * light.color * texture(material.specular,TexCoords).rgb * light.specular;
	//计算物体自身发光
	//vec3 glow = texture(material.glow,TexCoords).rgb;
	
	vec3 result = ambient + (diffuse + specular) * attenuation * intensity;
	
	FragColor = vec4(result,1.0f);
}