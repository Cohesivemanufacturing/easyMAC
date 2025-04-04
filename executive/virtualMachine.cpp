//#include "stdafx.h"

#include "virtualMachine.h"

/**************************************************************
Static functions outside the class
***************************************************************/

// function prototypes
static inline glm::vec4 ParseVec4(const std::string& line);
static inline std::string ParseFileName(const std::string& line);
static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token);
static inline float ParseFloatValue(const std::string& token, unsigned int start, unsigned int end);
static inline unsigned int ParseIndex(const std::string& token, unsigned int start, unsigned int end);
static inline std::vector<std::string> SplitString(const std::string &s, char delim);
static inline int parse_block(char* block, char code);
static inline int* parse_H_variable(char* block);

// function definitions
static inline  glm::vec4 ParseVec4(const std::string& line)
{
	unsigned int tokenLength = line.length();
	const char* tokenString = line.c_str(); // points to theentire array

	unsigned int vertIndexStart = 0;

	while (vertIndexStart < tokenLength)
	{
		if (tokenString[vertIndexStart] != ' ')
			break;
		vertIndexStart++;
	}

	unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float x = ParseFloatValue(line, vertIndexStart, vertIndexEnd);

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float y = ParseFloatValue(line, vertIndexStart, vertIndexEnd);

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float z = ParseFloatValue(line, vertIndexStart, vertIndexEnd);

	vertIndexStart = vertIndexEnd + 1;
	vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

	float w = ParseFloatValue(line, vertIndexStart, vertIndexEnd);

	return glm::vec4(x, y, z, w);

}

static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
{
	unsigned int result = start;
	while (result < length)
	{
		result++;
		if (str[result] == token)
			break;
	}

	return result;
}

static inline float ParseFloatValue(const std::string& token, unsigned int start, unsigned int end)
{
	return atof(token.substr(start, end - start).c_str());
}

static inline std::string ParseFileName(const std::string& line)
{
	unsigned int tokenLength = line.length();
	const char* tokenString = line.c_str(); // points to the entire array
	unsigned int strIndexstart = 2;

	while (strIndexstart < tokenLength)
	{
		if (tokenString[strIndexstart] != ' ')
			break;
		strIndexstart++;
	}

	unsigned int vertIndexEnd = FindNextChar(strIndexstart, tokenString, tokenLength, ' ');

	return line.substr(strIndexstart, vertIndexEnd - strIndexstart);
}

static inline unsigned int ParseIndex(const std::string& token, unsigned int start, unsigned int end)
{
	unsigned int result = atoi(token.substr(start, end - start).c_str());
	return result;
}

static inline std::vector<std::string> SplitString(const std::string &s, char delim)
{
	std::vector<std::string> elems;

	const char* cstr = s.c_str();
	unsigned int strLength = s.length();
	unsigned int start = 0;
	unsigned int end = 0;

	while (end <= strLength)
	{
		while (end <= strLength)
		{
			if (cstr[end] == delim)
				break;
			end++;
		}

		elems.push_back(s.substr(start, end - start));
		start = end + 1;
		end = start;
	}

	return elems;
}

static inline int parse_block(char* block, char code)
{
	const char* cstr = block;

	while (cstr != NULL && *cstr != '\n')
	{
		if (*cstr == code) {        // if the char associated with the pointer pnt is equal to the code
			return atoi(cstr + 1);  // convert the string pointed by  ptr + 1 into a float and returned
		}
		cstr++;
	}
	return 0;                      // otherwise returns cero.
}

static inline int* parse_H_variable(char* block) {
	static int temp[4] = { 0 };					    // All members of temp are init to zero
	temp[0] = parse_block(block, 'H');
	if (temp[0] != 0) {								// If we need to change any parameter
		if (temp[0] == 102) {							// Specific for changing origin
			temp[1] = parse_block(block, 'X');
			temp[2] = parse_block(block, 'Y');
			temp[3] = parse_block(block, 'Z');
			return temp;
		}
		temp[1] = parse_block(block, '=');			// Change other parameter different from origin
	}
	return temp;
}

/**************************************************************
Machine object
***************************************************************/
// virtual machine public services

VirtualMachine::VirtualMachine(Winsocket *socket)
{
	// bind windows socket for cloudNC
	this->cloudNCsocket = cloudNCsocket;

	// create machine joints
	Joint x = Joint();
	x.jointIndex = 0;
	x.positionBitValue = 1;
	x.directionBitValue = 2;
	this->joints.push_back(x);
	
	Joint y = Joint();
	y.jointIndex = 1;
	y.positionBitValue = 4;
	y.directionBitValue = 8;
	this->joints.push_back(y);
	
	Joint z = Joint();
	z.jointIndex = 2;
	z.positionBitValue = 16;
	z.directionBitValue = 32;
	this->joints.push_back(z);

	// default options for virtual machine constructor
	m_isSDL = false;
	m_isCloudNC = true;

	// initializes the circular buffers 
	buffer = new Buffer();
}

VirtualMachine::VirtualMachine(const std::string& fileName, Display* display, Shader* shader, Camara* camara)
{
	// initialize virtual machine
	InitVirtualMechanism(fileName);

	// bind openGL enviroment
	m_display = display;
	m_shader = shader;
	m_camara = camara;

	// default options for virtual machine constructor
	m_isSDL = true;
	m_isCloudNC = false;

	// initializes the circular buffers 
	buffer = new Buffer();
}

VirtualMachine::~VirtualMachine()
{
	// delete meshes
	for (std::vector<Mesh*>::iterator mesh_it = this->meshes.begin(); mesh_it != this->meshes.end(); ++mesh_it)
	{
		delete *mesh_it;
	}
	// delete textures
	for (std::vector<Texture*>::iterator texture_it = this->textures.begin(); texture_it != this->textures.end(); ++texture_it)
	{
		delete *texture_it;
	}
}

void VirtualMachine::ProcessCommand(char* block)
{
	int* in_data = parse_H_variable(block);

	switch (*in_data) {
	case 0: // MODE: process an instruction block

		InterpolatorParameters parameters = LoadInterpolator(block);
		StepDirections         stepDir = setStepDirections(parameters);

		Interpolate(parameters, stepDir);

		break;
	}

}

void VirtualMachine::Render()
{

	m_display->Clear(0.0f, 0.15f, 0.3f, 1.0f);

	m_shader->Bind();

	glm::mat4 T0 = *this->rootTransform;

	// iterate over the links
	for (std::vector<Link>::iterator link_it = this->links.begin(); link_it != this->links.end(); ++link_it)
	{
		glm::mat4 B = glm::mat4();

		// calculate the transformation at each link
		for (std::vector<TransformIndices>::iterator transf_it = link_it->kinematicTransforms.begin(); transf_it != link_it->kinematicTransforms.end(); ++transf_it)
		{
			glm::mat4 T1 = this->shapeTransforms[transf_it->shapeTransform1];     // prev. link shape transform
			glm::mat4 T2 = this->shapeTransforms[transf_it->shapeTransform2];     // new link shape transform
			glm::mat4 Joint = this->joints[transf_it->Joint].GetJointTransform(); // Joint Transform

																				  // this matrices are by post-Multiplication. e.i, each of the transforms it w.r.t to the previous.
																				  // the final product of this operation is the transform of link i  relative to the WCS
			B = B * T1 * Joint * inverse(T2);
		}

		// render each of the bodies in the links
		for (std::vector<Body>::iterator body_it = link_it->bodies.begin(); body_it != link_it->bodies.end(); ++body_it)
		{
			glm::mat4 Tb = *body_it->shapeTrasnf;

			// this matrices are pre-multiplied to the solids. each transf. is w.r.t the WCS starting with Tb all 
			// the way to the projection matrix.

			glm::mat4 model = m_camara->GetViewProjection() * m_display->GetUserCamara()* T0 * B * Tb;
			//glm::mat4 model = camara.GetViewProjection() * userCamara.GetModel() * Tb;

			// render
			body_it->texture->Bind(0);
			m_shader->Update(model);
			body_it->mesh->Draw();
		}
	}
	m_display->Update();
}

axis VirtualMachine:: getInterpolatorPosition() {
	/*
	* X = Xmachine - Xorigin - Xoffset or,
	* Xoffset = Xmachine - Xorigin - X
	*/

	float machine_BLU = get_machine_BLU();

	struct axis result;

	float Xorigin = 0;
	float Yorigin = 0;
	float Zorigin = 0;

	float Xoffset = h_variables->H[ORGIN_OFFSET_X];
	float Yoffset = h_variables->H[ORGIN_OFFSET_Y];
	float Zoffset = h_variables->H[ORGIN_OFFSET_Z];

	switch (machine_vector->coordinate_system)
	{
	case G54:
		Xorigin = h_variables->H[G54_ORIGIN_X];
		Yorigin = h_variables->H[G54_ORIGIN_Y];
		Zorigin = h_variables->H[G54_ORIGIN_Z];
		break;
	case G55:
		Xorigin = h_variables->H[G55_ORIGIN_X];
		Yorigin = h_variables->H[G55_ORIGIN_Y];
		Zorigin = h_variables->H[G55_ORIGIN_Z];
		break;
	case G56:
		Xorigin = h_variables->H[G56_ORIGIN_X];
		Yorigin = h_variables->H[G56_ORIGIN_Y];
		Zorigin = h_variables->H[G56_ORIGIN_Z];
		break;
	case G57:
		Xorigin = h_variables->H[G57_ORIGIN_X];
		Yorigin = h_variables->H[G57_ORIGIN_Y];
		Zorigin = h_variables->H[G57_ORIGIN_Z];
		break;
	case G58:
		Xorigin = h_variables->H[G58_ORIGIN_X];
		Yorigin = h_variables->H[G58_ORIGIN_Y];
		Zorigin = h_variables->H[G58_ORIGIN_Z];
		break;
	case G59:
		Xorigin = h_variables->H[G59_ORIGIN_X];
		Yorigin = h_variables->H[G59_ORIGIN_Y];
		Zorigin = h_variables->H[G59_ORIGIN_Z];
		break;
	}

	result.X = (this->joints[0].jointPos - Xorigin - Xoffset) / machine_BLU;
	result.Y = (this->joints[1].jointPos - Yorigin - Yoffset) / machine_BLU;
	result.Z = (this->joints[2].jointPos - Zorigin - Zoffset) / machine_BLU;
	return result;
}

axis VirtualMachine:: getInterpolatorNativePosition(void) {

	float machine_BLU = get_machine_BLU();
	
	struct axis result;

	result.X = this->joints[0].jointPos / machine_BLU;
	result.Y = this->joints[1].jointPos / machine_BLU;
	result.Z = this->joints[2].jointPos / machine_BLU;

	return result;
}

// virtual machine initialization services

void VirtualMachine::InitVirtualMechanism(const std::string& fileName) {
	// parse machine data file (.mdf)

	std::ifstream file;          // filebuf object 
	file.open(fileName.c_str());

	std::string line;
	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			line.erase(line.find_last_not_of(" \n\r\t") + 1); // trim the string

			unsigned int lineLength = line.length();

			const char* lineCStr = line.c_str(); // a C type string

			switch (lineCStr[0])
			{
			case 'm':
				CreateMesh(line);
				break;
			case 't':
				CreateTexture(line);
				break;
			case 's':
				if (lineCStr[1] == 't')
				{
					getline(file, line);
					this->shapeTransforms.push_back(ParseTransf(file, line));
				}
				break;
			case 'j':
				CreateJoint(line);
				break;
			case 'b':
				CreateComponent(line);
				break;
			case 'l':
				CreateBodyConstraints(line);
				break;
			default: break;
			};
		}
	}

	// create the mechanism links and root transform

	m_numlinks = components.size();
	this->links.reserve(m_numlinks);

	for (unsigned int i = 0; i < m_numlinks; i++)
	{
		Link link;

		link.linkIndex = i;

		link.bodies = this->components[i].bodies;

		link.baseFrameIndex = link.bodies[0].shapeTransfIndex;
		link.baseFrame = link.bodies[0].shapeTrasnf;

		link.jointConstraints = this->mechanismConstraints[i].jointConstraints;

		this->links.push_back(link);
	}

	this->rootLink = links[0];
	this->rootTransform = &shapeTransforms[0];

	// calculate the link kinematic transforms
	calcTransforms();
}

void VirtualMachine::CreateMesh(const std::string& line)
{
	Mesh* mptr;
	mptr = new Mesh("./res/virtualMachine/" + ParseFileName(line));
	this->meshes.push_back(mptr);
}

void VirtualMachine::CreateTexture(const std::string& line)
{
	Texture* tptr;
	tptr = new Texture("./res/virtualMachine/" + ParseFileName(line));
	this->textures.push_back(tptr);
}

void VirtualMachine::CreateComponent(const std::string& line)
{
	std::vector<std::string> tokens = SplitString(line, ' ');
	unsigned int numTokens = tokens.size();

	// crate a vector to store the bodies
	Component component;

	component.bodies.reserve(numTokens - 1);
	component.componentIndex = this->components.size();

	for (unsigned int i = 1; i < numTokens; i++)
	{
		Body body = ParseBody(tokens[i], m_hasTexture);

		component.bodies.push_back(body);

		this->bodies.push_back(body);
	}

	this->components.push_back(component);
}

void VirtualMachine::CreateBodyConstraints(const std::string& line)
{
	std::vector<std::string> tokens = SplitString(line, ' ');
	unsigned int numTokens = tokens.size();

	// create a new bodyConstraints object
	BodyConstraints bodyConstraint;

	bodyConstraint.jointConstraints.reserve(numTokens - 1);
	bodyConstraint.bodyConstraintsIndex = this->mechanismConstraints.size();

	// fill the bodyConstraints object with joint constraints 
	for (unsigned int i = 1; i < numTokens; i++)
	{
		JointConstraint jointconstraint = ParseJointConstraint(tokens[i]); // creates new jointConstraint

		bodyConstraint.jointConstraints.push_back(jointconstraint);        // stores the jointConstraint in the bodyConstraint object

		this->jointConstraints.push_back(jointconstraint);                 // also stores it in the mechansim object for easy accesibility

	}

	this->mechanismConstraints.push_back(bodyConstraint);                  // stores the bodyConstraint in the mechanism Object
}

void VirtualMachine::CreateJoint(const std::string& line)
{
	std::vector<std::string> tokens = SplitString(line, ' ');
	this->joints.push_back(ParseJoint(tokens));
}

// virtual machine mdf-parsing services 

glm::mat4 VirtualMachine::ParseTransf(std::ifstream& file, std::string& line)
{
	glm::vec4 x = ParseVec4(line);
	getline(file, line);
	glm::vec4 y = ParseVec4(line);
	getline(file, line);
	glm::vec4 z = ParseVec4(line);
	getline(file, line);
	glm::vec4 w = ParseVec4(line);

	return transpose(glm::mat4(x, y, z, w));
}

Body VirtualMachine::ParseBody(const std::string& token, bool hasTexture)
{
	unsigned int tokenLength = token.length();
	const char* tokenString = token.c_str();

	Body result;

	result.bodyIndex = this->bodies.size();

	/**************************************************************
	mesh
	***************************************************************/
	unsigned int indexStart = 0;
	unsigned int indexEnd = FindNextChar(indexStart, tokenString, tokenLength, '/');

	result.meshIndex = ParseIndex(token, indexStart, indexEnd) - 1;
	result.mesh = this->meshes[result.meshIndex];

	/**************************************************************
	shape transform
	***************************************************************/
	indexStart = indexEnd + 1;
	indexEnd = FindNextChar(indexStart, tokenString, tokenLength, '/');

	result.shapeTransfIndex = ParseIndex(token, indexStart, indexEnd) - 1;
	result.shapeTrasnf = &this->shapeTransforms[result.shapeTransfIndex];

	result.textureIndex = 0;
	result.texture = NULL;

	if (indexEnd >= tokenLength)
		return result;

	/**************************************************************
	texture
	***************************************************************/

	indexStart = indexEnd + 1;
	indexEnd = FindNextChar(indexStart, tokenString, tokenLength, '/');

	result.textureIndex = ParseIndex(token, indexStart, indexEnd) - 1;
	result.texture = this->textures[result.textureIndex];
	m_hasTexture = true;

	return result;
}

JointConstraint VirtualMachine::ParseJointConstraint(const std::string& token)
{
	unsigned int tokenLength = token.length();
	const char* tokenString = token.c_str();

	JointConstraint result;

	result.jointIndex = this->jointConstraints.size();

	/**************************************************************
	joint
	***************************************************************/
	unsigned int indexStart = 0;
	unsigned int indexEnd = FindNextChar(indexStart, tokenString, tokenLength, '/');

	result.jointIndex = ParseIndex(token, indexStart, indexEnd) - 1;

	/**************************************************************
	shape transform
	***************************************************************/
	indexStart = indexEnd + 1;
	indexEnd = FindNextChar(indexStart, tokenString, tokenLength, '/');

	result.shapeTransfIndex = ParseIndex(token, indexStart, indexEnd) - 1;
	result.shapeTrasnfPtr = &this->shapeTransforms[result.shapeTransfIndex];

	return result;
}

Joint VirtualMachine::ParseJoint(std::vector<std::string>& tokens)
{
	Joint result;

	result.jointIndex = this->joints.size();

	result.link1Index = atoi(tokens[1].c_str()) - 1;
	result.link2Index = atoi(tokens[2].c_str()) - 1;

	result.jointPos = 0;
	return result;
}

void VirtualMachine::calcTransforms()
{
	std::vector <unsigned int> visitedLinks;  // a vector to stored the indexes of rendered links
	std::vector <unsigned int> visitedJoints;

	std::vector <Joint*> adjJoints;           // a vector to store the next joints to spam

	Link* newLink = &this->rootLink;          // the first link to render 
	Link* prevLink;						      // the first link to render 

	// push the link into the rendered list
	visitedLinks.push_back(newLink->linkIndex);

	while (visitedLinks.size() != m_numlinks)
	{

		// Push adj. joints into joint list

		for (std::vector<JointConstraint>::iterator it = newLink->jointConstraints.begin(); it != newLink->jointConstraints.end(); ++it)
		{
			//  store a new joint in the list
			int jointIndex = it->jointIndex;

			if (std::find(visitedJoints.begin(), visitedJoints.end(), jointIndex) == visitedJoints.end()) // if the joint has not been visited
			{
				adjJoints.push_back(&this->joints[jointIndex]);
			}
		}

		// pop the first joint out of the joint list

		Joint* newjoint = adjJoints[0];
		adjJoints.erase(adjJoints.begin());

		// decide which of the adjacent link to the joint follows

		int link1Index = newjoint->link1Index;
		int link2Index = newjoint->link2Index;

		if (std::find(visitedLinks.begin(), visitedLinks.end(), link1Index) != visitedLinks.end())
		{
			prevLink = &this->links[link1Index];
			newLink = &this->links[link2Index];
		}
		else
		{
			prevLink = &this->links[link2Index];
			newLink = &this->links[link1Index];
		}

		// calculate the link transformation and store it
		// 1. push the previous link transform into the kinematicTransforms list of the link

		newLink->kinematicTransforms = prevLink->kinematicTransforms;

		// 2. calculate the new transform indices

		TransformIndices newTransform;
		newTransform.shapeTransform1 = prevLink->GetShapeTransform(newjoint->jointIndex);
		newTransform.shapeTransform2 = newLink->GetShapeTransform(newjoint->jointIndex);
		newTransform.Joint = newjoint->jointIndex;

		newLink->kinematicTransforms.push_back(newTransform);

		// push the link into the rendered list

		visitedJoints.push_back(newjoint->jointIndex);
		visitedLinks.push_back(newLink->linkIndex);
	}

}

// interpolation services
/*
* DDA-based, combined interpolation algorithm for circles and lines. The circular motion
* of all plains are generated by the X and Y DDAs while the function setStep directions
* set the output pulses to the correponding axes.
*/
void VirtualMachine::Interpolate(InterpolatorParameters &parameters, StepDirections &stepDir)
{
	// notify the virtualMachine start of interpolation
	this->m_isInterpolating = true;

	/* encoded bit */
	int encoded_bit;

	/* q registers DDAs*/
	int qx = 0;
	int qy = 0;
	int qz = 0;
	int qf = 0;

	int qr = 0;

	/* initialize output counters */
	int outx = 0;
	int outy = 0;
	int outz = 0;

	/* Circular and Linear DDA interpolation scheme */
	while (parameters.xtarget - outx > m_actTol || parameters.ytarget - outy > m_actTol || parameters.ztarget - outz > m_actTol)
	{
		/*interpolation ISR*/
		encoded_bit = 0;                                                // reset the encoded bit.
		qf = qf + parameters.pf;									    // update the q register of render DDA
		if (qf >= m_fmax) {
			qf = qf %  m_fmax;											// reset the q-resister
			qx = qx + parameters.px;									// update the q register of the x axis DDA
			qy = qy + parameters.py;								    // update the q register of the y axis DDA
			qz = qz + parameters.pz;
			if (qx >= parameters.steplim || qy >= parameters.steplim ||
				qz >= parameters.steplim) {								// if the q register of one of these DDAs overflowed
				if (qx >= parameters.steplim) {						    // if it was the x-register
					qx = qx % parameters.steplim;						// reset the q-resister
					if (parameters.CB) parameters.py--;				    // if circulat bit is on (inversion bit IB calc. outside the loop)
					if (parameters.xtarget > outx) {					// if target not reached 																
						stepDir.stepx->jointPos += stepDir.stepx->JointDir;  // take a step according to the inverison bit IB
						outx++;
						encoded_bit = encoded_bit | stepDir.stepx->positionBitValue;
						if (stepDir.stepx->JointDir == -1) encoded_bit = encoded_bit | stepDir.stepx->directionBitValue;
					}
				}
				if (qy >= parameters.steplim) {                         // if it was the x-register
					qy = qy % parameters.steplim;                       // do the same..
					if (parameters.CB) parameters.px++;
					if (parameters.ytarget > outy) {
						stepDir.stepy->jointPos += stepDir.stepy->JointDir;
						outy++;
						encoded_bit = encoded_bit | stepDir.stepy->positionBitValue;
						if (stepDir.stepy->JointDir == -1) encoded_bit = encoded_bit | stepDir.stepy->directionBitValue;
					}
				}
				if (qz >= parameters.steplim) {                         // if it was the z-register
					qz = qz % parameters.steplim;                       // do the same..
					if (parameters.ztarget > outz) {
						stepDir.stepz->jointPos += stepDir.stepz->JointDir;
						outz++;
						encoded_bit = encoded_bit | stepDir.stepz->positionBitValue;
						if (stepDir.stepz->JointDir == -1) encoded_bit = encoded_bit | stepDir.stepz->directionBitValue;
					}
				}
			}

		}
		// there is a lot we can do with the just-computed motion..
		if (m_isSDL) {
			qr++;
			if (qr >= m_rmax)
			{														// render after a number 2^N/pf of cycles of the DDA algorithm.  
				qr = qr % m_rmax;									// reset the velocty q-resister 
				m_display->GetEvents();
				Render();
			}
		}

		if (m_isCloudNC) 
		{
			// an oportunity to slow down the output stream
			qr++;
			if (qr >= m_rmax)
			{														
				qr = qr % m_rmax;		
				Sleep(20); 
				
			}
			
			// build the motion string word
			std::string word = std::to_string(encoded_bit) + "|";
			// plot to the console
			vmConsole->printf("%s", word.c_str());
		}
	}

	// notify the virtualMachine end of interpolation
	this->m_isInterpolating = false;
}

InterpolatorParameters VirtualMachine::LoadInterpolator(char* block)
{
	InterpolatorParameters result;

	result.CB = parse_block(block, 'C'); // circular bit (0, 1)
	result.IB = parse_block(block, 'I'); // interchange bit (0, 1)
	result.PB = parse_block(block, 'T'); // plain byte (1, 2, 3)
	result.x = parse_block(block, 'X'); // X inc. distance (blu)
	result.y = parse_block(block, 'Y'); // Y inc. distance (blu)
	result.z = parse_block(block, 'Z'); // Z inc. distance (blu)
	result.px = parse_block(block, 'P'); // X vel. register (int)
	result.py = parse_block(block, 'Q'); // Y vel. register (int)
	result.pz = parse_block(block, 'R'); // Z vel. register (int)
	result.pf = parse_block(block, 'F'); // Z vel. register (int)

	// initial targets
	result.xtarget = abs(result.x);
	result.ytarget = abs(result.y);
	result.ztarget = abs(result.z);

	// rendering-velocity register
	m_rmax = parse_block(block, 'F') * pow(2, m_M) / pow(2, m_N);

	// position register size
	result.steplim = sqrt(pow(result.xtarget, 2) + pow(result.ytarget, 2) + pow(result.ztarget, 2));

	return result;
}

StepDirections VirtualMachine::setStepDirections(InterpolatorParameters& parameters) {

	/* set the direction of the steps */

	StepDirections result = StepDirections();

	if (parameters.CB) {

		switch (parameters.PB)
		{
		case 1:  // XY plane 

			parameters.px = parameters.px;  // have been calculated on the executive. no need to invert
			parameters.py = parameters.py;

			if (parameters.IB)
			{
				parameters.xtarget = abs(parameters.y);
				parameters.ytarget = abs(parameters.x);

				result.stepx = &this->joints[1]; // Y
				result.stepy = &this->joints[0]; // X
			}
			else
			{
				result.stepx = &this->joints[0]; // X
				result.stepy = &this->joints[1]; // Y
			}
			break;

		case 2:

			parameters.py = parameters.px;
			parameters.px = parameters.pz; // map to the XY plane, no need to invert. 

			if (parameters.IB) {

				parameters.xtarget = abs(parameters.x);
				parameters.ytarget = abs(parameters.z);

				result.stepx = &this->joints[2]; // Z
				result.stepy = &this->joints[0]; // X
			}
			else {
				result.stepx = &this->joints[0]; // X
				result.stepy = &this->joints[2]; // Z
			}
			break;

		case 3:

			parameters.px = parameters.py; // map to the XY plane, no need to invert. 
			parameters.py = parameters.pz;

			if (parameters.IB) {

				parameters.xtarget = abs(parameters.z);
				parameters.ytarget = abs(parameters.y);

				result.stepx = &this->joints[2]; // Z
				result.stepy = &this->joints[1]; // Y
			}
			else {
				result.stepx = &this->joints[1]; // Y
				result.stepy = &this->joints[2]; // Z
			}
			break;
		}
	}

	else {
		result.stepx = &this->joints[0]; // X
		result.stepy = &this->joints[1]; // Y
		result.stepz = &this->joints[2]; // Y
	}

	parameters.x < 0 ? this->joints[0].JointDir = -1 : this->joints[0].JointDir = 1;
	parameters.y < 0 ? this->joints[1].JointDir = -1 : this->joints[1].JointDir = 1;
	parameters.z < 0 ? this->joints[2].JointDir = -1 : this->joints[2].JointDir = 1;

	return result;
}

float VirtualMachine::get_machine_BLU(void)
{
	if (machine_vector->units == MM) {
		return h_variables->H[BLU_MM];
	}
	else {
		return h_variables->H[BLU_IN];
	}
}

void VirtualMachine::Ready()
{
	// lets the program know that we are ready to process a new command


	// updates the tail pointer
	//m_countout = (m_countout + 1) % (MAX_BUF);

}




