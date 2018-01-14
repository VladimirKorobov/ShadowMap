#version 330 core

uniform vec3 LightDir;
uniform sampler2D depthTexture;

in vec3 vsNormal;
in float vsAlpha;
in vec4 vsPosition;

layout(location = 0) out vec4 color;

void main(void)
{
	if(vsAlpha < 0.001 || vsAlpha > 1)
	{
		discard;
	}

	float amax = -100;
	for(int i = -1; i <= 1; i ++)
	{
		for(int j = -1; j <= 1; j ++)
		{
			float a = textureOffset(depthTexture, vsPosition.xy, ivec2(i, j)).x;
			if(a > amax)
			{
				amax = a;
			}
		}
	}

	if(amax < vsPosition.z - 0.001)
	{
		discard;
	}

	color = vec4(0, 0, 1, vsAlpha);

	if(vsAlpha < 0.1)
	{
		vec4 white = vec4(1, 1 , 1, vsAlpha * 20);
		float t = vsAlpha * 10;
		color = color * t + white * (1 - t);
	}

	float d = max(-dot(vsNormal, LightDir), 0.0);
	color = color * d;
}