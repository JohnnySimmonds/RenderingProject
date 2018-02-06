#version 430 core
uniform sampler1D colormap;
uniform sampler2D image;
out vec4 color;

in vec3 N;
in vec3 P;
in vec3 L;

in vec2 UV;
void main (void)
{
	vec2 uv = UV;
    vec3 diffuse_albedo = vec3(1.0, 1.0, 1.0);
    float kd = 0.8;
    vec3 diffuse = kd*diffuse_albedo*max( 0.0, dot( N, normalize(L - P)));
    //color = vec4( diffuse, 1.0);
    
    
     vec4 C = texture(image,uv);
  
    color = C;
}
