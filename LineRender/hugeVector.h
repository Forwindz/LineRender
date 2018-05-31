#pragma once
namespace mth
{
	/*
	an unsafe vector class.
	*/
	typedef unsigned int sizelen;
	template<typename T>
	class HugeVector
	{
	public:
		HugeVector();
		HugeVector(sizelen size);
		~HugeVector();
		void SetSize(sizelen newSize);
		void Fill(const T v);
		void CopyData(T* copyTo);
		T* getMemPtr() const;
		T& operator[](const sizelen pos);
		
	protected:
		T * data = nullptr;
		sizelen length;
		void clear();
	};

	typedef HugeVector<float> fhvec;
	typedef HugeVector<unsigned int> uihvec;

	template<typename T>
	inline HugeVector<T>::HugeVector()
	{
		SetSize(0);
	}

	template<typename T>
	inline HugeVector<T>::HugeVector(sizelen size)
	{
		SetSize(size);
	}

	template<typename T>
	inline HugeVector<T>::~HugeVector()
	{
		clear();
	}

	template<typename T>
	inline void HugeVector<T>::SetSize(sizelen newSize)
	{
		if (newSize != length)
		{
			clear();
			length = newSize;
			if (newSize == 0)return;
			data = new T[newSize];
		}
	}

	template<typename T>
	inline void HugeVector<T>::Fill(const T v)
	{
#pragma omp for
		for (int i = 0; i < length; ++i)
		{
			data[i] = v;
		}
	}

	template<typename T>
	inline void HugeVector<T>::CopyData(T * copyTo)
	{
		memcpy_s(copyTo, length * sizeof(T), data, length * sizeof(T));
	}

	template<typename T>
	inline T * HugeVector<T>::getMemPtr() const
	{
		return data;
	}

	template<typename T>
	inline T & HugeVector<T>::operator[](const sizelen pos)
	{
		return data[pos];
	}

	template<typename T>
	inline void HugeVector<T>::clear()
	{
		if (data != nullptr)
		{
			delete[] data;
		}
		data = nullptr;
	}
}