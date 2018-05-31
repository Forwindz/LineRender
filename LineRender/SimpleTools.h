#pragma once
#include "PreCompile.h"

namespace Tool
{
	//force to get members' function address
	template<typename dst_type, typename src_type>
	dst_type pointer_cast(src_type src)
	{
		return *static_cast<dst_type*>(static_cast<void*>(&src));
	}
	//force to get members' function address
	template<typename dst_type, typename src_type>
	dst_type union_cast(src_type src)
	{
		union {
			src_type s;
			dst_type d;
		}u;
		u.s = src;
		return u.d;
	}

	template<typename dst_type,typename src_type>
	dst_type cpy_cast(src_type src)
	{
		dst_type temp;
		memcpy(&temp, &src, sizeof(src_type));
		return temp;
	}
	/*
	float: 0.0~1.0 >>> 2^24
	(fixed point float 24bits for decimal part, 8bits for integer part)
	*/
	inline unsigned int floatToUint(const float v)
	{
		return (unsigned int)(v*(unsigned int)0x00FFFFFF);
	}

	inline float uintToFloat(const unsigned int v)
	{
		return (float)((float)v / (float)((unsigned int)0x00FFFFFF));
	}

//#define indirect_invoke(dst_type,src_obj,src_fun)\

	/*
	template<typename dst_type, typename src_type,typename src_func_type>
	dst_type invoke_cast(src_type* src,src_func_type fun)
	{
		dst_type temp;
		memcpy(&temp, &src, sizeof(src_type));
		return temp;
	}*/

	/*
	only for windows 32bit version
	__declspec(naked) void* __cdecl cdecl_cast(...)
	{
	__asm{
	mov eax,dword ptr[esp+4]
	ret
	}
	}
	*/
}