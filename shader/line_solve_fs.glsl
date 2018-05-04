#version 420 core



// The per-pixel image containing the head pointers
layout(binding = 0, r32ui) uniform uimage2D head_pointer_image;
// Buffer containing linked lists of fragments
layout(binding = 1, rgba32ui) uniform uimageBuffer list_buffer;
// H Matrix
layout(binding = 2, r8) uniform image2D mh;

// This is the output color
layout(location = 0) out vec4 color;

// This is the maximum number of overlapping fragments allowed
#define MAX_FRAGMENTS 40

// Temporary array used for sorting fragments
uvec4 fragment_list[MAX_FRAGMENTS];
uint frag_index_list[MAX_FRAGMENTS];

void main(void)
{
	uint current_index;
	uint fragment_count = 0;

	current_index = imageLoad(head_pointer_image, ivec2(gl_FragCoord).xy).x;

	while (current_index != 0 && fragment_count < MAX_FRAGMENTS)
	{
		uvec4 fragment = imageLoad(list_buffer, int(current_index));
		fragment_list[fragment_count] = fragment;
		frag_index_list[fragment_count] = current_index;
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

				float depth1 = uintBitsToFloat(fragment1.z);
				float depth2 = uintBitsToFloat(fragment2.z);

				if (depth1 < depth2)
				{
					fragment_list[i] = fragment2;
					fragment_list[j] = fragment1;
				}
			}
		}

	}
	//store
	if (fragment_count > 1)
	{
		for (i = 0; i < fragment_count; i++)
		{
			imageStore(list_buffer,int(frag_index_list[i]),fragment_list[i]);
		}
	}

	//compute H
	uint innerPos;
	float iw;
	float jw;
	float v;
	ivec2 mhPos;
	float tempX;
	vec4 tempVec;
	if (fragment_count > 1)
	{

		for (uint iii = 0; iii < fragment_count - 1; iii++)
		{
			iw=uintBitsToFloat(fragment_list[iii].w);
			i=uint(floor(iw+0.5));
			for (uint jjj = iii + 1; jjj < fragment_count; jjj++)
			{
				jw=uintBitsToFloat(fragment_list[jjj].w);
				j=uint(floor(jw));
				v=jw-floor(jw);//frac
				
				mhPos=ivec2(i,j);
				tempVec=imageLoad(mh,mhPos);
				tempVec.x+=1-v;
				imageStore(mh,mhPos,tempVec);
				
				mhPos=ivec2(i,j+1);
				tempVec=imageLoad(mh,mhPos);
				tempVec.x+=v;
				imageStore(mh,mhPos,tempVec);	
				
			}
			
		}
	}

	//final color
	vec4 final_color = vec4(0.0);
/*
	for (i = 0; i < fragment_count; i++)
	{
		vec4 modulator = unpackUnorm4x8(fragment_list[i].y);
		
		final_color = modulator;
	}*/
	final_color=vec4(0.0f,
	0.8f,
	0.0f,
	0.0f);
	color = final_color;

	// color = vec4(float(fragment_count) / float(MAX_FRAGMENTS));
}
