#include "Line.h"
/*
void Shape::LineStrip::Init(
	Line & _line, 
	float _width, 
	glm::vec3 _baseDir, 
	glm::vec4 _color)
{
	breakLine = false;
	//提前计算，不使用曲面shader
	std::vector<LineVertices> d;
	std::vector<unsigned int> indices;
	glm::vec3 temp,pre,linev;
	float w = -0.5f;
	size = _line.size();
	d.reserve(size<<1);
	indices.reserve(size << 1);

	pre =glm::normalize(glm::cross(_baseDir,_line[1].pos-_line[0].pos));

	for (int i = 0; i < size-1; ++i)
	{
		linev = _line[i + 1].pos - _line[i].pos;
		temp = (glm::normalize(glm::cross(_baseDir,linev))+pre)*0.5f;
		linev = glm::normalize(glm::cross(linev, temp));
		d.push_back({ _line[i].pos + temp * (_width*0.5f),_line[i].color, w+=1.0f, linev});
		d.push_back({_line[i].pos - temp*(_width*0.5f), _line[i].color, w,linev});
		indices.push_back((i << 1));
		indices.push_back((i << 1)+1);
		pre = temp;
	}

	linev = _line[size - 1].pos - _line[size - 2].pos;
	temp= glm::normalize(glm::cross(_baseDir, linev));
	linev = glm::normalize(glm::cross(linev, temp));
	d.push_back({ _line[size - 1].pos + temp * (_width*0.5f),_line[size-1].color, w += 1.0f ,linev});
	d.push_back({ _line[size - 1].pos - temp * (_width*0.5f),_line[size - 1].color, w += 1.0f,linev });
	indices.push_back(((size-1) << 1));
	indices.push_back(((size-1) << 1) + 1);
	size <<= 1;
	indiceSize = size;
	BindData(sizeof(glm::vec3)*(size), d.data(),
		sizeof(unsigned int)*(size), indices.data());

}*/

void Shape::LineStrip::Init(
	const std::vector<LineBasicData*> lines, 
	float _width, glm::vec3 _baseDir, int totalSeg)
{
	//提前计算，不使用曲面shader

	std::vector<LineVertices> d;
	std::vector<unsigned int> indices;
	glm::vec3 temp, pre, linev;
	float w = -0.5f;
	size = 0;
	indiceSize = 0;
	breakLine = false;
	segNum = totalSeg;
	gapRecord.clear();


	for (unsigned int i = 0; i < lines.size(); i++)
	{
		size += lines[i]->size();
	}
	size <<= 1;
	indiceSize = size + lines.size();
	d.reserve(size);
	indices.reserve(indiceSize);

	const float wStep = totalSeg / (float)size;
	int i;
	i = -1;
	for (unsigned int k = 0; k < lines.size(); k++)
	{
		LineBasicData& lptr = *lines[k];
		unsigned int jl = 0;

		pre = glm::normalize(glm::cross(_baseDir, lptr[1].pos - lptr[0].pos));

		for (jl = 0; jl < lptr.size() - 1; jl++)
		{
			++i;
			linev = lptr[jl + 1].pos - lptr[jl].pos;
			temp = (glm::normalize(glm::cross(_baseDir, linev)) + pre)*0.5f;
			linev = glm::normalize(glm::cross(linev, temp));
			d.push_back({ lptr[jl].pos + temp * (_width*0.5f),lptr[jl].color, w += wStep ,linev});
			d.push_back({ lptr[jl].pos - temp * (_width*0.5f), lptr[jl].color, w ,linev});
			indices.push_back((i << 1));
			indices.push_back((i << 1) + 1);
			pre = temp;
		}
		++i;
		linev = lptr[jl].pos - lptr[jl - 1].pos;
		temp = glm::normalize(glm::cross(_baseDir, lptr[jl].pos - lptr[jl - 1].pos));
		//segment's w set to xxx.5f
		w = (int)(w + wStep + 0.5f) + 0.5f;
		d.push_back({ lptr[jl].pos + temp * (_width*0.5f),lptr[jl].color, w,linev });
		d.push_back({ lptr[jl].pos - temp * (_width*0.5f),lptr[jl].color, w,linev });
		indices.push_back((i << 1));
		indices.push_back((i << 1) + 1);

		if (k != lines.size() - 1)
		{
			indices.push_back(-1);//断点
			gapRecord.push_back((int)w);
			breakLine = true;
		}
	}
	BindData(sizeof(glm::vec3)*(size), d.data(),
		sizeof(unsigned int)*(size), indices.data());
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

	glBindBuffer(GL_ARRAY_BUFFER,lineData);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertices), 0);//pos
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertices), (void*)sizeof(glm::vec3));//color
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(LineVertices), (void*)(sizeof(glm::vec3)*2));//w
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(LineVertices), (void*)(sizeof(glm::vec3)*2+sizeof(float)));//n
	
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
}

int Shape::LineStrip::GetSize() const
{
	return size;
}

void Shape::RandomGenerateLine(LineBasicData & line, 
	const glm::vec3 begin, 
	const float curve, 
	const unsigned int count)
{
	line.clear();
	line.reserve(count);
	line.push_back({ begin,glm::vec3(0.5f,0.5f,0.0f)});

	glm::vec3 a;
	glm::vec3 p(1.0f, 0.0f, 0.0f);
	glm::vec3 dir(RandFloat(), RandFloat(), RandFloat());
	dir = glm::normalize(dir)*0.05f;
	for (unsigned int i = 1; i < count; i++)
	{
		a = glm::normalize(cross(dir,p)+glm::normalize(glm::vec3(RandFloat() ,RandFloat() ,RandFloat()))*curve);
		dir += a;
		//colorful line // grey line
		line.push_back({ line[i-1].pos + dir ,/*dir*0.2f+*/glm::vec3(0.8f,0.8f,0.8f)});
	}
}

void Shape::RandomGenerateLineStra(LineBasicData & line, const glm::vec3 begin, const float curve, const unsigned int count)
{
	
	line.clear();
	line.reserve(count);
	
	line.push_back({ begin+150.0f,glm::vec3(0.5f,0.5f,0.0f) });

	glm::vec3 a,dis;
	glm::vec3 p(1.0f, 0.0f, 0.0f);
	glm::vec3 dir(RandFloat(), RandFloat(), RandFloat());
	dir = glm::normalize(dir)*10.0f;
	for (unsigned int i = 1; i < count; i++)
	{
		dis = begin - line[i - 1].pos+120.0f;
		float r = glm::length(dis)/10.0f;
		r = -1000.1f / (r*r+100.0f);
		a = glm::normalize(glm::normalize(-dis) + curve*glm::cross(glm::normalize(dis),glm::normalize(glm::vec3(RandFloat(), RandFloat(), RandFloat()))))*r;
		dir += a;
		//printf("%d\tdir=  %f\n", i,dir);
		//colorful line // grey line
		line.push_back({ line[i - 1].pos + dir ,dir*0.0f+2.0f*glm::vec3(0.5f,0.5f,0.5f) });
	}/*
	line.clear();
	line.reserve(count);
	line.push_back({ begin+100.0f,glm::vec3(0.5f,0.5f,0.0f) });

	glm::vec3 a;
	glm::vec3 p(1.0f, 0.0f, 0.0f);
	glm::vec3 dir(RandFloat(), RandFloat(), RandFloat());
	dir = glm::normalize(dir)*0.0001f;
	for (unsigned int i = 1; i < count; i++)
	{
		a = glm::normalize(cross(dir, p) + glm::normalize(glm::vec3(RandFloat(), RandFloat(), RandFloat()))*curve)*0.1f;
		dir += a;
		//colorful line // grey line
		line.push_back({ line[i - 1].pos + dir ,dir*0.2f+glm::vec3(0.8f,0.8f,0.8f) });
	}*/
}

void Shape::RandomGenerateLineStrip(
	LineStrip & ls, 
	const glm::vec3 dir, const float curve, 
	const unsigned int count, const unsigned int groupCount, const int segNum)
{
	std::vector<LineBasicData*> line;
	std::vector<float> g;

	line.reserve(groupCount);
	g.reserve(segNum);

	
	for (unsigned int i = 0; i < 1; ++i)
	{
		line.push_back(new LineBasicData());
		RandomGenerateLineStra(*(line.back()), dir, curve, count);
	}
	for (unsigned int i = 1; i < groupCount; ++i)
	{
		line.push_back(new LineBasicData());
		RandomGenerateLine(*(line.back()), 
			dir//+ glm::normalize(glm::vec3(RandFloat(), RandFloat(), RandFloat()))*50.0f
			, curve, count);
	}

	ls.Init(line, 10.0f, glm::vec3(0.0f, 0.5f, 0.5f), segNum);

	//float gChangeRate = 0.01f;
	//float gChangeDir = 1.0f;
	//float gCurrent = 0.0f;
	//const glm::vec3 pos(50.0f, 50.0f, 100.0f);
	/*
	const glm::vec3 pos(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < segNum; ++i)
	{
		
		float result=glm::distance(pos, line[i / count]->at(i%count).pos);
		if (result < 50.0f)
			g.push_back(0.99f);
		else
			if (result < 200.0f)
			g.push_back(0.5f);
			else
				g.push_back(0.1f);
	}*/
	
	for (unsigned int i = 0; i < segNum / groupCount*1; i++)
	{
		g.push_back(0.95f);
	}

	for (int i = segNum / groupCount* 1; i < segNum; i++)
	{
		g.push_back(0.05f);
	}
	/*
	int start = rand() % (segNum / 2 - 5);
	int end = start + 1000;
	if (end > segNum)end = segNum;
	for (int i = 0; i < start; ++i)
	{
		g.push_back(0.1f);
	}
	for (int i = start; i < end; ++i)
	{
		g.push_back(0.95f);
	}
	for (int i = end; i < segNum; ++i)
	{
		g.push_back(0.2f);
	}*/
	/*
	float gCurrent = 0.0f;
	float gChangeRate = 0.01f;
	for (int i = 0; i < segNum; ++i)
	{
		gCurrent += gChangeRate;
		if (gCurrent >= 0.999f || gCurrent <= 0.001f)
		{
			gChangeRate = -gChangeRate;
		}
		g.push_back(gCurrent);
	}*/
	/*
	for (int i = 0; i < segNum; ++i)
	{
		g.push_back(0.5f);
	}*/



	ls.g = g;

	for (unsigned int i = 0; i < groupCount; ++i)
	{
		delete line[i];
	}
	line.clear();
}
