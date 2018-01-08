#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 ModelMat;
uniform mat4 VPMat;
uniform mat4 ShadowMat;
uniform mat3 NormalMat;

out vec2 vsTexCoord;
out vec3 vsNormal;
out vec4 vsShadePos;

void main()
{
	vec4 vertex   = ModelMat * vec4(position, 1.0);
	vsNormal = normalize(NormalMat * normal);

	vsTexCoord = texcoord;
	gl_Position = VPMat * vertex;
	vsShadePos = ShadowMat * vec4(vec3(position), 1.0);
}

