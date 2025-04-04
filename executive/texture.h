#pragma once
#include <iostream>
#include <string>
#include <GL/glew.h>

class Texture
{
public:
	Texture(const std::string& fileName);
	Texture(const Texture& other) {}  //  copy constructor for a class/struct that is used to make a copy of an existing instance.

	void Bind(unsigned int unit);     // a way to distinguish which texture to bind..1, 2 etc

	virtual ~Texture();
protected:
private:
	Texture& operator=(const Texture& other) {}  // The assignment operator for a class is what allows you to use = to assign one instance to another

	GLuint m_texture;
};
