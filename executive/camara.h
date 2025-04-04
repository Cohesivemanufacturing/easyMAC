#pragma once

#include <glm.hpp>
#include <gtx/transform.hpp>
#include "display.h"

class Camara
{
public:
	Camara(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar) 
	{
		m_perspective = glm::perspective(fov, aspect, zNear, zFar);
		m_position    = pos;
		// migth want to program getters and setters to change the camara rortation
		m_forward	  = glm::vec3(0, 0, -1);
		m_up	      = glm::vec3(0, 1, 0);
	}

	/**************************************************************
	getters
	***************************************************************/
	
	inline glm::mat4 GetViewProjection() const
	{
		return m_perspective * glm::lookAt(m_position, m_position + m_forward, m_up); // a view projection
	}

	/**************************************************************
	setters
	***************************************************************/

protected:
private:
	glm::mat4 m_perspective;
	glm::vec3 m_position;
	// camara rotation
	glm::vec3 m_forward;    // forward direction from the way we are looking 
	glm::vec3 m_up;         // direction you percieve is up
};

