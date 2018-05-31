#pragma once
#include "PreCompile.h"
#include <sstream>
namespace Shape
{
	//detail data for segments' point
	struct LineVertices
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec3 n;
		//float gsqr;//g^2
		unsigned int lineId;//in which line?
		unsigned int w;
	};

	typedef std::vector<LineVertices> Line;

	class LineStrip
	{
	public:
		void LoadFromFile(const std::string& path, float width = 0.1f, 
			glm::vec3 beginDir = glm::vec3(1.0f, 0.0f, 0.0f));
		void Init(const std::vector<Line*> _line, float width, glm::vec3 _beginDir, int totalSeg);
		void BindData(unsigned int lineDataSize,void* lineData,
					unsigned int indicesSize,void* indicesData);
		void Render();
		int GetSize()const;
		std::vector<float> g;//importance for each segment
		int segNum;
	protected:
		GLuint lineData;
		GLuint indicesData;
		int size;//the number of segments
		int indiceSize;
		bool breakLine;
	};


}