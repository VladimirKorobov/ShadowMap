#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;


uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat3 NormalMat;
uniform mat4 ProjectionMat;
uniform vec4 WaterPlane;
uniform vec3 WaveDirection;
uniform vec3 WaveOffset;
uniform float Ph;

out vec3 vsNormal;
out vec4 vsPosition;
out float vsAlpha;

void main()
{
	float Refraction = 0.3;
	vec4 vertex   = ModelMat * vec4(position, 1.0);
	vsNormal = NormalMat * normal;

	gl_Position = ViewMat * vertex;
	vsPosition = ProjectionMat * gl_Position;
	vsPosition = vsPosition / (vsPosition.w * 2) + 0.5;

	vec3 pos3 = vec3(gl_Position);


	float D = WaterPlane.w + 1 * sin(dot(WaveDirection, pos3 - WaveOffset) * 0.1 + Ph);
	vec3 n = vec3(WaterPlane);
	
	
	float d = dot(pos3, n);
	vsAlpha = 0;
	
	if(d < 1.0e-6)
	{
		float t = D / d;
		if (t >= 0 && t <= 1.0)		
		{
			vec3 crossWater = pos3 * t; 
			vec3 underWater = pos3 - crossWater;
			vec3 alongWater = underWater - dot(underWater, n) * n;
			vec3 refracted = crossWater + alongWater * Refraction;

			vsAlpha = length(vec3(gl_Position) - refracted) / 20;
			if(vsAlpha > 0.999) vsAlpha = 0.999;

			gl_Position = vec4(refracted , 1);
			vsNormal = NormalMat * vec3(0, 1, 0);
		}
	}
	
	gl_Position = ProjectionMat * gl_Position;
	
}

