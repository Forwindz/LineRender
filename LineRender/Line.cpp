#include "Line.h"
//part of code from https://github.com/tobguent/small-doo/blob/master/lines.hpp
//I modified some of them to suit my program
void Shape::LineStrip::LoadFromFile(const std::string& path, float width, glm::vec3 beginDir)
{
	std::ifstream ifs;
	g.clear();
	std::vector<int> indices;
	std::vector<glm::vec3> p;
	ifs.open(path, std::ios::binary);
	int lineCount = 0;

	while (ifs.good())
	{
		std::string str;
		std::getline(ifs, str);
		switch (str[0])
		{
		case 'v':
			if (str[1] == ' ')//vertex position info
			{
				glm::vec3 pos;
				if (sscanf_s(str.c_str(), "v %f %f %f", &(pos.x), &(pos.y), &(pos.z)) == 3)
					p.push_back(pos);
			}
			else if (str[1] == 't')//importance
			{
				float imp;
				if (sscanf_s(str.c_str(), "vt %f", &imp) == 1)
					g.push_back(imp);
			}
			break;
		case 'l'://indices
		{
			std::stringstream ss(str.substr(2));//remove [l ]
			int ind;
			while (ss >> ind)
			{
				indices.push_back(ind);
			}
			break;
		}
		case 'g'://a new line
			lineCount++;
			indices.push_back(-1);
#ifdef DEBUG
			std::cout << "reading line> " << str << "\n";
#endif // DEBUG
			break;
		}
	}
	indices.push_back(-1);
	std::cout << "Line num=" << lineCount << std::endl;
	//begin to process the data
	//data:p=vertex position || indices= vertex info
	Line* li = nullptr;
	std::vector<Line*> lines;
	const int len = indices.size();
	int totalSeg = 0;
	unsigned int nowLine = 0;
	for (int i = 0; i < len; i++)
	{
		int indInfo = indices[i];
		if (indInfo == -1)
		{
			if (li != nullptr && li->size()>=2)
			{
				lines.push_back(li);
				++nowLine;
#ifdef DEBUG
				std::cout << "processed segments> " << totalSeg << "\n";
#endif // DEBUG

			}
			li = new Line();
		}
		else
		{
			totalSeg++;
			li->push_back({ p[indInfo - 1], glm::vec3(255/256.0f,240/256.0f,189/256.0f), WHITE, nowLine,(unsigned int)indices[i] });
		}
	}
	Init(lines, width, beginDir,g.size());
}

void Shape::LineStrip::Init(
	const std::vector<Line*> lines, 
	float _width, glm::vec3 _baseDir, int totalSeg)
{
	//提前计算，不使用曲面shader

	std::vector<LineVertices> d;
	std::vector<unsigned int> indices;
	glm::vec3 temp, pre, linev;
	size = 0;
	indiceSize = 0;
	breakLine = false;
	segNum = totalSeg;


	for (unsigned int i = 0; i < lines.size(); i++)
	{
		size += lines[i]->size();
	}
	size <<= 1;
	indiceSize = size + lines.size();
	d.reserve(size);
	indices.reserve(indiceSize);

	const float wStep = totalSeg / (float)size;
	unsigned int i;
	i = -1;
	for (unsigned int k = 0; k < lines.size(); k++)
	{
		Line& lptr = *lines[k];
		unsigned int jl = 0;

		pre = glm::normalize(glm::cross(_baseDir, lptr[1].pos - lptr[0].pos));

		for (jl = 0; jl < lptr.size() - 1; jl++)
		{
			++i;
			linev = lptr[jl + 1].pos - lptr[jl].pos;
			temp = (glm::normalize(glm::cross(_baseDir, linev)) + pre)*0.5f;
			linev = glm::normalize(glm::cross(linev, temp));
			lptr[jl].w = i;
			lptr[jl].pos += temp * (_width*0.5f);
			lptr[jl].n = linev;
			d.push_back(lptr[jl]);
			lptr[jl].pos -= temp * _width;
			d.push_back(lptr[jl]);
			indices.push_back((i << 1));
			indices.push_back((i << 1) + 1);
			pre = temp;
		}
		++i;
		linev = lptr[jl].pos - lptr[jl - 1].pos;
		temp = glm::normalize(glm::cross(_baseDir, lptr[jl].pos - lptr[jl - 1].pos));
		linev = glm::normalize(glm::cross(linev, temp));
		lptr[jl].w = i;
		lptr[jl].n = linev;
		lptr[jl].pos += temp * (_width*0.5f);
		d.push_back(lptr[jl]);
		lptr[jl].pos -= temp * _width;
		d.push_back(lptr[jl]);
		indices.push_back((i << 1));
		indices.push_back((i << 1) + 1);

		if (k != lines.size() - 1)
		{
			indices.push_back(-1);//断点
			breakLine = true;
		}
	}
	BindData(sizeof(glm::vec3)*(size), d.data(),
		sizeof(unsigned int)*(size), indices.data());

	//clear data
	for (int i = 0; i < lines.size(); i++)
	{
		if (lines[i] != nullptr)delete lines[i];
	}
}

void Shape::LineStrip::BindData(unsigned int lineDataSize, void * lineDataPtr, 
	unsigned int indicesSize, void * indicesDataPtr)
{
	glGenBuffers(1, &lineData);
	glBindBuffer(GL_ARRAY_BUFFER, lineData);
	glBufferData(GL_ARRAY_BUFFER, lineDataSize, lineDataPtr, GL_STATIC_DRAW);

	glGenBuffers(1, &indicesData);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indicesDataPtr, GL_STATIC_DRAW);
}

void Shape::LineStrip::Render()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER,lineData);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertices), 0);//pos
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertices), (void*)sizeof(glm::vec3));//color
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertices), (void*)(sizeof(glm::vec3) * 2));//n
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(LineVertices), (void*)(sizeof(glm::vec3) * 3));//line id
	glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(LineVertices), (void*)(sizeof(glm::vec3) * 3 +sizeof(unsigned int)));//line id
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesData);
	if (!breakLine)
	{
		glDrawElements(GL_TRIANGLE_STRIP, indiceSize, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(-1);
		glDrawElements(GL_TRIANGLE_STRIP, indiceSize, GL_UNSIGNED_INT, 0);
		glDisable(GL_PRIMITIVE_RESTART);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}

int Shape::LineStrip::GetSize() const
{
	return size;
}