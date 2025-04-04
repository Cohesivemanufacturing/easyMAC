// 2D-texture fragment shader.
// The fragment shader algorithm geneterates the intructions to raster the polygon 
// according to a bit map command or the the 2D texture image

#version 120

varying vec2 texCoord0;    // a shared variable to attribute texCoord form the VS to the FS
varying vec3 normal0;

uniform sampler2D diffuse; // do linear interpolation  of the texture values in the polygone

void main()
{
	gl_FragColor = texture2D(diffuse, texCoord0);  
}