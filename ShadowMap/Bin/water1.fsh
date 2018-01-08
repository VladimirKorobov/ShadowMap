#version 330 core
in struct Vertex
{
	vec3 vertex;
	vec3 normal;
	float waterLevel;
	float alpha;
} vert;


uniform struct Light
{
	vec3 dir;
} light;


layout(location = 0) out vec4 color;

void main(void)
{
	color = vec4(0.0f, 0.0f, 1.0f, 1);//discard;
	float kd = max(-dot(light.dir, vert.normal), 0);

	//float kd = abs(dot(light.dir, vert.normal));

	color = color * kd;
}