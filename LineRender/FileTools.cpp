#include "FileTools.h"
#include <fstream>

char * FileTool::readWholeFile(const char * fileName, int & length)
{
#ifdef DEBUG
	std::cout << "open file:" << fileName << std::endl;
#endif // DEBUG

	std::ifstream in;
	in.open(fileName,std::ios::in);
	unsigned int len=0;
	in.seekg(0, std::ios::end);
	len += in.tellg();
	length = len;
	len += 1;
	if (len <= 1)
	{
		fprintf(stderr, "Error: file length is negative or zero. File Path:%s\n", fileName);
		system("pause"); exit(1);
	}
	char* result = new char[len];
	memset(result, 0, sizeof(char)*len);
	in.seekg(0,std::ios::beg);
	in.read(result, len - 1);
	return result;
}
