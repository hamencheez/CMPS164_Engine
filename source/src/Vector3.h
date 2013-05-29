#pragma once

#include <math.h>

class Vector3 {
private:
	float x;
	float y;
	float z;

public:
	Vector3(void){};
	~Vector3(void){};
	Vector3(float xPos, float yPos, float zPos);
	float getLength();
	float dot(Vector3 *v);
	Vector3* cross(Vector3 *v);
	Vector3* add(Vector3 *v);
	Vector3* subtract(Vector3 *v);
	Vector3* normalize();

	//Multiplies by scalar
	Vector3* multiply(float value);

	//Retrieve values
	float getX();
	float getY();
	float getZ();


	//Set values
	void setX(float value);
	void setY(float value);
	void setZ(float value);
	void setPos(float xPos, float yPos, float zPos);
	Vector3* calcNorms(Vector3* otherVec1, Vector3* otherVec2);

	friend bool operator== (Vector3 &vec1, Vector3 &vec2);
	friend bool operator!= (Vector3 &vec1, Vector3 &vec2);

};