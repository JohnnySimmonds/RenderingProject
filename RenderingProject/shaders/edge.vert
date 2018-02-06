#version 430 core

uniform mat4 modelview;
uniform mat4 projection;
uniform float bunnyOrDragon;

layout (location = 0) in vec3 position;


void main (void)
{	

	float whichImage = bunnyOrDragon;
	vec3 scaleBunny = vec3 (0.0, -0.12, 0.0);
	vec3 scaleDragon = vec3 (0.0,0.0,0.0);
	vec3 scaleBudda = vec3 (0.0,-0.1,0.0);
	vec3 scaleDolphins = vec3(-20.5814, 52.2077, 0.0);
	vec3 imageChoice; 
	

   
   // Scale the choosen user model
	
	if(whichImage == 2.0)
	{
		imageChoice = scaleBunny;
	}
	else if (whichImage == 1.0)
	{
		imageChoice = scaleDragon;
	}
	else if(whichImage == 3.0)
	{
		imageChoice = scaleBudda;
	}
	else if (whichImage == 4.0)
	{
		imageChoice = vec3(0.0,0.0,0.0);
	}
	else if ( whichImage == 5.0)
	{
		imageChoice = scaleDolphins;
	}
	else if(whichImage == 6.0)
	{
		imageChoice = vec3(0.463005, -22.5763, 0);
	}
	else if (whichImage == 7.0)
	{
		imageChoice = vec3(0.0,0.0,0.0);
	}
    vec3 positionModelSpace = position + imageChoice;
    vec4 positionCameraSpace = modelview * vec4(positionModelSpace, 1.0);
	
    gl_Position = projection * positionCameraSpace;


}

