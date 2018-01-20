#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;


uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;
uniform vec4 WaterPlane;
uniform vec3 WaveDirection;
uniform vec3 WaveOffset;
uniform float Ph;


bool TranslateToWater(inout vec3 refracted)
{
	float Refraction = 0.5;
	vec3 pos3 = vec3(gl_Position);

	vec3 n = vec3(WaterPlane);
	float d = dot(pos3, n);

	if(d < 1.0e-6)
	{
		float D = WaterPlane.w + 1 * sin(dot(WaveDirection, pos3 - WaveOffset) * 0.1 + Ph);
		float t = D / d;
		if (t >= 0 && t <= 1.0)		
		{
			vec3 crossWater = pos3 * t; 
			vec3 underWater = pos3 - crossWater;
			vec3 alongWater = underWater - dot(underWater, n) * n;
			refracted = crossWater + alongWater * Refraction;

			return true;
		}
	}
	return false;
}

void main()
{
	vec4 vertex   = ModelMat * vec4(position, 1.0);
	gl_Position = ViewMat * vertex;

	vec3 refracted;
	if(TranslateToWater(refracted))
	{
		gl_Position = vec4(refracted , 1);
	}
	gl_Position = ProjectionMat * gl_Position;
}

