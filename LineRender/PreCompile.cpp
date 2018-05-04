#include "PreCompile.h"

void _checkOpenGLErrorInternal(const char* ch)
{

	bool wrong = false;
	for (GLenum err; (err = glGetError()) != GL_NO_ERROR;)
	{
		std::cout << "Err Code = " << err << "\t >" << gluErrorString(err) << std::endl;
		wrong = true;
		Sleep(1000);
	}
	if (wrong)std::cout << ch <<" > report complete\n";

}
