#pragma once
#include <string>
#include <GL/glew.h>
#include "transform.h"
#include "camara.h"

class Shader
{
public:
	Shader(const std::string& fileName);

	void Bind();   // set the GPU to use the vertex and fragment shader of this class. 
	void Update(const glm::mat4& model); // update the uniforms in an array to some new set of values 

	virtual ~Shader();
protected:
private:
	static const unsigned int NUM_SHADERS = 2;
	Shader(const Shader& other){}
	Shader operator=(const Shader& other) {}

	// to update the shader to save a trans. matrix in the uniform variable
	enum
	{
		TRANSFORM_U,     // transform Uniform            

		NUM_UNIFORMS
	};

	GLuint m_program;			     // handle for openGL to keep track where the program is.
	GLuint m_shaders[NUM_SHADERS];   // OpenGL Type unisgned int array. Handle for the two types of shaders, the vertex and the fragment shaders
	GLuint m_uniforms[NUM_UNIFORMS]; // handle uniform variables in the shader
};

