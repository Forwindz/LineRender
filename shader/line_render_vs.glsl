#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 dir; //normalized
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in uint line_id;
layout(location = 4) in float seg_id;
layout(binding = 4, r32f) uniform imageBuffer realAlphaList;
uniform mat4 gWorld;
uniform mat4 gWVP;

uniform vec3 viewVec;//normalized
uniform float width;

out vec3 normalVec;
out float alpha;
out flat uint l_id;
out flat int s_id;
out vec3 worldPos;
out vec2 texCoord;
out vec3 border;

//texCoord:
//odd Vertex:(1 or 0,0) even:(1 or 0,1)
//can be used for pos calculation

void main(void)
{
	//basic
	l_id=line_id;
	texCoord=tex_coord;
	int seg_intId=int(seg_id);
	float alpha1=imageLoad(realAlphaList,seg_intId).x;
	float alpha2=imageLoad(realAlphaList,seg_intId+1).x;
	float d=fract(seg_id);
	alpha = (1-d)*alpha1+d*alpha2;
	s_id=seg_intId;
	//position
	border = cross(dir,viewVec);
	gl_Position = gWVP * vec4((position+border*width*(tex_coord.x-0.5f)),1.0f);
	normalVec = cross(dir,border);
}
