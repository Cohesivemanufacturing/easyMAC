// 3D-texture fragment shader.
// The fragment shader algorithm geneterates the intructions to raster the polygon 
// according to a bit map command or the the 2D texture image

#version 120

varying vec2 texCoord0;    // a shared variable to attribute texCoord form the VS to the FS
varying vec3 normal0; 

uniform sampler2D diffuse; // do linear interpolation  of the texture values in the polygone

void main()
{
	// vec3(0, 0, 1) is the direction of light (light comming out of the camara)
	gl_FragColor = texture2D(diffuse, texCoord0)* clamp(dot(-vec3(0, 0, 1), normal0), 0.25, 1);  
}