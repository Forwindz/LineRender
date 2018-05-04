#pragma once
#include "PreCompile.h"
namespace Shape
{

	struct LineVertices
	{
		glm::vec3 pos;
		glm::vec3 color;
		float w;//index 0.5,1.5,2.5...
		float a=1.0f;//importance
	};
	typedef std::vector<LineVertices> Line;

	/*
	begin: the beginning of the line
	curve: the lower, the line will be more smooth
	count: the number of points
	*/
	void RandomGenerateLine(Line& line, 
		const glm::vec3 begin,const float curve,const unsigned int count);

	class LineStrip
	{
	public:
		void Init(Line& _line,float _width,
			glm::vec3 _beginDir,
			glm::vec4 _color);
		void Init(std::vector<Line*> _line, float width, glm::vec3 _beginDir, int totalSeg);
		void BindData(unsigned int lineDataSize,void* lineData,
					unsigned int indicesSize,void* indicesData);
		void Render();
		int GetSize()const;
		std::vector<float> g;//importance for each segment
		std::vector<int> gapRecord;
		int segNum;
	protected:
		GLuint lineData;
		GLuint indicesData;
		int size;//the number of segments
		int indiceSize;
		bool breakLine;
		//record segments:1~500 501~854 855~xxx are adjacent, so we record 500, 854;
		//math::HugeMatrix<float> q;
		//math::HugeMatrix<float> alpha;
	};

	//随机选择一组线段视为重要线段
	void RandomGenerateLineStrip(LineStrip& ls, 
		const glm::vec3 dir, const float curve, 
		const unsigned int count, const unsigned int groupCount, const int segNum);

}