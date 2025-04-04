//#include "stdafx.h"
#include "display.h"
#include <GL/glew.h>
#include <iostream>

Display::Display(int width, int height, const std::string& title)
{
	// because we are only using SDL for windowing purposes..
	SDL_Init(SDL_INIT_EVERYTHING);

	/**************************************************************
	Alocate space for display
	***************************************************************/

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);          // at least 256 different shades of RED colors
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);        // how much data in bits SLD will allocate for a single pixel (8X4)
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);         // allocate space for two windows(a non displayed window)

	m_window = SDL_CreateWindow(title.c_str(),          // SDL api is programmed on C, so convert the string to c
		SDL_WINDOWPOS_CENTERED, // creates window at the center of the display
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_OPENGL);     // don't use SDL stuff to draw in this window; do it with openGL
	m_glContext = SDL_GL_CreateContext(m_window);       // creates a conext to give the GPU control over the SDL window

	GLenum status = glewInit();

	if (status != GLEW_OK)
	{
		std::cout << "Glew failed to initialize!" << std::endl;
	}

	m_isClosed = false;

	glEnable(GL_DEPTH_TEST); // enables the use of our depth-buffer

	// dont draw tha face if it faces away from the camara 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void Display::Clear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Display::~Display()
{
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
	//std::cout << "destructur!" << std::endl;
}

void Display::GetEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)					   // type of event placed in the type member of event
		{
		case SDL_QUIT:
			m_isClosed = true;
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)      // SDL virtual key code (int)
			{
			case SDLK_ESCAPE:
				m_isClosed = true;
				break;
			case SDLK_UP:
				m_up_key = true;
				break;
			case SDLK_DOWN:
				m_down_key = true;
				break;
			case SDLK_RIGHT:
				m_right_key = true;
				break;
			case SDLK_LEFT:
				m_left_key = true;
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (e.key.keysym.sym)
			{
			case SDLK_UP:
				m_up_key = false;
				break;
			case SDLK_DOWN:
				m_down_key = false;
				break;
			case SDLK_RIGHT:
				m_right_key = false;
				break;
			case SDLK_LEFT:
				m_left_key = false;
				break;
			default:
				break;
			}
		default:
			break;
		}
	}
	
	// take action 
	if (m_up_key)
	{
		m_theta -= CAM_RES;
	}
	else if (m_down_key)
	{
		m_theta += CAM_RES;
	}
	if (m_right_key)
	{
		m_phi -= CAM_RES;
	}
	else if (m_left_key)
	{
		m_phi += CAM_RES;
	}
}

void Display::Update()
{
	SDL_GL_SwapWindow(m_window);
}