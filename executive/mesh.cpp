//#include "stdafx.h"
#include "mesh.h"
#include "obj_loader.h"
#include <vector>
#include <fstream>
#include <iostream>

Mesh::Mesh(const std::string& fileName)
{
	IndexedModel model = OBJModel(fileName).ToIndexedModel();

	m_GLmode = GL_TRIANGLES;

	InitMesh(model);
}

Mesh::Mesh(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices, int GL_mode)
{
	IndexedModel model;

	for (unsigned int i = 0; i < numVertices; i++)
	{
		model.positions.push_back(*vertices[i].GetPos());
		model.texCoords.push_back(*vertices[i].GetTexCoord());
		model.normals.push_back(*vertices[i].GetNormal());
	}

	for (unsigned int i = 0; i < numIndices; i++)
		model.indices.push_back(indices[i]);

	m_GLmode = GL_mode; // GL_triangles or GL_lines

	InitMesh(model);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void Mesh::InitMesh(const IndexedModel& model)
{
	m_drawCount = model.indices.size();

	glGenVertexArrays(1, &m_vertexArrayObject);		  // creates space for a vertex array in the  m_vertexArreyObject (a handle)
	glBindVertexArray(m_vertexArrayObject);			  // from this point, any operation that affect a vertex arrey object will affect this particular object

	// creates an array of buffers and puts the vertices data on them 
	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);  // gives us blocks of data in the GPU for the buffers we want to 
													  // generate and associates to them to the handle m_vertexArreyBuffers

	/**************************************************************
	positions Buffer
	***************************************************************/
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]); // the buffer is an array 
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW);  // put the vertices into the buffer (sends them to the GPU)

	/*
	* organize the vertex arrays by their attributes , e.g an array buffer
	* for the positions and another one for the colors (we only have one attrib. position)
	*/
	glEnableVertexAttribArray(0);  // only one attribute
								   // how to read each attribute in the array 
	glVertexAttribPointer(0,       // attribute array 0
		3,       // 3D vector 
		GL_FLOAT, // floats 
		GL_FALSE, // normalization parameter
		0,        // do not skip data between atributes (our data is a continous attribute array)
		0);       // do not skip data at the begining      

/**************************************************************
textures Buffer
***************************************************************/
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.texCoords[0]), &model.texCoords[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	/**************************************************************
	normals Buffer
	***************************************************************/
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(model.normals[0]), &model.normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/**************************************************************
	indeces Buffer
	***************************************************************/
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]); // an array that references the data in some other array
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STATIC_DRAW);

	if (m_GLmode == GL_LINE)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glBindVertexArray(0);
}

void Mesh::Draw()
{
	glBindVertexArray(m_vertexArrayObject);	    // from this point, any operation that affect a vertex arrey object will affect this particular object

	glDrawElements(m_GLmode, // triangle mode
		m_drawCount,         // num elements to draw
		GL_UNSIGNED_INT,     // indices are unsigned int 
		0);                  // start from the begining       

	glBindVertexArray(0); // end binding

}