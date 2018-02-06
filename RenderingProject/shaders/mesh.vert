#version 430 core

uniform mat4 modelview;
uniform mat4 projection;
uniform vec3 lightPosition;
uniform float bunnyOrDragon;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
//attributes in camera coordinates

out vec3 N;
out vec3 L;
out vec3 V;
out vec2 UV;

void main(void)
{

	UV = uv;
	vec4 lightCameraSpace = modelview * vec4(lightPosition, 1.0);
	lightCameraSpace /= lightCameraSpace.w;
	float whichImage = bunnyOrDragon;
	vec3 scaleBunny = vec3 (0.0, -0.12, 0.0);
	
	vec3 scaleDragon = vec3 (0.0,0.0,0.0);
	vec3 scaleBudda = vec3 (0.0,-0.1,0.0);
	vec3 scaleDolphins = vec3(-20.5814, 52.2077, 0.0);
	vec3 imageChoice = vec3(0.0,0.0,0.0);
	

	
    float scale; 
   
   /* Scale the choosen user model*/
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
    vec3 positionModelSpace = vertex + imageChoice;
    vec4 positionCameraSpace = modelview * vec4(positionModelSpace, 1.0);
	vec3 P = positionCameraSpace.xyz/positionCameraSpace.w; // position in camera coordinates normalized
	
	mat3 normalMatrix = mat3(transpose(inverse(modelview)));
    vec3 Norm = normalMatrix * normal;

	N = normalize(Norm);
	
	L = normalize(lightCameraSpace.xyz - P);
	V = normalize(-P);
    gl_Position = projection * positionCameraSpace;    //does the normalizing b .w in gl_position
}
