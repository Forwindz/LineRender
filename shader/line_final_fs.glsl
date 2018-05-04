#version 420 core



// The per-pixel image containing the head pointers
layout(binding = 0, r32ui) uniform uimage2D head_pointer_image;
// Buffer containing linked lists of fragments
layout(binding = 1, rgba32ui) uniform uimageBuffer list_buffer;
// H Matrix
//layout(binding = 2, r8) uniform image2D mh;

layout(binding = 3, r8) uniform imageBuffer alphaList;

// This is the output color
layout(location = 0) out vec4 color;

// This is the maximum number of overlapping fragments allowed
#define MAX_FRAGMENTS 40

// Temporary array used for sorting fragments
uvec4 fragment_list[MAX_FRAGMENTS];
//uint frag_index_list[MAX_FRAGMENTS];

void main(void)
{
	
	uint current_index;
	uint fragment_count = 0;

	current_index = imageLoad(head_pointer_image, ivec2(gl_FragCoord).xy).x;

	while (current_index != 0 && fragment_count < MAX_FRAGMENTS)
	{
		uvec4 fragment = imageLoad(list_buffer, int(current_index));
		fragment_list[fragment_count] = fragment;
		current_index = fragment.x;
		fragment_count++;
	}
	//final color
	vec4 final_color = vec4(0.0);

	
	for (int i = 0; i < fragment_count; i++)
	{
		float iw,delta;
		int pos;
		float alpha;
		float data1,data2;
		iw=uintBitsToFloat(fragment_list[i].w);
		pos=int(iw);
		delta=iw-pos;
		data1=imageLoad(alphaList,pos).x;
		data2=imageLoad(alphaList,pos+1).x;
		alpha=data2*delta+data1*(1-delta);
		//alpha=1.0f;
		vec4 modulator = unpackUnorm4x8(fragment_list[i].y);
		final_color = /*vec4(0.1f,0.1f,0.1f,1.0f)+*/mix(final_color, modulator, alpha);
		final_color.a=1.0f;
	}
	//color = vec4(0.1f,0.8f,0.1f,1.0f);
	color=final_color;//+vec4(0.1f,0.1f,0.1f,1.0f);

	// color = vec4(float(fragment_count) / float(MAX_FRAGMENTS));
}
