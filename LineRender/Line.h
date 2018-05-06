#pragma once
#include "PreCompile.h"
namespace Shape
{

	//basic data for segments' point
	struct LineVerticeBasicInfo
	{
		glm::vec3 pos;
		glm::vec3 color;
	};

	//detail data for segments' point
	struct LineVertices
	{
		glm::vec3 pos;
		glm::vec3 color;
		float w;//index 0.5,1.5,2.5...
		glm::vec3 n;
	};
	typedef std::vector<LineVerticeBasicInfo> LineBasicData;
	typedef std::vector<LineVertices> Line;

	/*
	begin: the beginning of the line
	curve: the lower, the line will be more smooth
	count: the number of points
	*/
	void RandomGenerateLine(LineBasicData& line,
		const glm::vec3 begin,const float curve,const unsigned int count);
	void RandomGenerateLineStra(LineBasicData& line,
		const glm::vec3 begin, const float curve, const unsigned int count);

	class LineStrip
	{
	public:
		void Init(const std::vector<LineBasicData*> _line, float width, glm::vec3 _beginDir, int totalSeg);
		void BindData(unsigned int lineDataSize,void* lineData,
					unsigned int indicesSize,void* indicesData);
		void Render();
		int GetSize()const;
		std::vector<float> g;//importance for each segment
		//record segments:1~500 501~854 855~xxx are adjacent, so we record 500, 854;
		std::vector<int> gapRecord;
		int segNum;
	protected:
		GLuint lineData;
		GLuint indicesData;
		int size;//the number of segments
		int indiceSize;
		bool breakLine;
	};

	//随机选择一组线段视为重要线段
	void RandomGenerateLineStrip(LineStrip& ls, 
		const glm::vec3 dir, const float curve, 
		const unsigned int count, const unsigned int groupCount, const int segNum);

}