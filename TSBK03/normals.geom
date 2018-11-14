#version 430 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 normal[];

const float FACTOR = 0.5;

void gen_normal_line(int pointIndex)
{
	gl_Position = gl_in[pointIndex].gl_Position;
	EmitVertex();
	gl_Position = gl_in[pointIndex].gl_Position + vec4(normalize(normal[pointIndex]), 0.0)*FACTOR;
	EmitVertex();
	EndPrimitive();
}

void main()
{
	gen_normal_line(0);
	gen_normal_line(1);
	gen_normal_line(2);
}