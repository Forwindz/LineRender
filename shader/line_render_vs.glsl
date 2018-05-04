#version 420 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in float w;
//layout(location = 3) in float a;

//uniform mat4 model_matrix;
//uniform mat4 view_matrix;
//uniform mat4 projection_matrix;
uniform mat4 gWorld;
uniform mat4 gWVP;

//out gl_PerVertex
//{
	//vec4 gl_Position;
//};

out vec3 surface_color;
out vec3 frag_position;
out float wIndex;
//out vec3 frag_normal;

void main(void)
{/*
	vec4 offset = vec4(float(gl_InstanceID & 3) * 2.0,
		float((gl_InstanceID >> 2) & 3) * 2.0,
		float((gl_InstanceID >> 4) & 3) * 2.0, 0.0) -
		vec4(4.0, 4.0, 4.0, 0.0);*/

	surface_color = color;/*normalize(offset) * 0.5 + */

	//vec4 object_pos = (position + offset);
	//vec4 world_pos = model_matrix * object_pos;
	frag_position = (gWVP*vec4(position,1.0f)).xyz;//world_pos.xyz;
	//frag_normal = mat3(model_matrix * view_matrix) * normal;
	wIndex=w;
	gl_Position = gWVP * vec4(position,1.0f);//world_pos;
}
