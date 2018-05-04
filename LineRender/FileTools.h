#pragma once
namespace FileTool
{
	//return the content of file, fileName:which file to read, length: return the length of content
	char* readWholeFile(const char* fileName, int& length);
}