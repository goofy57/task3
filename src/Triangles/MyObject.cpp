#include "MyObject.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>


MyObject::MyObject():
	dataCount(0),
	indexesCount(0),
	pData(0),
	pIndexes(0), 
	radius(1.0),
	speed(1), 
	centx(0), 
	centy(0), 
	centz(0), 
	rotSun(0), 
	rotSelf(0) {}

void MyObject::fillData(float x, float y, float z, float _speed, float _rotSpeed, float _radius)
{
	centx = x; 
	centy = y; 
	centz = z; 
	speed = _speed; 
	rotSpeed = _rotSpeed; 
	radius = _radius; 
	generateData();
}

glm::vec3 MyObject::getCenterVec3()
{
	return glm::vec3(centx, centy, centz); 
}


MyObject::~MyObject(void)
{
	glDeleteBuffers(2,vbo);
	glDeleteVertexArrays(1,&vao);
	if (pData)
	{
		delete[] pData;
		delete[] pIndexes;
	}
}

float MyObject::getSpeed()
{
	return speed; 
}

void MyObject::initGLBuffers(GLuint programId, const char* posName,const char* norName,const char* texName)
{
	glGenVertexArrays( 1, &vao );
	glBindVertexArray ( vao );

	glGenBuffers ( 2, &vbo[0]);
	
	glBindBuffer ( GL_ARRAY_BUFFER, vbo[0] );
	glBufferData ( GL_ARRAY_BUFFER, dataCount*sizeof(VertexData), pData, GL_STATIC_DRAW );
		
	glEnable(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, vbo[1] );
	glBufferData ( GL_ELEMENT_ARRAY_BUFFER, indexesCount*sizeof(unsigned int), pIndexes, GL_STATIC_DRAW );
	
	int	loc = glGetAttribLocation(programId, posName);
	if (loc>-1)
	{
		glVertexAttribPointer(loc,3,GL_FLOAT,GL_FALSE,sizeof(VertexData),(GLuint *)0);
		glEnableVertexAttribArray (loc);
	}
	int loc2 = glGetAttribLocation(programId, norName);
	if (loc2>-1)
	{
		glVertexAttribPointer(loc2,3,GL_FLOAT,GL_FALSE,sizeof(VertexData),(GLuint *)(0+sizeof(float)*3));
		glEnableVertexAttribArray (loc2);
	}
	int loc3 = glGetAttribLocation(programId, texName);
	if (loc3>-1)
	{
		glVertexAttribPointer(loc3,2,GL_FLOAT,GL_FALSE,sizeof(VertexData),(GLuint *)(0+sizeof(float)*6));
		glEnableVertexAttribArray (loc3);
	}	
}

void MyObject::Draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES,indexesCount,GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
}

void MyObject::updateAngles()
{
	rotSun += speed; 
	rotSelf += rotSpeed; 
}

float MyObject::getAngleAroundItself()
{
	return rotSelf; 
}

float MyObject::getAngleAroundSun()
{
	return rotSun; 
}

void MyObject::generateData()
{
	const double Pi = 4 * atan(1.0); 

	if (pData)
	{
		delete[] pData;
		delete[] pIndexes;
	}

	int rows = 51; 
	int cols = 50; 

	dataCount = rows * cols; 
	indexesCount = rows * cols * 6; 

    pData = new VertexData [dataCount];
	pIndexes = new unsigned int [indexesCount];

	VertexData * data = pData; 
	unsigned int * ind = pIndexes; 

	for (int i = -rows/2; i <= rows/2; i++)
	{
		double tetta = i * Pi / 2 / (rows / 2); 
		float z = sin(tetta); 
		double smallr = cos(tetta); 
		for (int j = 0; j < cols; j++)
		{
			double phi = j * 1.0 / (cols - 1) * 2 * Pi; 

			float x = smallr * cos(phi); 
			float y = smallr * sin(phi); 

			data->pos = glm::vec3(centx + x * radius, centy + y * radius, centz + z * radius);
			data->nor = glm::vec3(x, y, z); 
			data->tex = glm::vec2(j * 1.0 / (cols - 1), (i + rows / 2) * 1.0 / (rows - 1)); 
			data++; 
		}
	}

	for (int i = 0; i < rows - 1; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int p1 = i * cols + j; 
			int p2 = i * cols + (j + 1) % cols; 
			int p3 = p1 + cols;  
			int p4 = p2 + cols; 
			
			*ind++ = p1;
			*ind++ = p2; 
			*ind++ = p3; 
			
			*ind++ = p2; 
			*ind++ = p3; 
			*ind++ = p4; 
		}
	}
}