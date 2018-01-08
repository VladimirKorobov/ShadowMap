#version 330 core
layout(location = 0) out vec4 color;
//layout(location = 1) out float fragmentdepth;
uniform vec3 LightDir;

in vec2 vsTexCoord;
in vec3 vsNormal;


void main(void)
{
	color = vec4(0.5, 0.8, 0.8, 1.0);
	
	float kd = max(-dot(LightDir, vsNormal), 0);


	color = color * kd;
	//fragmentdepth = gl_FragCoord.z;
}