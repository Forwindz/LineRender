#version 420 core

layout(early_fragment_tests) in;

layout(binding = 0, r32ui) uniform uimage2D head_pointer_image;
layout(binding = 1, rgba32ui) uniform writeonly uimageBuffer list_buffer;

layout(binding = 0, offset = 0) uniform atomic_uint list_counter;

/*layout(location = 0)*/ out vec4 color;

in vec3 frag_position;
//in vec3 frag_normal;
in vec3 surface_color;
in float wIndex;

void main(void)
{
	uint index;
	uint old_head;
	uvec4 item;
	vec4 frag_color;

	index = atomicCounterIncrement(list_counter);

	old_head = imageAtomicExchange(head_pointer_image, ivec2(gl_FragCoord.xy), uint(index));

	vec4 modulator =vec4(surface_color, 1.0f);
	item.x = old_head;
	item.y = packUnorm4x8(modulator);
	item.z = floatBitsToUint(gl_FragCoord.z);
	item.w = floatBitsToUint(wIndex);

	imageStore(list_buffer, int(index), item);

	frag_color = modulator;

	color = vec4(0.0f,0.0f,0.0f,0.0f);
}
