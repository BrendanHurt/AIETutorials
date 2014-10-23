#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 position[];
in float Lifetime[];
in float Lifespan[];

out vec4 Color;

uniform mat4 projectionView;
uniform mat4 cameraTransform;

uniform float sizeStart;
uniform float sizeEnd;

uniform vec4 colorStart;
uniform vec4 colorEnd;

void main ()
{
	Color = mix(colorStart, colorEnd, Lifetime[0] / Lifespan[0]);
	
	//calulates size and creates the corners of a quad
	float HalfSize = mix(sizeStart, sizeEnd, Lifetime[0] / Lifespan[0]) * 0.5f;
	
	vec3 Corners[4];
	Corners[0] = vec3( HalfSize, -HalfSize, 0);
	Corners[1] = vec3( HalfSize,  HalfSize, 0);
	Corners[2] = vec3(-HalfSize, -HalfSize, 0);
	Corners[3] = vec3(-HalfSize,  HalfSize, 0);
	
	//billboard
	vec3 zAxis = normalize(cameraTransform[3].xyz - position[0]);
	vec3 xAxis = cross(cameraTransform[1].xyz, zAxis);
	vec3 yAxis = cross(zAxis, xAxis);
	mat3 Billboard = mat3(xAxis, yAxis, zAxis);
	
	gl_Position = projectionView * vec4(Billboard * Corners[0] + position[0], 1);
	EmitVertex();
	
	gl_Position = projectionView * vec4(Billboard * Corners[1] + position[0], 1);
	EmitVertex();
	
	gl_Position = projectionView * vec4(Billboard * Corners[2] + position[0], 1);
	EmitVertex();
	
	gl_Position = projectionView * vec4(Billboard * Corners[3] + position[0], 1);
	EmitVertex();
}