#pragma once

#include <glm.hpp>
#include <GL/glew.h>
#include "obj_loader.h"

class Vertex  // could've been a struct
{
public:
	Vertex(const glm::vec3& pos, const glm::vec2& texCoord, const glm::vec3& normal = glm::vec3(0, 0, 0))
	{
		this->pos = pos;
		this->texCoord = texCoord;
		this->normal = normal;
	}

	inline glm::vec3* GetPos() { return &pos; }
	inline glm::vec2* GetTexCoord() { return &texCoord; }
	inline glm::vec3* GetNormal() { return &normal; }

protected:
private:
	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal;
}; 

class Mesh
{
public:
	Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices, int GL_Mode);
	Mesh(const std::string& fileName);
	
	void Draw(); //draws the mesh

	virtual ~Mesh();
protected:
private:
	Mesh(const Mesh& other) {}             // copy constructor for a class/struct that is used to make a copy of an existing instance.
	Mesh& operator=(const Mesh& other) {}  // The assignment operator for a class is what allows you to use = to assign one instance to another

	void InitMesh(const IndexedModel& model);

	enum
	{
		POSITION_VB,   // position Vertex Buffer                  
		TEXCOORD_VB,
		NORMAL_VB,

		INDEX_VB,      // vertex connectivity (topology)

		NUM_BUFFERS 
	};

	GLuint m_vertexArrayObject;                // refers to mesh data on the GPU
	GLuint m_vertexArrayBuffers[NUM_BUFFERS];  // a buffer is any block of data on the GPU
	unsigned int m_drawCount;                  // how many things were going to draw
	int m_GLmode;                              // GL_TRIANGLES or GL_LINES
};

