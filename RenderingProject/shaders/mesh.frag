#version 430 core
uniform float yellow;
uniform float blue;
uniform float beta;
uniform float alpha;
uniform float difBlue;
uniform float difGreen;
uniform float difRed;
uniform float gooch;
uniform float silR;
uniform float reflectCheck;

//uniform vec3 mainDiffuse;
uniform sampler2D image;

out vec4 color;
in vec3 N;
in vec3 L;
in vec3 V;
in vec2 UV;

void main(void)
{    	
	float reflectShade = reflectCheck;
	float goochOnOff = gooch;
	vec2 uv = UV;
	float r = difRed;
	float g = difGreen;
	float db = difBlue;
	
	float b = blue;
	float y = yellow;
	float be = beta;
	float alph = alpha;
	vec3 dif = vec3(r,g,db);
	
	
	vec3 R = reflect(-L, N);
	vec3 yellow = vec3(y,y,0.0);
	vec3 blue = vec3(0.0,0.0,b);
	
	vec3 diffuse = max(0.0, dot(N,L)) * dif;
	
	float e = 100.0;
	float specular = pow(max(dot(R, V), 0.0), e);
	
	/* Figuring out the cool to warm tone*/
	
	float cool = 1 - (1 + dot(L,N))/2;
	vec3 Kcool = (blue + (alph*diffuse));
	
	float warm = ((1 + dot(L,N))/2);
	vec3 Kwarm = yellow + (be*diffuse);
	
	Kwarm = Kwarm * warm;
	Kcool = Kcool * cool;
	vec4 Kcoolwarm = vec4(Kcool+Kwarm,1.0);
	
	
	/*Orientation based attribute mapping */
	
	float Tr = silR;
	
	vec3 Na = normalize(N);
	/* come back here later*/
	float D;
	if(reflectShade == 0.0)
		D = pow(abs(dot(N,V)), Tr); //used for Silhouette textures
	else if(reflectShade == 1.0)
		D = pow(abs(dot(R,V)), Tr); //used for Specular highlights
	
	
	
	float Lam = (dot(N,L));

	
	float t = clamp(D, 0.0f, 1.0f);
	float s = clamp(Lam, 0.001f, 1.0f);
	
	
	vec2 st = vec2(s,t);
	vec4 colorFromImage = texture2D(image,-st);  //not sure why but I inverted the st and I get the desired effect.
	
	
	//colorFromImage = vec4(1.0,1.0,1.0,1.0); // TODO hardcoded for now
	
    
    if(goochOnOff == 1.0) //gives the opition to include the gooch shading from assignment 1
    {
		colorFromImage.x += (specular + Kcoolwarm.x);
		colorFromImage.y += (specular + Kcoolwarm.y);
		colorFromImage.z += (specular + Kcoolwarm.z);
	}
    color = vec4(colorFromImage);
  
}
