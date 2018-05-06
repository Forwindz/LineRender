#version 420 core

struct Ambientlights
{
	vec3 color;
	float intensity;
};

struct Directionlights
{
	vec3 color;
	float intensity;
	vec3 direct;
};

struct LightData
{
	Ambientlights al;
	Directionlights dl;
	vec3 tempColor;
};
//layout(early_fragment_tests) in;
uniform LightData lights;
layout(binding = 0, r32ui) uniform uimage2D head_pointer_image;
layout(binding = 1, rgba32ui) uniform writeonly uimageBuffer list_buffer;
layout(binding = 0, offset = 0) uniform atomic_uint list_counter;


out vec4 color;

//in vec3 frag_position;
in vec3 surface_color;
in float wIndex;
in vec3 normalVec;

void main(void)
{
	vec4 modulator;
	//lights compute
	//surface_color=vec3(1.0f);
	modulator=vec4(surface_color*lights.al.intensity*lights.al.color,1.0f);
	modulator.xyz+=surface_color*lights.dl.intensity*lights.dl.color*(dot(lights.dl.direct,normalVec))+lights.tempColor+vec3(0.0f);
	
	//
	uint index;
	uint old_head;
	uvec4 item;
	vec4 frag_color;

	index = atomicCounterIncrement(list_counter);

	old_head = imageAtomicExchange(head_pointer_image, ivec2(gl_FragCoord.xy), uint(index));

	item.x = old_head;
	item.y = packUnorm4x8(modulator);
	item.z = floatBitsToUint(gl_FragCoord.z);
	item.w = floatBitsToUint(wIndex);

	imageStore(list_buffer, int(index), item);

	frag_color = modulator;

	color = vec4(0.0f);
}
