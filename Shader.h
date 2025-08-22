#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;

	// Reads vertex and fragment source codes, 
	// creates and compiles shader objects for them, 
	// and links them into a shader program
	Shader(const char* vertexPath, const char* fragmentPath);

	// Execute this shader program as current program in rendering state
	void use();

	// Utility functions to set 'uniforms' in the shader program
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
};

#endif