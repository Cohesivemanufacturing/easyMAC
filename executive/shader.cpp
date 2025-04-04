//#include "stdafx.h"
#include "shader.h"
#include <iostream>
#include <fstream> //for loading files

static std::string LoadShader(const std::string& fileName);
static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
static GLuint CreateShader(const std::string& text, GLenum shaderType);

Shader::Shader(const std::string& fileName)
{
	m_program    = glCreateProgram();											   // creates a new shader program and returns a handle to refer with by.
	m_shaders[0] = CreateShader(LoadShader(fileName + ".vs"), GL_VERTEX_SHADER);   // creates a vertex shader (proecessing algorithm to create the vetices) from a file 
	m_shaders[1] = CreateShader(LoadShader(fileName + ".fs"), GL_FRAGMENT_SHADER); // creates a fragment shader (proecessing algorithm to generate pixel colors) from a file 

	for (unsigned int i = 0; i < NUM_SHADERS; i++)  // attached the shaders to the program
		glAttachShader(m_program, m_shaders[i]);

	glBindAttribLocation(m_program, 0, "position"); // finds the attribute "position" in the shader and maps it to attribute 0  in the mesh.     
	glBindAttribLocation(m_program, 1, "texCoord"); // finds the attribute "texCoord" in the shader and maps it to attribute 1  in the mesh. 
	glBindAttribLocation(m_program, 2, "normal");   // finds the attribute "normal" in the shader and maps it to attribute 2  in the mesh. 

	glLinkProgram(m_program);						// links the shader programs compiled objet to our program's objects
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Error: Shader Program linking failed: ");

	glValidateProgram(m_program);					// validates the program
	CheckShaderError(m_program, GL_VALIDATE_STATUS, true, "Error: Program is invalid: ");

	m_uniforms[TRANSFORM_U] = glGetUniformLocation(m_program, "transform"); // access to the transform uniform variable in the shader,
}


Shader::~Shader()
{
	for (unsigned int i = 0; i < NUM_SHADERS; i++)  // attached the shaders to the program
	{
		glDetachShader(m_program, m_shaders[i]);
		glDeleteShader(m_shaders[i]);
	}
		
	glDeleteProgram(m_program);
}

/*
 * Loads a text file from the hard drive.
 */

void Shader::Bind()
{
	glUseProgram(m_program);
}

//void Shader::Update(const Transform& transform, const Camara& camara)
void Shader::Update(const glm::mat4& model)
{
	
	//glm::mat4 model = camara.GetViewProjection() * transform.GetModel(); 

	glUniformMatrix4fv(m_uniforms[TRANSFORM_U], // the handle uniform
					   1,                        // passing 1 argument
					   GL_FALSE,                 // no need to transpose 
					   &model[0][0]);
}

static GLuint CreateShader(const std::string& text, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	
	if (shader == 0)
		std::cerr << "Error Shader creation failed!" << std::endl;
	
	// openGL is a C API, convet text to C string
	const GLchar* shaderSourceStrings[1];  
	GLint shaderSourceStringLengths[1];  
	
	shaderSourceStrings[0] = text.c_str();
	shaderSourceStringLengths[0] = text.length();

	glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringLengths);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader compilation failed: ");

	return shader;
}

static std::string LoadShader(const std::string& fileName)
{
	std::ifstream file;
	file.open((fileName).c_str());

	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		std::cerr << "Unable to load shader: " << fileName << std::endl;
	}

	return output;
}

/*
 * find error messages for shaders.
 */

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << errorMessage << ": '" << error << "'" << std::endl;
	}
}

