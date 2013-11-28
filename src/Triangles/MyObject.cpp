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
	centx(0), 
	centy(0), 
	centz(0), 
	rotSun(0), 
	rotSelf(0) {}

void MyObject::fillData(double _x, double _y, double _z, double _rotSpeed, double _radius, double _ownAxisAngle, double _mass, double _vx, double _vy, double _vz)
{
	centx = x = _x; 
	centy = y = _y; 
	centz = z = _z; 
	rotSpeed = _rotSpeed; 
	radius = _radius; 
	ownAxisAngle = _ownAxisAngle;
	mass = _mass; 
	vx = _vx; 
	vy = _vy; 
	vz = _vz;
	generateData();
}

glm::dvec3 MyObject::getVelocity()
{
	return glm::dvec3(vx, vy, vz); 
}

double MyObject::getMass()
{
	return mass;
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

glm::dvec3 MyObject::getRealCenter()
{
	return glm::dvec3(x, y, z); 
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

void MyObject::updateAngles(float sp)
{
	rotSelf += sp * rotSpeed; 
	while (rotSelf > 360) rotSelf -= 360; 
}

float MyObject::getAngleAroundItself()
{
	return rotSelf; 
}

float MyObject::getAngleAroundSun()
{
	return rotSun; 
}

float MyObject::getOwnAxisAngle()
{
	return ownAxisAngle;
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

			data->pos = glm::vec3(centx + x, centy + y, centz + z);
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

void MyObject::updateVelocity(glm::dvec3 v)
{
	vx = v.x, vy = v.y, vz = v.z; 
}

void MyObject::updateCenters(glm::dvec3 v)
{
	x += v.x, y += v.y, z += v.z; 
}