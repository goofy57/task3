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
	VertexData* pData;	//pointer to object's internal data
	unsigned int dataCount;

	float speed, rotSpeed, radius, centx, centy, centz; 
	float rotSun, rotSelf; 

	unsigned int* pIndexes;	//pointer to indexes (list of vetrices) 
	unsigned int indexesCount;
	
	unsigned int vbo[2];//VertexBufferObject one for MeshVertexData, another for Indexes
	unsigned int vao;//one VertexArrayObject

public:
	MyObject();
	~MyObject(void);
	//function for initialization
	void initGLBuffers(unsigned int programId, const char* posName,const char* norName,const char* texName);
	void fillData(float x = 0.0f, float y = 0.0f, float z = 0.0f, float _speed = 0.0f, float _rotSpeed = 0.0f, float _radius = 1.0f); 
	glm::vec3 getCenterVec3();
	void updateAngles(); 
	float getAngleAroundSun();
	float getAngleAroundItself(); 
	float getSpeed();
	//function for drawing
	void Draw();

private:
	//helper function generates two triangles
	void generateData();

};

