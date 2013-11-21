//standard libraries
#include <iostream>
#include <fstream>
using namespace std;
//opengl headers
#include <GL/glew.h>
#include <GL/freeglut.h>

//opengl mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

//functions for shader compilation and linking
#include "shaderhelper.h"
//object for drawing
#include "MyObject.h"

//struct for loading shaders
ShaderProgram shaderProgram;

//window size
int windowWidth = 800;
int windowHeight = 600;

//last mouse coordinates
int mouseX,mouseY;

//camera position
glm::vec3 eye(0.0, 0.0, 3.0);
//reference point position
glm::vec3 cen(0.0, 0.0, 0.0);
//up vector direction (head of observer)
glm::vec3 up(0.0, 1.0, 0.0);

//matrices
glm::mat4x4 modelViewMatrix;
glm::mat4x4 projectionMatrix;
glm::mat4x4 modelViewProjectionMatrix;
glm::mat4x4 normalMatrix;

///defines drawing mode
bool useTexture = true;
bool useMove = true; 

const int numPlanets = 4; 
const char * paths[] = {"C:\\task3\\src\\models\\sun.bmp", "C:\\task3\\src\\models\\mercury.bmp", "C:\\task3\\src\\models\\venus.bmp", "C:\\task3\\src\\models\\earth.bmp"};
GLubyte * imgData[numPlanets]; 
MyObject *planets;
float plantetsCenters[][3] = {{0, 0, -10}, {3, 0, -10}, {5, 0, -10}, {7, 0, -10}};
float radii[] = {2.0f, 1.0f, 1.0f, 1.0f};
float planetsSpeed[] = {0.00, 0.05, 0.05, 0.08};
float planetsRotationSpeed[] = {0.00, 0.05, 0.06, 1}; 

//texture identificator
GLuint *texId;

//names of shader files. program will search for them during execution
//don't forget place it near executable 
char VertexShaderName[] = "Vertex.vert";
char FragmentShaderName[] = "Fragment.frag";

////////////////////////////////////////////////////////
///
void initTexture()
{
	//enable texturing and zero slot
    glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE0);
	
	texId = new GLuint[numPlanets];
	//generate as many textures as you need
	glGenTextures(numPlanets, texId);
	
	const int width = 2048, height = 1024; 
	for (int i = 0; i < numPlanets; i++)
	{

		//make texId current 
		glBindTexture(GL_TEXTURE_2D, texId[i]);
	
		//don't use alignment
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	
		// Set nearest filtering mode for texture minification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		imgData[i] = new GLubyte[width * height * 3];  
		FILE * inp = fopen(paths[i], "rb"); 
		fread(imgData[i], 1, width * height * 3, inp); 
		for (int j = 0; j < width * height; j++)
		{
			swap(imgData[i][j * 3], imgData[i][j * 3 + 2]); 
		}
		fclose(inp); 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &imgData[i][0]);
	}
	

}

/////////////////////////////////////////////////////////////////////
///is called when program starts
void init()
{
	//enable depth test
	glEnable(GL_DEPTH_TEST);
	//initialize shader program
	shaderProgram.init(VertexShaderName,FragmentShaderName);
	//use this shader program
	glUseProgram(shaderProgram.programObject);

	planets = new MyObject[numPlanets];

	for (int i = 0; i < numPlanets; i++)
	{
		planets[i].fillData(plantetsCenters[i][0], plantetsCenters[i][1], plantetsCenters[i][2], planetsSpeed[i], planetsRotationSpeed[i], radii[i]); 
	}
	
	//initialize opengl buffers and variables inside of object
	for (int i = 0; i < numPlanets; i++)
	{
		planets[i].initGLBuffers(shaderProgram.programObject,"pos","nor","tex");
	}
	//initializa texture
	initTexture();
}


/////////////////////////////////////////////////////////////////////
///called when window size is changed
void reshape(int width, int height)
{
  windowWidth = width;
  windowHeight = height;
  //set viewport to match window size
  glViewport(0, 0, width, height);
  
  float fieldOfView = 45.0f;
  float aspectRatio = float(width)/float(height);
  float zNear = 0.1f;
  float zFar = 100.0f;
  //set projection matrix
  projectionMatrix = glm::perspective(fieldOfView,aspectRatio,zNear,zFar);
}

////////////////////////////////////////////////////////////////////
///actions for single frame
void display()
{
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  //Draw triangle with shaders (in screen coordinates)
  //need to set uniform in modelViewMatrix
  
  for (int i = 0; i < numPlanets; i++)
  {

	  glUseProgram(shaderProgram.programObject);

	  //camera matrix. camera is placed in point "eye" and looks at point "cen".
	  glm::mat4x4 viewMatrix = glm::lookAt(eye,cen,up);
	  //modelMatrix is connected with current object. move planets to scene's center
	  glm::mat4x4 modelMatrix = glm::translate(glm::mat4(),glm::vec3(0,0,0));

	  if (useMove)
		  planets[i].updateAngles();
	  glm::mat4x4 rotateMatrix = glm::rotate(glm::mat4(), planets[i].getAngleAroundSun(), glm::vec3(0.0f,0.0f,1.0f));
	  
	  glm::mat4x4 rotateAroundItselfMatrix = glm::rotate(glm::mat4(), planets[i].getAngleAroundItself(), glm::vec3(0.0f,0.0f,1.0f));
	  
	  glm::vec3 cents = planets[i].getCenterVec3(); 
	  glm::mat4x4 translationMatrix = glm::translate(glm::mat4(), cents); 
	  glm::mat4x4 reverseTranslationMatrix = glm::translate(glm::mat4(), -cents);

	  rotateMatrix *= translationMatrix * rotateAroundItselfMatrix * reverseTranslationMatrix; 

	  //modelViewMatrix consists of viewMatrix and modelMatrix
	  modelViewMatrix = viewMatrix*modelMatrix*rotateMatrix;
	  //calculate normal matrix 
	  normalMatrix = glm::inverseTranspose(modelViewMatrix);
	  //finally calculate modelViewProjectionMatrix
	  modelViewProjectionMatrix = projectionMatrix*modelViewMatrix;

	  //bind texture
	  glBindTexture(GL_TEXTURE_2D, texId[i]);
  
	  //pass variables to the shaders

	  int locMV = glGetUniformLocation(shaderProgram.programObject,"modelViewMatrix");
	  if (locMV>-1)
		  glUniformMatrix4fv(locMV,1,0,glm::value_ptr(modelViewMatrix));
	  int locN = glGetUniformLocation(shaderProgram.programObject,"normalMatrix");
	  if (locN>-1)
	  		  glUniformMatrix4fv(locN,1,0,glm::value_ptr(normalMatrix));
	  
	  int locP = glGetUniformLocation(shaderProgram.programObject,"modelViewProjectionMatrix");
	  if (locP>-1)
		  glUniformMatrix4fv(locP,1,0,glm::value_ptr(modelViewProjectionMatrix));
	  int texLoc = glGetUniformLocation(shaderProgram.programObject,"textureSampler");
	  if (texLoc>-1)
		  glUniform1ui(texLoc,0);
	  int locFlag = glGetUniformLocation(shaderProgram.programObject,"useTexture");
	  if (locFlag>-1)
		  glUniform1i(locFlag,useTexture);
  
	  //if there is some problem
	  if (locMV<0 || locN<0 || locP<0 || texLoc <0 || locFlag<0)
	  {
		  //not all uniforms were allocated - show blue screen.
		  //check your variables properly. May be there is unused?
		  glClearColor(0,0,1,1);
		  glClear(GL_COLOR_BUFFER_BIT);
	  }
	  else //draw planets
	  {
		  //draw object
		  planets[i].Draw();
	  }
  }
	//end frame visualization
	glutSwapBuffers();

}

//////////////////////////////////////////////////////////////////////////
///IdleFunction
void update()
{
	//make animation
	glutPostRedisplay();
}

inline float sqr(float x)
{
	return x * x; 
}

float len(glm::vec3 & v)
{
	return sqrt(sqr(v.x) + sqr(v.y) + sqr(v.z)); 
}

/////////////////////////////////////////////////////////////////////////
///is called when key on keyboard is pressed
///use SPACE to switch mode
///TODO: place camera transitions in this function
void keyboard(unsigned char key, int mx, int my)
{
	if (key==' ')
		useMove = !useMove;
	if (key == 'w' || key == 246)
	{
		glm::vec3 v = cen - eye; 
		
		v /= len(v);
		v *= 0.2; 

		eye += v;
		cen += v; 
	}
	if (key == 's' || key == 251) 
	{
		glm::vec3 v = cen - eye; 
		v /= len(v);
		v *= 0.2; 

		eye -= v;
		cen -= v; 
	}
	if (key == 'a' || key == 244)
	{
		glm::vec3 v = glm::cross(up, cen - eye); 
		v /= len(v);
		v *= 0.2; 

		eye += v;
		cen += v; 
	}
	if (key == 'd' || key == 226)
	{
		glm::vec3 v = glm::cross(up, cen - eye); 
		v /= len(v);
		v *= 0.2; 

		eye -= v;
		cen -= v; 
	}
}

/////////////////////////////////////////////////////////////////////////
///is called when mouse button is pressed
///TODO: place camera rotations in this function
void mouse(int button, int mode,int posx, int posy)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (mode == GLUT_DOWN)
		{
			mouseX = posx; mouseY = posy;

			float a = mouseX - windowWidth / 2;
			float b = - mouseY + windowHeight / 2; //up
			
			glm::vec3 tomove(mouseX - windowWidth / 2, - mouseY + windowHeight / 2, 0); 
			float l = len(tomove); 
			tomove /= l; 
			a /= l; 
			b /= l; 

			//turn up on b

			glm::vec3 v = cen - eye; 
			cen += up * b; 
			up -= v / 3.0f * b / 3.0f;

			up /= len(up); 
			v = cen - eye; 
			v /= len(v); 
			v *= 3.0; 
			cen = eye + v;

			//cout << glm::dot(v, up) << endl;  //works pretty good =)

			//turn right on a

			glm::vec3 ort = glm::cross(v, up); 
			ort /= len(ort); 
			cen += ort * a; 

			up /= len(up); 
			v = cen - eye; 
			v /= len(v); 
			v *= 3.0; 
			cen = eye + v;

			//cout << glm::dot(v, up) << endl << endl; works well too
		}
		else
		{
			mouseX = -1; mouseY = -1;
		}
	}
}

////////////////////////////////////////////////////////////////////////
///this function is used in case of InitializationError
void emptydisplay()
{
}

////////////////////////////////////////////////////////////////////////
///entry point
int main (int argc, char* argv[])
{
  glutInit(&argc, argv);
#ifdef __APPLE__
  glutInitDisplayMode( GLUT_3_2_CORE_PROFILE| GLUT_RGBA | GLUT_DOUBLE );
#else
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
  glutInitContextVersion (3, 2);  
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutInitContextFlags (GLUT_FORWARD_COMPATIBLE);
  glewExperimental = GL_TRUE;
#endif
  glutCreateWindow("Test OpenGL application");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutReshapeWindow(windowWidth,windowHeight);
  glutIdleFunc(update);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);

  glewInit();
  const char * slVer = (const char *) glGetString ( GL_SHADING_LANGUAGE_VERSION );
  cout << "GLSL Version: " << slVer << endl;

  try
  {
	  init();
  }
  catch (const char *str)
  {
	  cout << "Error During Initialiation: " << str << endl;
	  delete planets;
	  if (texId)
		glDeleteTextures(numPlanets, texId);
	  //start main loop with empty screen
	  glutDisplayFunc(emptydisplay);
	  glutMainLoop();
	  return -1;
  }


  try
  {
	glutMainLoop();
  }
  catch (const char *str)
  {
	cout << "Error During Main Loop: " << str << endl;
  }
  //release memory
  delete planets;
  glDeleteTextures(numPlanets, texId);
  return 0;
}
