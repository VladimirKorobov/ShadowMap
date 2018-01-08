#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 ModelMat;
uniform mat3 NormalMat;
uniform mat4 VPMat;

out vec2 vsTexcoord;
out vec3 vsNormal;

void main()
{
	vec4 vertex   = ModelMat * vec4(position, 1.0);
	vsNormal = normalize(NormalMat * normal);

	vsTexcoord = texcoord;
	gl_Position = VPMat * vertex;
}

