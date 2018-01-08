#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 ShadowMat;


void main()
{
	gl_Position = ShadowMat * vec4(position, 1.0);
}

