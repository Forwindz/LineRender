#version 430 core  

uniform float s;
layout(local_size_x = 32) in;  

layout(binding = 5, r32ui) uniform uimageBuffer alphaList;
layout(binding = 4, r32f) uniform imageBuffer realAlphaList;
void main(void)  
{  
	int pos=int(gl_GlobalInvocationID.x);
	float org_data = uintBitsToFloat(imageLoad(alphaList,pos).x);
	vec4 storeData=vec4(org_data);
	imageStore(realAlphaList,pos,storeData);
	barrier();
	
	for(int i=0;i<s;++i)	
	{
		float data1 = imageLoad(realAlphaList,pos-1).x;
		float data2 = imageLoad(realAlphaList,pos).x;
		float data3 = imageLoad(realAlphaList,pos+1).x;
		float data;
		data=(data1+data3+data2)/3.0f;
		//data=data2+0.8*(data-data2);
		//data=min(data2,data);
		//data=max(data,0.1f);
		storeData=vec4(data);
		barrier();
		imageStore(realAlphaList,pos,storeData);
	}
	imageStore(alphaList,pos,uvec4(floatBitsToUint(1.0f)));
}  