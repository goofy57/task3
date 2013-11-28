#pragma once
#include <glm/glm.hpp>

//helper struct for Vertex
//contains position, normal and texture coordinates
struct VertexData
{
	 glm::vec3 pos;
	 glm::vec3 nor;
	 glm::vec2 tex;
};


//some object for drawing
class MyObject
{
public:

	VertexData* pData;	//pointer to object's internal data
	unsigned int dataCount;

	double rotSpeed, radius, centx, centy, centz; 
	double rotSun, rotSelf; 
	double ownAxisAngle;
	double mass; 
	double vx, vy, vz; 
	double x, y, z; 

	unsigned int* pIndexes;	//pointer to indexes (list of vetrices) 
	unsigned int indexesCount;
	
	unsigned int vbo[2];//VertexBufferObject one for MeshVertexData, another for Indexes
	unsigned int vao;//one VertexArrayObject

	MyObject();
	~MyObject(void);
	//function for initialization
	void initGLBuffers(unsigned int programId, const char* posName,const char* norName,const char* texName);
	void fillData(double _x, double _y, double _z, double _rotSpeed, double _radius, double _ownAxisAngle, double _mass, double _vx, double _vy, double _vz); 
	glm::vec3 getCenterVec3();
	glm::dvec3 getRealCenter(); 
	glm::dvec3 getVelocity();
	double getMass(); 
	void updateAngles(float sp); 
	float getAngleAroundSun();
	float getAngleAroundItself(); 
	float getOwnAxisAngle();
	void updateVelocity(glm::dvec3); 
	void updateCenters(glm::dvec3); 
	//function for drawing
	void Draw();

private:
	//helper function generates two triangles
	void generateData();

};

