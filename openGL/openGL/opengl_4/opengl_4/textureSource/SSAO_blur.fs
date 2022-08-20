#version 330 core

in vec2 texCoords;

out float fragColor;

uniform sampler2D SSAO;
uniform bool ver;

void main() 
{
  	vec2 texelSize = 1.0f / vec2(textureSize(SSAO, 0));
   	float result = 0.0;
	for(int i = -8;i < 8;i++){
		if(ver){
			result += texture(SSAO, vec2(i,0) * texelSize + texCoords).r;
		}else{
			result += texture(SSAO, vec2(0,i) * texelSize + texCoords).r;
		}
	}
 
   fragColor = result / 16.0f;
}