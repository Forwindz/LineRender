#pragma once
#include "PreCompile.h"
#include <sstream>
namespace Shape
{
	//detail data for segments' point
	struct LineVertices
	{
		glm::vec3 pos;
		glm::vec3 n;
		glm::vec2 tex;
		unsigned int lineId;//in which line?
		float w;
	};

	typedef std::vector<LineVertices> Line;

	class LineStrip
	{
	public:
		void LoadFromFile(const std::string& path,const int segNum, float width = 0.1f, 
			glm::vec3 beginDir = glm::vec3(1.0f, 0.0f, 0.0f));
		void Init(const std::vector<Line*> _line, float width, glm::vec3 _beginDir, int totalSeg);
		void BindData(unsigned int lineDataSize,void* lineData,
					unsigned int indicesSize,void* indicesData);
		void Render();
		int GetSize()const;
		std::vector<float> g;//importance for each segment
		int segNum;
		glm::vec3 min, max;
	protected:
		void reformLine(std::vector<Line*>& l, std::vector<float>& gg, const int newSeg);
		void updateMinMax(glm::vec3 pos);
		GLuint lineData;
		GLuint indicesData;
		int size;//the number of segments
		int indiceSize;
		bool breakLine;
	};


}