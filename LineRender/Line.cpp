#include "Line.h"
//part of code from https://github.com/tobguent/small-doo/blob/master/lines.hpp
//I modified some of them to suit my program
void Shape::LineStrip::LoadFromFile(const std::string& path, const int segNums, float width, glm::vec3 beginDir)
{
	std::ifstream ifs;
	std::vector<int> indices;
	std::vector<glm::vec3> p;
	std::vector<float> g;
	g.clear();
	ifs.open(path, std::ios::binary);
	int lineCount = 0;
	bool first = true;
	min = max = WHITE;

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
				if (first) 
				{ 
					min = max = pos; 
					first = false; 
				}
				else
				{
					updateMinMax(pos);
				}
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
			if (lineCount % 10 == 0)std::cout << "reading line> " << str << "\n";
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
			}
			li = new Line();
		}
		else
		{
			totalSeg++;
			li->push_back({ p[indInfo - 1],WHITE,glm::vec2(0,0), nowLine,0 });
		}
	}
	reformLine(lines, g, segNums);
	Init(lines, width, beginDir,this->g.size());
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
	bool textCoordIndex = 0;
	const glm::vec2 textCoord[2][2] =
	{
		{glm::vec2(0,0),glm::vec2(1,0) },
		{glm::vec2(0,1),glm::vec2(1,1)}
	};

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
			//lptr[jl].w = i;
			lptr[jl].pos += temp * (_width*0.5f);
			lptr[jl].n = linev;
			lptr[jl].tex = textCoord[textCoordIndex][0];
			d.push_back(lptr[jl]);
			lptr[jl].tex = textCoord[textCoordIndex][1];
			textCoordIndex = !textCoordIndex;
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
		//lptr[jl].w = i;
		lptr[jl].n = linev;
		lptr[jl].pos += temp * (_width*0.5f);
		lptr[jl].tex = textCoord[textCoordIndex][0];
		d.push_back(lptr[jl]);
		lptr[jl].tex = textCoord[textCoordIndex][1];
		lptr[jl].pos -= temp * _width;
		d.push_back(lptr[jl]);
		indices.push_back((i << 1));
		indices.push_back((i << 1) + 1);

		//if (k != lines.size() - 1)
		{
			indices.push_back(-1);//断点
			breakLine = true;
		}
	}
	BindData(sizeof(glm::vec3)*(size), d.data(),
		sizeof(unsigned int)*(indices.size()), indices.data());

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertices), 
		0);//pos
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertices), 
		(void*)(sizeof(glm::vec3)));//n
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(LineVertices), 
		(void*)(sizeof(glm::vec3) * 2));//texture pos
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(LineVertices), 
		(void*)(sizeof(glm::vec3) * 2+sizeof(glm::vec2)));//line id
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(LineVertices),
		(void*)(sizeof(glm::vec3) * 2 +sizeof(unsigned int)+ sizeof(glm::vec2)));//segments id
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

void Shape::LineStrip::reformLine(std::vector<Line*>& l, std::vector<float>& gg, const int newSeg)
{
	int lastInd = -1;
	float totalLen = 0.0f;
	for (int i = 0; i < l.size(); ++i)
	{
		int lineSegCount = l[i]->size() - 1;
		Line& line = *l[i];
		for (int j = 0; j < lineSegCount; ++j)
		{
			totalLen += glm::distance(line[j].pos, line[j + 1].pos);
		}
	}

	const float perSegLength = totalLen / newSeg;

	g.clear();
	g.push_back(gg[0]);
	int indIndex = -1;
	float w = 0.0f;
	for (int i = 0; i < l.size(); ++i)
	{

		int lineSegCount = l[i]->size()-1;
		Line& line = *l[i];
		float leftLen = perSegLength;
		float totalG = 0.0f;
		int segs = 0;
		for (int j = 0; j < lineSegCount; ++j,++indIndex)
		{
			float dis= glm::distance(line[j].pos, line[j + 1].pos);
			if (dis < leftLen)
			{
				leftLen -= dis;
				++segs;
			}
			else 
			{
				float d;
				if (segs == 0)
				{
					float now = leftLen;
					while (dis > now)
					{
						d = now / dis;
						g.push_back((1 - d)*gg[indIndex] + d * gg[indIndex + 1]);
						now += perSegLength;
						w += 1;
					}
					leftLen = now - dis;
				}
				else
				{
					d = leftLen / dis;
					g.push_back((1 - d)*gg[indIndex]+ d * gg[indIndex + 1]);
					leftLen = leftLen + perSegLength - dis;
				}
				segs = 0;
			}
			line[j].w = w;
			w += dis / perSegLength;
		}
		line[lineSegCount].w = w;
		++indIndex;
	}
#ifdef DEBUG
	std::cout << "Segments: " << newSeg << "\n";
	std::cout << "w= " << w << "\n";
	std::cout << "per length: " << perSegLength << "\n";
#endif // DEBUG
}

void Shape::LineStrip::updateMinMax(glm::vec3 pos)
{
	min.x = pos.x < min.x ? pos.x : min.x;
	min.y = pos.y < min.y ? pos.y : min.y;
	min.z = pos.z < min.z ? pos.z : min.z;

	max.x = pos.x > max.x ? pos.x : max.x;
	max.y = pos.y > max.y ? pos.y : max.y;
	max.z = pos.z > max.z ? pos.z : max.z;
}
