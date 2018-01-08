#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;


uniform mat4 MVPMat;
uniform mat4 BiasShadowMat;

out vec4 vsShadowCoord;
out vec2 vsTexCoord;


void main()
{
	vsTexCoord = texcoord;
	vec3 pos =position;
	pos.y = 0;

	gl_Position = MVPMat * vec4(pos, 1.0);
	vsShadowCoord = BiasShadowMat * vec4(vec3(position), 1.0);
}

