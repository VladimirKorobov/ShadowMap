#version 330 core
layout(location = 0) out vec4 color;

uniform sampler2D DepthTexture;

in vec4 ShadowCoord;

void main(void)
{
	color = texture( DepthTexture, ShadowCoord.xy);
	//color = vec4(c, c, c, 1.0);
}