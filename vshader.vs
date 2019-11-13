#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float t;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 fragmentColor;

void main()
{
	gl_Position = projection * view  *model* vec4(aPos,1.0);
	fragmentColor=vec3(1.0f,1.0f,1.0f);
}