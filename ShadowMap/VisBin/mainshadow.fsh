#version 330 core
layout(location = 0) out vec4 color;
//layout(location = 1) out float fragmentdepth;
uniform vec3 LightDir;

in vec2 vsTexCoord;
in vec3 vsNormal;
in vec4 vsShadePos;

//uniform sampler2DShadow depthTexture;
uniform sampler2D depthTexture;


vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);


float LinearizeDepth(in vec2 uv)
{
    float zNear = 0.1;    // TODO: Replace by the zNear of your perspective projection
    float zFar  = 1000.0; // TODO: Replace by the zFar  of your perspective projection
    float depth = texture2D(depthTexture, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}



void main(void)
{
	color = vec4(0.5, 0.8, 0.8, 1.0);
	float kd = max(-dot(LightDir, vsNormal), 0);

	float x =  vsShadePos.x / (vsShadePos.w * 2) + 0.5;
	float y =  vsShadePos.y / (vsShadePos.w * 2) + 0.5;
	float z =  vsShadePos.z / (vsShadePos.w * 2) + 0.5;

	float bias = 0.0000005;
	
	float visibility = texture2D(depthTexture, vec2(x,y)).x;
	visibility = visibility  < z - bias ? 0.2 : 1;
	color = color * kd * visibility;
}