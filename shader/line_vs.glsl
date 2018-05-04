#version 330

layout (location=0) in vec3 Position;

uniform mat4 gWVP;
uniform mat4 gWorld;

//out float importance;
void main()
{
	gl_Position = gWVP * vec4(Position,1.0f);
	//importance = 1.0f-distance(Position,vec3(0.0f,0.0f,0.0f))/100.0f;
}