// Basic vertex shader.
// algorithm to process the input data into points that are readable by the GPU
// VS: Vertex shader.
// 
// The homogene transformations in the shader use projective coordinates to amplify or attenuate 
// the translation component the trasnformation 


#version 120

attribute vec3 position; // attrubute matched to the mesh buffer 0 in shader.cpp
attribute vec3 normal;   // attrubute matched to the mesh buffer 1 in shader.cpp


varying vec3 normal0;    // a shared variable to attribute normal form the VS to the FS

uniform mat4 transform;  // this uniform variable can be set by the CPU (my actual program)
 
void main()
{
	gl_Position  = transform * vec4(position, 1.0);     // coudl be just = vec4(1.0, 0.0, 0.0, 1.0)
	normal0      = (transform * vec4(normal, 0.0)).xyz; // and the normal (no translation is done to the normal) 
}
