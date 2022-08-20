#version 420 core // 注意GLSL的版本！
out vec4 FragColor;
layout (depth_less) out float gl_FragDepth;

void main()
{             
    FragColor = vec4(1.0);
    gl_FragDepth = gl_FragCoord.z - 0.1;
}  