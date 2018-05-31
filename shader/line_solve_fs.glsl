#version 430 core

struct Params
{
	float p,q,r,l;
};

uniform Params params;

// The per-pixel image containing the head pointers
layout(binding = 0, r32ui) uniform uimage2D head_pointer_image;
// Buffer containing linked lists of fragments
layout(binding = 1, rgba32ui) uniform uimageBuffer list_buffer;
// g^2
layout(binding = 2, r32f) uniform imageBuffer gs;
// g behind
//layout(binding = 3, r32ui) uniform uimageBuffer gb;
// g front
//layout(binding = 4, r32ui) uniform uimageBuffer gf;
// alpha
layout(binding = 5, r32ui) uniform uimageBuffer alphaList;
// This is the output color
layout(location = 0) out vec4 color;

// This is the maximum number of overlapping fragments allowed
#define MAX_FRAGMENTS 15

// Temporary array used for sorting fragments
uvec4 fragment_list[MAX_FRAGMENTS];
float frag_gs[MAX_FRAGMENTS];
int frag_id[MAX_FRAGMENTS];

void main(void)
{
	uint current_index;
	uint fragment_count = 0;

	current_index = imageLoad(head_pointer_image, ivec2(gl_FragCoord).xy).x;

	while (current_index != 0 && fragment_count < MAX_FRAGMENTS)
	{
		uvec4 fragment = imageLoad(list_buffer, int(current_index));
		fragment_list[fragment_count] = fragment;
		//frag_index_list[fragment_count] = current_index;
		current_index = fragment.x;
		fragment_count++;
	}

	uint i, j;
	//sort
	if (fragment_count > 1)
	{

		for (i = 0; i < fragment_count - 1; i++)
		{
			for (j = i + 1; j < fragment_count; j++)
			{
				uvec4 fragment1 = fragment_list[i];
				uvec4 fragment2 = fragment_list[j];

				float depth1 = unpackUnorm2x16(fragment1.z).x;
				float depth2 = unpackUnorm2x16(fragment2.z).x;

				if (depth1 < depth2)
				{
					fragment_list[i] = fragment2;
					fragment_list[j] = fragment1;
				}
			}
		}

	}
	
	float gsum=0;
	for(i = 0; i < fragment_count; i++)
	{
		//(l_id<<22)|s_id     10+22 bit
		//llll llll llss ssss ssss ssss ssss ssss
		frag_id[i]=int(fragment_list[i].w&(0x003FFFFF));
		frag_gs[i]=(imageLoad(gs,frag_id[i]).x);
		gsum+=frag_gs[i]*frag_gs[i];
	}
	
	float gfront_sum=0;
	float gbehind_sum=0;
	for(i = 0; i < fragment_count; i++)
	{
		gbehind_sum=gsum-frag_gs[i]*frag_gs[i]-gfront_sum;
		vec4 alphaData=imageLoad(alphaList, frag_id[i]);
		alphaData.x=params.p/(params.p+pow((1-frag_gs[i]),2*params.l)*
			(gbehind_sum*params.q+params.r*gfront_sum));
		imageAtomicMin(alphaList,frag_id[i],floatBitsToUint(alphaData.x));
		gfront_sum+=frag_gs[i]*frag_gs[i];
	}
	//compute color
	vec4 final_color;
	//final color
	for (int i = 0; i < fragment_count; i++)
	{
		vec4 modulator = unpackUnorm4x8(fragment_list[i].y);
		final_color = mix(final_color, modulator, unpackUnorm2x16(fragment_list[i].z).y);//alpha
	}
	color=final_color;
}
