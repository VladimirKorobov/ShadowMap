#version 330 core

in vec2 vsTexcoord;
in vec3 vsNormal;

uniform vec3 LightDir;

layout(location = 0) out vec4 color;

void main(void)
{
	color = vec4(231.0f/255, 164.0f/255, 97.0f/255, 1.0);
	float kd = max(-dot(LightDir, vsNormal), 0);
	//float kd = abs(dot(light.dir, vert.normal));
	color = color * kd;
}