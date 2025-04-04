// Basic vertex shader.
// The fragment shader algorithm geneterates the intructions to raster the polygon 
// according to a bit map command or the the 2D texture image

#version 120

varying vec3 normal0; 

void main()
{
	// vec3(0, 0, 1) is the direction of light (light comming out of the camara)
	gl_FragColor = vec4(0.75, 0.75, 0.75, 1.0);  
}