// 2D-texture vertex shader.
// algorithm to process the input data into points that are readable by the GPU
// VS: Vertex shader.
// 
// The homogene transformations in the shader use projective coordinates to amplify or attenuate 
// the translation component the trasnformation 


#version 120

attribute vec3 position; // attrubute matched to the mesh buffer 0 in shader.cpp
attribute vec2 texCoord; // attrubute matched to the mesh buffer 1 in shader.cpp
attribute vec3 normal;

varying vec2 texCoord0;  // a shared variable to attribute texCoord form the VS to the FS
varying vec3 normal0; 

void main()
{
	gl_Position = vec4(position, 1.0);     // coudl be just = vec4(1.0, 0.0, 0.0, 1.0)
	texCoord0   = texCoord;                // now the fragment shader has access to the texcoord
	normal0     = normal;
}
