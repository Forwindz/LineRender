#version 430 core  

struct FadeParam{
	float s;
	float it_times;
	float fade_speed;
};

uniform FadeParam fp;
layout(local_size_x = 32) in;  

layout(binding = 5, r32ui) uniform uimageBuffer alphaList;
layout(binding = 4, r32f) uniform imageBuffer realAlphaList;
layout(binding = 7, r32f) uniform imageBuffer targetAlphaList;
void main(void)  
{  
	int pos=int(gl_GlobalInvocationID.x);
	float target=0;
	float org_data = uintBitsToFloat(imageLoad(alphaList,pos).x);
	vec4 storeData=vec4(org_data);
	imageStore(targetAlphaList,pos,storeData);
	barrier();
	
	for(int i=0;i<fp.it_times;++i)	
	{
		//calculate target alpha
		float data1 = imageLoad(targetAlphaList,pos-1).x;
		float data2 = org_data;
		float data3 = imageLoad(targetAlphaList,pos+1).x;
		float ss=(3-fp.s)/2;
		target=(data1*ss+data3*fp.s+data2*ss)/3.0f;
		
		//data=data2+0.8*(data-data2);
		//data=min(data2,data+0.1f);
		//data=max(data,0.1f);
		//storeData=vec4(data);
		barrier();
		imageStore(targetAlphaList,pos,vec4(target));
		barrier();
	}
	//fade to target alpha
	float curAlpha=imageLoad(realAlphaList,pos).x;
	curAlpha += (target-curAlpha)*fp.fade_speed;
	//urAlpha= target;
	imageStore(realAlphaList,pos,vec4(curAlpha));
	imageStore(alphaList,pos,uvec4(floatBitsToUint(1.0f)));
}  