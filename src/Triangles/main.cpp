//standard libraries
#include <iostream>
#include <fstream>
#include <vector>
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
glm::vec3 eye(0.0, 0.0, 13.0);
//reference point position
glm::vec3 cen(0.0, 0.0, 10.0);
//up vector direction (head of observer)
glm::vec3 up(0.0, 1.0, 0.0);

double moveUp = 0, moveRight = 0; 
int IterationsMove = 0;
int currentPlanetType;

//matrices
glm::mat4x4 modelViewMatrix;
glm::mat4x4 projectionMatrix;
glm::mat4x4 modelViewProjectionMatrix;
glm::mat4x4 normalMatrix;

///defines drawing mode
bool useTexture = true;
bool useMove = false; 
bool smallPlanets = false;
bool schemeView = true;
bool uselessFlag = false; 
float changeSpeed = 3600.0f;

const double G = 6.673848E-11;
const int numPlanets = 12; 
const char * paths[] = {"C:\\task3\\src\\models\\sun.bmp", "C:\\task3\\src\\models\\mercury.bmp", "C:\\task3\\src\\models\\venus.bmp", "C:\\task3\\src\\models\\earth.bmp", "C:\\task3\\src\\models\\mars.bmp", "C:\\task3\\src\\models\\jupiter.bmp", "C:\\task3\\src\\models\\saturn.bmp", "C:\\task3\\src\\models\\uranus.bmp", "C:\\task3\\src\\models\\neptune.bmp", "C:\\task3\\src\\models\\pluto.bmp", "C:\\task3\\src\\models\\stars.bmp", "C:\\task3\\src\\models\\atmosphere.bmp"};
GLubyte * imgData[numPlanets]; 
MyObject *planets;
double plantetsCenters[][3] = 
	{
		{9.166368072918009, -3.587908596635106E+01, -1.263365847270218}, //sun
		{3.981532574646779E+03,  2.693705871595619E+03, -1.427017526993466E+02}, //mercury
		{1.083995110582697E+04, -7.516123504840386E+02, -6.361445678009944E+02}, //venus
		{1.162398804913440E+04,  9.214935380493926E+03, -1.560961036842257}, //earth
		{-1.528793878916806E+04, 1.925541334868633E+04, 7.784109759604024E+02}, //mars
		{-1.308404872563110E+04, 7.618480179221750E+04, -2.483656477704518E+01}, //jupiter
		{-1.062988073620396E+05, -1.022591702447227E+05, 6.008502801056857E+03}, //saturn
		{2.946083841697208E+05, 5.529078700946414E+04, -3.611419689746372E+03}, //uranus
		{4.036568780165731E+05, -1.954926579794473E+05, -5.276881058283679E+03}, //neptune
		{9.074663629135412E+04, -4.775885697847751E+05, 2.485565548180889E+04}, //pluto
		{1e-6, 1e-6, 1e-6}, //starMap
		{13, 13, 13} //atmosphere
	};

double planetsSpeed[][3] = 
	{
		{1.068407615399651E-06, 2.825998901862247E-07, -2.451813399270692E-08}, 
		{-3.712878513620788E-3, 4.226944115449314E-3, 6.860766793768278E-4}, 
		{2.163430099495670E-4, 3.478693145745870E-3, 3.521473884663605E-5}, 
		{-1.903255921308336E-3,  2.319272201812972E-3, -9.394261929108316E-08},
		{-1.805947412894641E-3, -1.298811643007845E-3, 1.711362049767525E-05}, 
		{-1.303624071468951E-3, -1.591321557884244E-4, 2.983270239243695E-05}, 
		{6.170956639472989E-4, -6.985778300302598E-4, -1.241099702155237E-05}, 
		{-1.305921743797635E-4, 6.375718119862134E-4, 4.060658963891937E-06}, 
		{2.332992134044233E-4, 4.924012112924557E-4, -1.551671101516667E-05}, 
		{5.444809029005323E-4, -8.040967446408351E-06, -1.566356777898360E-4},
		{0, 0, 0}, 
		{0, 0, 0}
	}; 

double radii[] = {695990 * 1e3, 2439.7 * 1e3, 6051.8 * 1e3, 6371 * 1e3, 3389.5 * 1e3, 69911 * 1e3, 57316 * 1e3, 25266 * 1e3, 24552.5 * 1e3, 1195 * 1e3, 1e13, 8000 * 1e3};

double ownAxis[] = {7.25, 2.11, 177.36, 23.44, 25.1919, 3.13, 26.73, 97.77, 28.32, 119.591, 0.0, 0.0};

const double day = 24 * 10; 
double planetsRotationSpeed[] = {
	1.0 / 25.38 / day, 
	1.0 / 58.646 / day, 
	1.0 / 243.023 / day, 
	1.0 / 0.99726968 / day, 
	1.0 / 1.029 / day,
	1.0 / 0.4147 / day, 
	1.0 / 0.4416 / day, 
	1.0 / 0.71833 / day, 
	1.0 / 0.6653 / day, 
	1.0 / 6.38723 / day,
	0.0, 
	0.0
}; 

double planetsMass[] = {1.99e+30, 3.33e+23, 4.87e+24, 5.97e+24, 6.42E+23, 1.90E+27, 5.68E+26, 8.68E+25, 1.02E+26, 1.30E+22, 0.0, 0.0}; 

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
		fseek(inp, 54, SEEK_SET);
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	//initialize shader program
	shaderProgram.init(VertexShaderName,FragmentShaderName);
	//use this shader program
	glUseProgram(shaderProgram.programObject);

	planets = new MyObject[numPlanets];

	for (int i = 0; i < numPlanets; i++)
	{
		planets[i].fillData(plantetsCenters[i][0], plantetsCenters[i][1], plantetsCenters[i][2], planetsRotationSpeed[i], radii[i], ownAxis[i], planetsMass[i], planetsSpeed[i][0], planetsSpeed[i][1], planetsSpeed[i][2]); 
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
  float zFar = INFINITE;
  //set projection matrix
  projectionMatrix = glm::perspective(fieldOfView,aspectRatio,zNear,zFar);
}

inline float sqr(float x)
{
	return x * x; 
}

float len(glm::vec3 & v)
{
	return sqrt(sqr(v.x) + sqr(v.y) + sqr(v.z)); 
}

inline double sqrd(double x)
{
	return x * x; 
}

double lend(glm::dvec3 v)
{
	return sqrt(sqrd(v.x) + sqrd(v.y) + sqrd(v.z)); 
}

void turnUp(float b)
{
	glm::vec3 v = cen - eye; 
	cen += up * b; 
	up -= v / 3.0f * b / 3.0f;

	up /= len(up); 
	v = cen - eye; 
	v /= len(v); 
	v *= 3.0; 
	cen = eye + v;

	//cout << glm::dot(v, up) << endl;  //works pretty good =)
}

void turnRight(float a)
{
	glm::vec3 v = cen - eye;
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

////////////////////////////////////////////////////////////////////
///actions for single frame

inline int sign(double x)
{
	if (x < 0) return -1; 
	return 1;
}

glm::vec3 fscheme(glm::vec3 v)
{
	if (len(v) < 0.05) return v;
	return v * (log(len(v))) / len(v);
}

void updatePlanets()
{ 
  if (!useMove) return; 

  double allTime = changeSpeed; 

  while (allTime > 1e-6)
  {
	  double dt = min(allTime, 3600.0); 

	 vector<glm::dvec3> newCoords(10); 

	  for (int i = 0; i < 10; i++)
	  {
		  glm::dvec3 v = planets[i].getVelocity(); 
		  glm::dvec3 x = planets[i].getRealCenter();

		  glm::dvec3 a(0.0, 0.0, 0.0);

		  for (int j = 0; j < 10; j++)
		  {
			  if (i == j) continue; 
			  glm::dvec3 u = planets[j].getRealCenter() - x; 
			  double l = sqrt(u.x * u.x + u.y * u.y + u.z * u.z); 
			  a += ((u * G * planets[j].getMass()) / (l * l * l)) / 1e21; 
		  }

		  if (useMove)
		  {
			  newCoords[i] = v * dt + a * dt * dt * 0.5; 
			  planets[i].updateVelocity(v + a * dt);
		  }
		  else
		  {
			  newCoords[i] = glm::vec3(0.0f, 0.0f, 0.0f); 
		  }
	  }

	  for (int i = 0; i < 10; i++)
	  {
		  planets[i].updateCenters(newCoords[i]); 
	  }

	  allTime -= dt;

  }

  glutPostRedisplay();
}

void display()
{
	if (IterationsMove)
	{
		IterationsMove--; 
		turnUp(moveUp); 
		turnRight(moveRight);
	}
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  for (int i = 0; i < numPlanets; i++)
  {
	  if (i == 10 && uselessFlag) continue; 
	  if (i == 11)
	  {
		  planets[i].x = planets[3].x + 1e-6; 
		  planets[i].y = planets[3].y + 1e-6; 
		  planets[i].z = planets[3].z + 1e-6; 
	  }
	  glUseProgram(shaderProgram.programObject);

	  //camera matrix. camera is placed in point "eye" and looks at point "cen".
	  glm::mat4x4 viewMatrix = glm::lookAt(eye,cen,up);

	  glm::mat4x4 rotateAroundItselfMatrix = glm::rotate(glm::mat4(), planets[i].getAngleAroundItself(), glm::vec3(0.0f,0.0f,1.0f));
	  
	  glm::vec3 cents = planets[i].getCenterVec3(); 

	  glm::mat4x4 rotateMatrix = glm::rotate(glm::mat4(), planets[i].getAngleAroundSun(), glm::vec3(0.0f,0.0f,1.0f));
	  	  
	  double scaleFactor = log(radii[i]) / 30.0;

	  glm::mat4x4 modelMatrix; 
	  
	  if (schemeView && i != 10) modelMatrix = glm::translate(glm::mat4(), fscheme(static_cast<glm::vec3>(planets[i].getRealCenter()))) * glm::scale(glm::mat4(), glm::vec3(scaleFactor, scaleFactor, scaleFactor)); 
	  else modelMatrix = glm::translate(glm::mat4(), static_cast<glm::vec3>(planets[i].getRealCenter() / 10.0)) * glm::scale(glm::mat4(), glm::vec3(radii[i] / 1e7, radii[i] / 1e7, radii[i] / 1e7)); 
	  
	  glm::mat4x4 rotAxisMat = glm::rotate(glm::mat4(), (float)ownAxis[i], glm::vec3(0, 1, 0)); 

	  rotateAroundItselfMatrix = rotAxisMat * rotateAroundItselfMatrix;

	  glm::mat4x4 oneMatrix = modelMatrix * rotateAroundItselfMatrix * glm::translate(glm::mat4(), -cents);

	  if (useMove)
		planets[i].updateAngles(changeSpeed);

	  modelViewMatrix = viewMatrix * oneMatrix;
	
	  if (i && schemeView) 
	  {
		  glm::vec3 cur = fscheme(static_cast<glm::vec3>(planets[i].getRealCenter())); 
		  glm::mat4x4 translationMatrix = glm::translate(glm::mat4(), cur); 
		  glm::mat4x4 reverseTranslationMatrix = glm::translate(glm::mat4(), -(static_cast<glm::vec3>(planets[i].getRealCenter())));

		  normalMatrix = glm::inverseTranspose(glm::lookAt(fscheme(static_cast<glm::vec3>(planets[0].getRealCenter())), cur, up) * translationMatrix * rotateAroundItselfMatrix * reverseTranslationMatrix);
	  }
	  if (i && !schemeView)
	  {
		  glm::vec3 cur = static_cast<glm::vec3>(planets[i].getRealCenter()) / 10.0f; 
		  glm::mat4x4 translationMatrix = glm::translate(glm::mat4(), cur); 
		  glm::mat4x4 reverseTranslationMatrix = glm::translate(glm::mat4(), -(static_cast<glm::vec3>(planets[i].getRealCenter())));

		  normalMatrix = glm::inverseTranspose(glm::lookAt(static_cast<glm::vec3>(planets[0].getRealCenter() / 10.0), cur, up) * translationMatrix * rotateAroundItselfMatrix * reverseTranslationMatrix);
	  }

	  //finally calculate modelViewProjectionMatrix
	  modelViewProjectionMatrix = projectionMatrix*modelViewMatrix;
	  //bind texture

	  glBindTexture(GL_TEXTURE_2D, texId[i]);
  
	  //pass variables to the shaders
	  if (i) currentPlanetType = 1;
	  if (i == 0) currentPlanetType = 0; 
	  if (i == 10) currentPlanetType = 2;
	  if (i == 11) currentPlanetType = 3; 

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
	  int locType = glGetUniformLocation(shaderProgram.programObject,"perceivesLight");
	  if (locType>-1)
		  glUniform1i(locType,currentPlanetType);

  
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
	updatePlanets();
	glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////////
///is called when key on keyboard is pressed
///use SPACE to switch mode
///TODO: place camera transitions in this function

double findClosestDist()
{
	if (schemeView) return 0.1; 

	double ans = 1e100; 
	for (int i = 0; i < numPlanets; i++)
	{
		ans = min(ans, glm::length(static_cast<glm::dvec3>(eye) - planets[i].getRealCenter() / 10.0)); 
	}
	return ans / 30.0;
}

void keyboard(unsigned char key, int mx, int my)
{
	glm::vec3 oldeye = eye, oldcen = cen; 

	if (key==' ')
		useMove = !useMove;
	if (key == 'w' || key == 246)
	{
		glm::vec3 v = cen - eye; 
		
		v = glm::normalize(v); 
		v *= findClosestDist(); 

		eye += v;
		cen += v; 
	}
	if (key == 's' || key == 251) 
	{
		glm::vec3 v = cen - eye; 
		
		v = glm::normalize(v); 
		v *= findClosestDist(); 

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
	if (key == '+') 
	{
		changeSpeed *= 1.1; 
	}
	if (key == '-')
	{
		changeSpeed /= 1.1;
	}
	if (key == 'q' || key == 233)
	{
		schemeView = !schemeView; 
	}
	if (key >= '0' && key <= '9' && !schemeView) 
	{
		glm::vec3 v = eye - cen; 
		cen = planets[key - '0'].getRealCenter() / 10.0; 
		eye = cen + v; 
	}
	if (key >= '0' && key <= '9' && schemeView) 
	{
		glm::vec3 v = eye - cen; 
		cen = fscheme(static_cast<glm::vec3>(planets[key - '0'].getRealCenter())); 
		eye = cen + v; 
	}
	if (key == 'u' || key == 227) 
	{
		uselessFlag = !uselessFlag; 
	}

	if (glm::length(cen) > 5e5) 
	{
		eye = oldeye, cen = oldcen; 
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

			float a = mouseX * 2.0 / windowWidth - 1;
			float b = -mouseY * 2.0 / windowHeight + 1; //up
			
			//glm::vec3 tomove(mouseX * 2.0 / windowWidth - 1, -mouseY * 2.0 / windowHeight + 1, 0); 
			//float l = len(tomove); 
			//tomove /= l; 
			//a /= l; 
			//b /= l; 

			moveUp = (moveUp * IterationsMove + b) / 100; 
			moveRight = (moveRight * IterationsMove + a) / 100; 
			IterationsMove = 100; 
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
