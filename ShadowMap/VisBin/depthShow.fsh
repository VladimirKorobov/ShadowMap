#version 330 core
layout(location = 0) out vec4 color;

uniform sampler2D depthTexture;
//uniform sampler2DShadow depthTexture;

in vec4 vsShadowCoord;
in vec2 vsTexCoord;


float LinearizeDepth(in vec2 uv)
{
    float zNear = 0.1;    // TODO: Replace by the zNear of your perspective projection
    float zFar  = 1000.0; // TODO: Replace by the zFar  of your perspective projection
    float depth = texture2D(depthTexture, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

/*
void main()
{
    float c = LinearizeDepth(vsTexCoord);
    color = vec4(c, c, c, 1.0);
}
*/


void main() 
{ 
	float x =  vsShadowCoord.x / (vsShadowCoord.w * 2) + 0.5;
	float y =  vsShadowCoord.y / (vsShadowCoord.w * 2) + 0.5;

  	//float visibility = texture(depthTexture, vec3(x, y, vsShadowCoord.z / vsShadowCoord.w ));//LinearizeDepth(vec2(x, y));
	float visibility = LinearizeDepth(vec2(x, y));//texture2D(depthTexture, vec2(x, y)).x;
	//visibility = visibility * 0.5 + 0.5;

//texture2D(depthTexture, vec2(x, y)).x;
	color = vec4(visibility, visibility, visibility, 1.0);
}
