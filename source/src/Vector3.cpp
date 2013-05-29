
#include "Vector3.h"
#include <math.h>

Vector3::Vector3(float xPos, float yPos, float zPos) {
	x = xPos;
	y = yPos;
	z = zPos;
}

float Vector3::getLength() {
	return (sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z)));
}

float Vector3::dot(Vector3 *v) {
	float result = (this->x * v->x) + (this->y * v->y) + (this->z * v->z);
	return result;
}

Vector3* Vector3::cross(Vector3 *v) {
	float x, y, z;
	x = (this->y * v->z) - (this->z * v->y);
	y = (this->z * v->x) - (this->x * v->z);
	z = (this->x * v->y) - (this->y * v->x);
	Vector3 *result = new Vector3(x, y, z);
	return result;
}

Vector3* Vector3::add(Vector3 *v) {
	float x, y, z;
	x = this->x + v->x;
	y = this->y + v->y;
	z = this->z + v->z;
	Vector3 *result = new Vector3(x, y, z);
	return result;
}

Vector3* Vector3::subtract(Vector3 *v) {
	float x, y, z;
	x = this->x - v->x;
	y = this->y - v->y;
	z = this->z - v->z;
	Vector3 *result = new Vector3(x, y, z);
	return result;
}

Vector3* Vector3::normalize() {
	float x, y, z;
	float length = this->getLength();
	if (length == 0) return new Vector3(0, 0, 0);
	x = this->x / length;
	y = this->y / length;
	z = this->z / length;
	Vector3 *result = new Vector3(x, y, z);
	return result;
}

Vector3* Vector3::multiply(float value) {
	Vector3 *result = new Vector3(this->x * value, this->y * value, this->z * value);
	return result;
}

float Vector3::getX() {
	return this->x;
}

float Vector3::getY() {
	return this->y;
}

float Vector3::getZ() {
	return this->z;
}

void Vector3::setPos(float xVal, float yVal, float zVal) {
	x = xVal;
	y = yVal;
	z = zVal;
}

void Vector3::setX(float value) {
	x = value;
}

void Vector3::setY(float value) {
	y = value;
}

void Vector3::setZ(float value) {
	z = value;
}

Vector3* Vector3::calcNorms(Vector3* otherVec1, Vector3* otherVec2){
	
	float xNorm, yNorm, zNorm;
	Vector3* uVec = otherVec1->subtract(this);
	Vector3* vVec = otherVec2->subtract(this);
	xNorm = uVec->getY()*vVec->getZ() - uVec->getZ()*vVec->getY();
	yNorm = uVec->getZ()*vVec->getX() - uVec->getX()*vVec->getZ();
	zNorm = uVec->getX()*vVec->getY() - uVec->getY()*vVec->getX();

	Vector3 result = Vector3(xNorm, yNorm, zNorm);
	result.normalize();
	return &result;
}

bool operator== (Vector3 &vec1, Vector3 &vec2){
	return (vec1.getX() == vec2.getX() && vec1.getY() == vec2.getY() && vec1.getZ() == vec2.getZ());
}

bool operator!= (Vector3 &vec1, Vector3 &vec2){
	return (vec1.getX() != vec2.getX() || vec1.getY() != vec2.getY() || vec1.getZ() != vec2.getZ());
}