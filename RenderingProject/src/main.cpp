
#include "GL/glew.h"
#include <cstdlib>
#include <cstdio>
#include <GLFW/glfw3.h>

#include "SceneShader.h"

double mouse_old_x, mouse_old_y, right_mouse_x, right_mouse_y;
int mouse_buttons = 0;
float rotate_x = 0.0, rotate_y = 0.0;
float translate_z = -1.0;
float imageChoice = 0;
float gooch = 0.0;
int choice = 0;
int figChoice = 0;
float reflect = 12.0;
int width = 1024;
int height = 1024;
GLFWwindow* window;
SceneShader shader;
SceneShader bunny;
SceneShader dragon;
SceneShader budda;
SceneShader cube;
bool wired = false;
float scale;
bool checkMesh = false;
void keyboard(unsigned char key, int x, int y, int action)
{
	float factor = 0.05f;
	float rInt = 0.01f;
	switch (key)
	{
	case (27):
						exit(EXIT_FAILURE);
	case (GLFW_KEY_X):
						rotate_y += 1.0f;
	break;
	case (GLFW_KEY_Z):
						rotate_y -=  1.0f;
	break;

	case (GLFW_KEY_A):
				shader.updateLightPositionX(-factor);
	break;
	case (GLFW_KEY_D):
				shader.updateLightPositionX(factor);
	break;
	case (GLFW_KEY_S):
				shader.updateLightPositionY(-factor);
		break;
	case (GLFW_KEY_W):
				shader.updateLightPositionY(factor);
	break;

	case (GLFW_KEY_Q):
				shader.updateLightPositionZ(-factor);
	break;
	case (GLFW_KEY_E):
				shader.updateLightPositionZ(factor);
		break;
	case (GLFW_KEY_Y):
			shader.updateYellow(factor);
		break;
	case (GLFW_KEY_U):
			shader.updateYellow(-factor);
		break;
	case (GLFW_KEY_I):
			shader.updateBeta(factor);
		break;
	case (GLFW_KEY_O):
			shader.updateBeta(-factor);
		break;
	case (GLFW_KEY_B):
			shader.updateBlue(factor);
		break;
	case (GLFW_KEY_N):
			shader.updateBlue(-factor);
		break;
	case (GLFW_KEY_K):
			shader.updateAlpha(factor);
		break;
	case (GLFW_KEY_L):
			shader.updateAlpha(-factor);
		break;
	case (GLFW_KEY_1):
			shader.updateDifRed(factor);
		break;
	case (GLFW_KEY_2):
			shader.updateDifRed(-factor);
		break;
	case (GLFW_KEY_3):
			shader.updateDifGreen(factor);
		break;
	case (GLFW_KEY_4):
			shader.updateDifGreen(-factor);
		break;
	case (GLFW_KEY_5):
			shader.updateDifBlue(factor);
		break;
	case (GLFW_KEY_6):
			shader.updateDifBlue(-factor);
		break;
	case (GLFW_KEY_G):
			shader.updateArtist(true);
		break;
	case(GLFW_KEY_H):
	{
		if(action == GLFW_RELEASE)
		{
			if(!checkMesh)
			{
				checkMesh = true;
				shader.updateMesh(checkMesh);	
			}
			else
			{
				checkMesh = false;
				shader.updateMesh(checkMesh);	
			}
		}
	}
		break;
	case (GLFW_KEY_F):
		{
			if(action == GLFW_RELEASE)
			{
				if(gooch == 0)
				{
					
					gooch = 1.0;
					shader.updateGooch(gooch);
				}
				else if(gooch == 1.0)
				{
					gooch = 0.0;
					shader.updateGooch(gooch);
				}
			}
		}
		break;
	case(GLFW_KEY_R):
		{
			if(action == GLFW_RELEASE)
			{
				if(choice != 1)
				{
					choice -= 1.0;
					if(choice == 2.0)
						shader = bunny;
					if(choice == 1.0)
						shader = dragon;
					if(choice == 3.0)
						shader = budda;
					
				}
			}
		}
		break;
		case(GLFW_KEY_T):
		{
			if(action == GLFW_RELEASE)
			{
				if(choice != 4)
				{
					choice += 1.0;
					if(choice == 2.0)
						shader = bunny;
					if(choice == 3.0)
						shader = budda;
					if(choice == 4.0)
						shader = cube;
					
				}
			}
		}
		break;
	/* keys for adjusting user input r asg2*/
	case(GLFW_KEY_7):
	{
		//if(action == GLFW_RELEASE)
			shader.updateSilR(-rInt);
	}
		break;
	case(GLFW_KEY_8):
	{
		//if(action == GLFW_RELEASE)
			shader.updateSilR(rInt);
	}
		break;
	case(GLFW_KEY_9):
	{
		if(action == GLFW_RELEASE)
		{
			if(wired == false)
			{
				wired = true;
				shader.updateWired(wired);
			}
			else
			{
				wired = false;
				shader.updateWired(wired);
			}
		}
	}
	break;
	default:
		break;
	}
	

}


void mouse( GLFWwindow* window, int button, int action, int mods )
{
	
	if (button == GLFW_MOUSE_BUTTON_2 )//&& action == GLFW_PRESS)
	{
		if(action == GLFW_PRESS)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			right_mouse_x = x;
			right_mouse_y = y;
			
			shader.updateIntCheck(true);
		}
		else if(action == GLFW_RELEASE)
		{
			shader.updateIntCheck(false);
		}
	}
	
}


void motion( GLFWwindow* w, double x, double y )
{

	double dx, dy;
	dx = (x - mouse_old_x);
	dy = (y - mouse_old_y);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
	{
		rotate_x += dy * 0.5f;
		rotate_y += dx * 0.5f;
	}
	else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
	{
		//right_mouse_x = x;
		//right_mouse_y = y;
			right_mouse_x += dx;
			right_mouse_y += dy;
		
	}

	mouse_old_x = x;
	mouse_old_y = y;
}

void scroll( GLFWwindow* w, double x, double y )
{
	double dy;
	dy = (x - y);
	translate_z += dy * 0.03f;

}

void render()
{
	GLfloat color[] = {0.3215f, 0.3411f, 0.4352f, 1.0f};
	const GLfloat zero = 1.0f;

	glClearBufferfv(GL_COLOR, 0, color);
	glClearBufferfv(GL_DEPTH, 0, &zero);
	glEnable(GL_DEPTH_TEST);
	

	shader.setRotationX(rotate_x);
	shader.setMousePos(right_mouse_x, right_mouse_y);
	shader.setRotationY(rotate_y);
	shader.setZTranslation(translate_z);
	shader.render();
	
	glDisable(GL_DEPTH_TEST);
}

void reshape( GLFWwindow* w, int widthX, int heightY )
{
	width = widthX;
	height = heightY;
	glViewport(0, 0, width, height);
}

static void error_callback( int error, const char* description )
{
	fputs(description, stderr);
}

static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	keyboard(key, 0, 0, action);

}

void startGlew(int choice, int figChoice, float reflect, int minAng, int maxAng)
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong */
		fprintf(stderr, "Error: %s \n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* vendor = glGetString(GL_VENDOR); // vendor name string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	// GLSL version string
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major); // get integer (only if gl version > 3)
	glGetIntegerv(GL_MINOR_VERSION, &minor); // get dot integer (only if gl version > 3)
	printf("OpenGL on %s %s\n", vendor, renderer);
	printf("OpenGL version supported %s\n", version);
	printf("GLSL version supported %s\n", glslVersion);
	printf("GL version major, minor: %i.%i\n", major, minor);

	
	shader.startup(choice, figChoice, reflect, minAng, maxAng, scale);
	//bunny.startup(2.0, figChoice, reflect, minAng, maxAng, scale);
	//budda.startup(3.0, figChoice, reflect, minAng, maxAng, scale);
	//dragon.startup(1.0, figChoice, reflect, minAng, maxAng, scale);
	//cube.startup(4.0, figChoice, reflect, minAng, maxAng, scale);
	
}

int main( int argc, char**argv )
{

	/* Menu for choice between dragon and bunny*/
	
	int maxAng = -1;
	int minAng = -1;
	
	while(choice < 1 || choice > 7)
	{
		printf("1. Dragon\n2. Bunny\n3. Budda\n4. Cube\n5. Dolphins\n6. Raindeer\n7. Open Cube\n");
		printf("Choice: ");
		scanf("%d", &choice);
	}
	switch(choice)
	{
		case 1:
			scale = 1.0;
		break;
		case 2:
			scale = 4.0;
		break;
		case 3:
			scale = 6.0;
		break;
		case 4:
			scale = 0.5;
		break;
		case 5:
			scale = 0.003;
		break;
		case 6:
			scale = 0.007;
		break;
		case 7:
			scale = 0.5;
		break;
	}
	
	
	while(figChoice < 1 || figChoice > 15)
	{
		printf("1. fig7b\n2. fig7c\n3. fig7d\n4. fig10b\n5. fig10c\n6. fig10d\n7. fig11b\n8. fig11c\n9. fig11d\n10. fig8a\n11. fig8b\n12. fig9bc\n13. fig9c\n14. fig9e\n15. fig9f\n");
		printf("Choice: ");
		scanf("%d", &figChoice);
	}
	
	while(reflect != 0.0 && reflect != 1.0)
	{
		printf("0. Silhouette backlighting \n1. Specular Highlights\n");
		printf("Choice: ");
		scanf("%f", &reflect);
	}
	
	while(minAng < 0 || maxAng > 180)
	{
		printf("Enter the minimum angle(degrees): \n");
		scanf("%d", &minAng);
		printf("Enter the maximum angle(degrees): \n");
		scanf("%d", &maxAng);
	}
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_SAMPLES, 16);
	
	window = glfwCreateWindow(width, height, "OpenGL Window", NULL, NULL);

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse);
	glfwSetCursorPosCallback(window, motion);
	glfwSetScrollCallback(window, scroll);
	glfwSetWindowSizeCallback(window, reshape);

	

	startGlew(choice, figChoice, reflect, minAng, maxAng);


	while ( !glfwWindowShouldClose(window) )
	{
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;
		shader.setAspectRatio(ratio);

		
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);


}
