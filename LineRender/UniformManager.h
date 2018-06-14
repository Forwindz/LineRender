#pragma once
#include "PreCompile.h"
namespace Render
{

	template<typename T>
	class UniformProcess;

	template<typename T>
	class UniformBlock
	{
	public:
		UniformBlock();
		~UniformBlock();
		void BindUniform(GLuint program,const std::string& str,const size_t len);
		void BindUniforms(GLuint program,const std::string* str, const size_t* len,const int length);
		void BindUniforms(UniformProcess<T> up);
		T data;
	private:

		std::vector<int> eleSize;
		std::vector<size_t> loc;
		std::vector<int> type;
	};

	template<typename T>
	class UniformProcess
	{
	public:
		virtual initBlock(UniformBlock<T> blk)=0;
	};

}