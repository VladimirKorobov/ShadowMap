#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 MVPMat;
uniform mat4 ShadowMat;
out vec4 ShadowCoord;

void main()
{
	gl_Position = MVPMat * vec4(position, 1.0);
	ShadowCoord = ShadowMat * vec4(position, 1.0);
}

