#version 430 core

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

struct SpecularLight
{
	vec3 eyePos;
	float intensity;
	float power;
};


struct LightData
{
	Ambientlights al;
	Directionlights dl;
	SpecularLight sl;
};
//layout(early_fragment_tests) in;
uniform LightData lights;
layout(binding = 0, r32ui) uniform uimage2D head_pointer_image;
layout(binding = 1, rgba32ui) uniform writeonly uimageBuffer list_buffer;
layout(binding = 0, offset = 0) uniform atomic_uint list_counter;

layout(binding = 6)uniform sampler2D gColorMap; 

out vec4 color;

in vec3 normalVec;
in flat uint l_id;
in flat int s_id;
in float alpha;
in vec3 worldPos;
in vec2 texCoord;
in vec3 border;

void main(void)
{
	vec4 modulator;
	vec3 totalColor;
	float px=0.5f-texCoord.x;
	vec3 realNVec = normalize(px*border+sqrt(1-px*px)*normalVec);
	//lights compute
	vec3 surface_color=texture(gColorMap, texCoord.xy).xyz;
	totalColor=surface_color*lights.al.intensity*lights.al.color;
	vec3 dColor=surface_color*lights.dl.intensity*lights.dl.color*
				(dot(lights.dl.direct,realNVec));
	totalColor+=clamp(dColor,0,1);
	vec3 VertexToEye = normalize(worldPos-lights.sl.eyePos);
    vec3 LightReflect = normalize(reflect(lights.dl.direct, realNVec));
    float SpecularFactor = dot(VertexToEye, LightReflect);
    if (SpecularFactor > 0) {
        SpecularFactor = pow(SpecularFactor, lights.sl.power);
        totalColor+= vec3(lights.dl.color * lights.sl.intensity * SpecularFactor);
    }
	modulator=vec4(clamp(totalColor,0,1),alpha);
	//modulator=vec4(1.0f,0.2f,0.5f,1.0f)
	//light compute finish
	
	uint index;
	uint old_head;
	uvec4 item;
	vec4 frag_color;
	//10 bit line_id + 22 bit seg_id;
	uint data2=(l_id<<22)|int(s_id);
	
	index = atomicCounterIncrement(list_counter);

	old_head = imageAtomicExchange(head_pointer_image, ivec2(gl_FragCoord.xy), uint(index));

	item.x = old_head;
	item.y = packUnorm4x8(modulator);
	item.z = floatBitsToUint(gl_FragCoord.z);
	item.w = data2;

	imageStore(list_buffer, int(index), item);

	frag_color = modulator;

	color = vec4(1.0f);
}
