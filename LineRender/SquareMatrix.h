#pragma once
#include "PreCompile.h"

namespace math
{
	template<typename T>
	class HugeMatrix
	{
	public:
		HugeMatrix();
		~HugeMatrix();
		void Init(const int _n);
		void Init(const int _n, const int _m);
		void SetData(T* data, const int _n, const int _m);
		void Clear();
		T& at(const int x, const int y);
		T* getData() const;
	protected:
		void freeMemory();
		int n,m;//size row:n; col:m
		T* data;
	};

	template<typename T>
	inline HugeMatrix<T>::HugeMatrix()
	{
		n = 0;
	}

	template<typename T>
	inline HugeMatrix<T>::~HugeMatrix()
	{
		freeMemory();
	}

	template<typename T>
	inline void HugeMatrix<T>::Init(const int _n)
	{
		freeMemory();
		m = n = _n;
		data = new T[n*n];
	}
	template<typename T>
	inline void HugeMatrix<T>::Init(const int _n, const int _m)
	{
		freeMemory();
		n = _n;
		m = _m;
		data = new T[n*m];
	}
	template<typename T>
	inline void HugeMatrix<T>::SetData(T * _data, const int _n, const int _m)
	{
		freeMemory();
		data = _data;
		n = _n;
		m = _m;
	}
	template<typename T>
	inline void HugeMatrix<T>::Clear()
	{
		memset(data, 01, sizeof(T)*n*n);
	}
	template<typename T>
	inline T & HugeMatrix<T>::at(const int x, const int y)
	{
		return data[x + y * m];
	}
	template<typename T>
	inline T * HugeMatrix<T>::getData() const
	{
		return data;
	}
	template<typename T>
	inline void HugeMatrix<T>::freeMemory()
	{
		if (n >= 0)delete[] data;
	}
}