#pragma once

#include <SDL.h>
#include <string>
#include <glm.hpp>
#include <gtx/transform.hpp>

# define CAM_RES 0.03
# define WIDTH  800
# define HEIGHT 600

class Display
{
public:
	Display(int width, int height, const std::string& title);
	void Clear(float r, float g, float b, float a);
	void Update();
	void GetEvents();

	/**************************************************************
	getters
	***************************************************************/
	inline bool  IsClosed() { return m_isClosed; }
	inline glm::mat4 GetUserCamara() const
	{
		
		glm::mat4 rotXMatrix = glm::rotate(m_theta, glm::vec3(1, 0, 0));
		glm::mat4 rotYMatrix = glm::rotate(m_phi, glm::vec3(0, 1, 0));

		return rotYMatrix * rotXMatrix;
	}
	
	/**************************************************************
	setters
	***************************************************************/

	virtual ~Display();
protected:
private:
	Display(const Display& other){}             // copy constructor for a class/struct that is used to make a copy of an existing instance.
	Display& operator=(const Display& other){}  // The assignment operator for a class is what allows you to use = to assign one instance to another

	SDL_Window* m_window;
	SDL_GLContext m_glContext;

	/**************************************************************
	SDL even handlers
	***************************************************************/
	/* variables to store user events */
	bool m_isClosed  = false;
	bool m_up_key    = false;
	bool m_down_key  = false;
	bool m_right_key = false;
	bool m_left_key  = false;

	/* initial angles for the camara*/
	float m_theta =  0.25f;
	float m_phi   =  0.0f;

	//float m_theta =  0.0f;
	//float m_phi   = -M_PI / 6.f;
};

