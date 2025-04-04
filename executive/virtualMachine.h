#pragma once

/*
* ToDo: 
* - consider making the mambers of machine object (meshes, textures, shapeTransforms, jointTranforms...etc)
*   public, and program getters to get references to them 
* - implement the case rotary joint
*/
#include <winsock2.h>              // needed for Winsocket.h
#include <iostream>                // std::cout
#include <string>                  // std::string, std::to_string
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <GL/glew.h>

#include "display.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include "transform.h"
#include "camara.h"

#include "buffer.h"                  // buffer for thread communcations
#include "h_variables.h"             // h_variables type and defs  
#include "machine_vector.h"          // machine_vector type and defs
#include "modal_vector.h"            // modal_vector type, defs and methods
#include "interpolator_parameters.h" // interpolator communication structures
#include "Winsocket.h"               // Winsocket class 

#include "ConsoleLogger.h"

// Definitions
#define BLU 0.000005
     
/**************************************************************
Atomic Objects
***************************************************************/

class Joint
{
public:
	unsigned int jointIndex;
	unsigned int link1Index;
	unsigned int link2Index;

	int jointPos = 0;    //  parameter controlling the position of the joint in BLUs
	int JointDir = 1;    //  parameter controlling the direction of the joint (-1, 1) 
	int positionBitValue;
	int directionBitValue;

	Joint()
	{

	}

	inline glm::mat4 GetJointTransform() const
	{
		return glm::translate(glm::vec3(0, 0, (float) BLU * jointPos));  
	}

protected:
private:
};

class Body
{
public:
	unsigned int bodyIndex;

	unsigned int meshIndex;
	unsigned int shapeTransfIndex;
	unsigned int textureIndex;

	Mesh* mesh;
	glm::mat4* shapeTrasnf;
	Texture* texture;

	Body()
	{

	}

protected:
private:
};

class Component 
{
public:
	unsigned int componentIndex;
	
	std::vector <Body> bodies;
	
	Component()
	{

	}

protected:
private:
};

typedef struct JointConstraint
{
	unsigned int jointIndex;
	unsigned int shapeTransfIndex;

	glm::mat4* shapeTrasnfPtr;
};

typedef struct BodyConstraints
{
	unsigned int bodyConstraintsIndex;
	std::vector <JointConstraint> jointConstraints;
};

typedef struct TransformIndices
{
	unsigned int shapeTransform1;
	unsigned int Joint;
	unsigned int shapeTransform2;
};

class Link
{
public:
	
	unsigned int linkIndex;
	unsigned int baseFrameIndex;

	glm::mat4* baseFrame;

	std::vector <Body> bodies;
	std::vector <JointConstraint>  jointConstraints;
	std::vector <TransformIndices> kinematicTransforms;

	Link()
	{

	}

	/**************************************************************
	getters
	***************************************************************/
	inline unsigned int GetShapeTransform(unsigned int index) 
	{
		for (std::vector<JointConstraint>::iterator it = jointConstraints.begin(); it != jointConstraints.end(); ++it)
		{
			if (it->jointIndex == index)
		    
			return it->shapeTransfIndex;
		}	
	}

protected:
private:
};

typedef struct StepDirections
{
	Joint* stepx;
	Joint* stepy;
	Joint* stepz;
};

/**************************************************************
Machine Object
***************************************************************/

class VirtualMachine
{
public:

	/**************************************************************
	virtual machine public members
	***************************************************************/
	volatile Buffer* buffer;									// a circular buffer for the messages sent to the virtualMachine

	/**************************************************************
	Virtual Machine services
	***************************************************************/
	VirtualMachine(const std::string& fileName, Display* display, Shader* shader, Camara* camara);
	VirtualMachine(Winsocket *socket);
	void ProcessCommand(char* block);
	void Render(void);
	axis getInterpolatorPosition(void);
	axis VirtualMachine::getInterpolatorNativePosition(void);

	/**************************************************************
	getters
	***************************************************************/
	inline bool isEnabled() { return m_isEnabled; }  
	inline bool isInterpolating() { return m_isInterpolating; }
	inline int  numberOfLinks() { return m_numlinks; }
	inline bool hasTexture() { return m_hasTexture; }
	
	/**************************************************************
	setters
	***************************************************************/
	inline void setSocket(Winsocket *socket) {
		this->cloudNCsocket = socket;
	}
	// executive structures
	inline void setHvariables(H_variables *var) { h_variables = var; }
	inline void setMachineVector(Machine_vector *vector) { machine_vector = vector; }
	inline void setConsole(CConsoleLogger *console) { vmConsole = console;}

	// state variables
	inline void enable()  { m_isEnabled = true; }
	inline void disable() { m_isEnabled = false; }

	// public virtual distructor
	virtual ~VirtualMachine();

protected:

private:

	VirtualMachine(const VirtualMachine& other) {}             // copy constructor 
	VirtualMachine& operator=(const VirtualMachine& other) {}  // The assignment operator 

	/**************************************************************
	Machine private Services
	***************************************************************/
	
	// Initialization
	void InitVirtualMechanism(const std::string& fileName);
	void CreateMesh(const std::string& line);
	void CreateTexture(const std::string& line);
	void CreateComponent(const std::string& line);
	void CreateBodyConstraints (const std::string& line);
	void CreateJoint(const std::string& line);
	void calcTransforms();
	
	// mdf parsing
	Body ParseBody(const std::string& token, bool hasTexture);
	JointConstraint ParseJointConstraint(const std::string& token);
	Joint ParseJoint(std::vector<std::string>& token);
	glm::mat4 ParseTransf(std::ifstream& file, std::string& line);

	// interpolation
	InterpolatorParameters LoadInterpolator(char* block);
	StepDirections setStepDirections(InterpolatorParameters& parameters);
	void Interpolate(InterpolatorParameters &parameters, StepDirections &stepDir);

	// machine vector
	float get_machine_BLU(void);

	void Ready();

	/**************************************************************
	Machine private members
	***************************************************************/
	
	// mdf objetcs
	std::vector <Mesh*>			  meshes;                // vector of pointers to the memory-allocated meshes
	std::vector <Texture*>		  textures;              // vector of pointers to the memory-allocated textures
	std::vector <glm::mat4>		  shapeTransforms;       // vector with the shape transforms of the mechanism
	std::vector <Body>			  bodies;                // vector with the bodies (mechanism rep. of a mesh with texture) of the mechanism
	std::vector <Component>		  components;            // vector with the components (colections of bodies) of the mechanism
	std::vector <JointConstraint> jointConstraints;      // vector with the join constraints (indexed joint and shape transforms) of the mechanism
	std::vector <BodyConstraints> mechanismConstraints;  // vector with the body contraints (joint constraints of each body) of the mechanism
	std::vector <Joint>			  joints;                // vector of joints of the mechanims
	std::vector <Link>			  links;                 // vector of links of the mechanims
	Link						  rootLink;              // root link of the mechanism
	glm::mat4*					  rootTransform;         // root transform of the mechanism

	// openGL rendering 
	Display *m_display;                                  // object with display functions(clear, Update..etc) for the SDL and openGL enviroments
	Shader  *m_shader;									 // object with vertex and fragment shader functions
	Camara  *m_camara;                                   // object controlling the openGL camara (world) perspective
	
	// executive structures
	H_variables     *h_variables;	                     // H variables for configuration      
	Machine_vector  *machine_vector;                     // machine state vector

	// communications
	CConsoleLogger *vmConsole;                           // a console logger for the Virtual Machine
	Winsocket  *cloudNCsocket;                           // winsocket to stream out post-processed data 

    // state variables
	volatile bool m_isEnabled = true;                    // this value changes between threads            
	volatile bool m_isInterpolating = false;                 
	
	// interpolator's private members
	int  m_numlinks = 0;                                 // 
	bool m_hasTexture = false;                           //
	bool m_isSDL = false;                                // 
	bool m_isCloudNC = false;                            //
	int  m_M = 12;										 // register size of the interpolator feed DDA
	int  m_N = 16;										 // register size of the rendering DDA 
	int  m_fmax = pow(2, m_M);							 // overflowing value of the velocity register
	int  m_rmax;							             // overflowing value of the rendering register
	int  m_actTol = 10;								     // interpolation arc tolerace
};


