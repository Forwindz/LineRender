#pragma once
#include "PreCompile.h"
namespace Render {
	template <typename T>
	class UniformStruct
	{
	public:
		UniformStruct();
		~UniformStruct();
		void Init(GLuint program);
		void UpdateData();
		T data;

		static int numCount;
		const static std::string * names;
		const static int* fcounts;
	private:
		void pushUData(const int n, GLuint loc, float* data);
		GLuint* pos;
	};

	template<typename T>
	inline UniformStruct<T>::UniformStruct()
	{
		pos = new GLuint[numCount];
	}

	template<typename T>
	inline UniformStruct<T>::~UniformStruct()
	{
		delete[] pos;
	}

	template<typename T>
	inline void UniformStruct<T>::Init(GLuint program)
	{
		for (int i = 0; i < numCount; i++)
		{
			pos[i] = glGetUniformLocation(program, names[i].c_str());
		}
	}

	template<typename T>
	inline void UniformStruct<T>::UpdateData()
	{
		float* tempPtr = (float*)&data;
		for (int i = 0; i < numCount; i++)
		{
			pushUData(fcounts[i], pos[i], tempPtr);
			tempPtr += fcounts[i];
		}
	}
	template<typename T>
	inline void UniformStruct<T>::pushUData(const int n, GLuint loc, float * data)
	{
		switch (n)
		{
		case 1:
			glUniform1f(loc, data[0]);
			break;
		case 2:
			glUniform2f(loc, data[0], data[1]);
			break;
		case 3:
			glUniform3f(loc, data[0], data[1], data[2]);
			break;
		}
	}
}