#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform struct Transform
{
	mat4 M;
	mat4 V;
	mat4 P;
	mat3 N;
} transform;

out struct Vertex
{
	vec3 vertex;
	vec3 normal;
	float waterLevel;
	float alpha;
} vert;

void main()
{
	vec4 vertex   = transform.M * vec4(position, 1.0);
	vert.normal = normalize(transform.N * normal);

	gl_Position = transform.V * vertex;
	gl_Position = vec4(vec3(gl_Position) * 0.9, 1);
	gl_Position = transform.P * gl_Position;
	vert.alpha = 10;
}

