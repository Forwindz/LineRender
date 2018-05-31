#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 n;
layout(location = 3) in uint line_id;
layout(location = 4) in uint seg_id;
layout(binding = 4, r32f) uniform imageBuffer realAlphaList;
uniform mat4 gWorld;
uniform mat4 gWVP;

out vec3 surface_color;
out vec3 normalVec;
out float alpha;
out flat uint l_id;
out flat uint s_id;

void main(void)
{
	surface_color = color;
	gl_Position = gWVP * vec4(position,1.0f);
	normalVec=n;
	l_id=line_id;
	s_id=seg_id;
	alpha=imageLoad(realAlphaList,int(seg_id)).x;
}
