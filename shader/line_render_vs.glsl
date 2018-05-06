#version 420 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in float w;
layout(location = 3) in vec3 n;

uniform mat4 gWorld;
uniform mat4 gWVP;

out vec3 surface_color;
out float wIndex;
out vec3 normalVec;

void main(void)
{
	surface_color = color;
	//frag_position = (gWVP*vec4(position,1.0f)).xyz;
	wIndex=w;
	gl_Position = gWVP * vec4(position,1.0f);
	normalVec=n;
}
