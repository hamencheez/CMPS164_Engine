#include "Tile.h"
#include "Physics.h"
#include <ctime>

using namespace std;

const float OBJ_DENSITY = 0.9f;
const float WALL_HEIGHT = 0.2f;

float Tile::getHeightOfPoint(int x1, int z1){
	Vector3* norm = tileNorm;
	float x, y, z, height;
	x = norm->getX() * (xVertex.at(0) - x1);
	y = norm->getY() * yVertex.at(0);
	z = norm->getZ() * (zVertex.at(0) - z1);
	height = norm->multiply(0.02)->getY();
	return ((x + y + z) / norm->getY()) + abs(height);
}

void Tile::buildAllTileObjects(){
	srand((unsigned)time(0));

	float maxX, maxZ;
	float minX, minZ;
	float x, y, z;
	int numObjects;
	float area;
	maxX = maxZ = -9999;
	minX = minZ = 9999;
	
	for (unsigned int curr = 0; curr < xVertex.size() - 1; curr++) {
		if (xVertex[curr] < minX) minX = xVertex[curr];
		if (xVertex[curr] > maxX) maxX = xVertex[curr];
		if (zVertex[curr] < minZ) minZ = zVertex[curr];
		if (zVertex[curr] > maxZ) maxZ = zVertex[curr];
	}

	area = (maxX - minX) * (maxZ - minZ);
	numObjects = (int)(area * OBJ_DENSITY);

	for(int i = 0; i < numObjects; i++){
		x = minX + (float)rand()/((float)RAND_MAX/(maxX-minX));
		z = minZ + (float)rand()/((float)RAND_MAX/(maxZ-minZ));
		y = getHeightOfPoint(x, z);
		if(rand() % 100 > 90){
			objSpL.push_back(SphereLarge(x, y, z, objSpL.size()));
		} else {
			objSpS.push_back(SphereSmall(x, y, z, objSpS.size()));
		}
	}
}

//Builds tiles and any surrounding wall
//initializes values
void Tile::buildAll(){
	tileVerts = tileNorms = tileColor = vector<float>();
	wallVerts = wallNorms = wallColor = vector<float>();
	buildWalls();
	buildTile();

	slope = acos(tileNorm->getY() / sqrt((tileNorm->getX() * tileNorm->getX()) + (tileNorm->getY() * tileNorm->getY()) + (tileNorm->getZ() * tileNorm->getZ())));
	slope = (slope * 180) / PI;
	slope = abs(slope);

	downVec = new Vector3(0, -1, 0);
	downVec = tileNorm->cross(downVec);
	downVec = downVec->cross(tileNorm);
	downVec = downVec->normalize();
}

//triangulates any tile
void Tile::buildTile(){
	float xPos, yPos, zPos;
	
	for(int j = 0; j <= vertices - 3; j++){
		xPos = xVertex[0];
		yPos = yVertex[0];
		zPos = zVertex[0];
		tileVerts.push_back(xPos);
		tileVerts.push_back(zPos);
		tileVerts.push_back(yPos);
		Vector3 vec1 = Vector3(xPos, zPos, yPos);

		xPos = xVertex[j+1];
		yPos = yVertex[j+1];
		zPos = zVertex[j+1];
		tileVerts.push_back(xPos);
		tileVerts.push_back(zPos);
		tileVerts.push_back(yPos);
		Vector3 vec2 = Vector3(xPos, zPos, yPos);
				
		xPos = xVertex[j+2];
		yPos = yVertex[j+2];
		zPos = zVertex[j+2];
		tileVerts.push_back(xPos);
		tileVerts.push_back(zPos);
		tileVerts.push_back(yPos);
		Vector3 vec3 = Vector3(xPos, zPos, yPos);
		
		for(int k = 0; k < 3; k++){
			tileColor.push_back(0.1f);
			tileColor.push_back(0.8f);
			tileColor.push_back(0.1f);
			tileColor.push_back(1.0f);
		}

		calcNorms(&vec1, &vec2, &vec3, 3, &tileNorms);
	}
}

//Builds quads of walls
void Tile::buildWalls(){
	for(int i = 0; i < neighbors.size(); i++){
		if(neighbors[i] <= 0){
			if(i != neighbors.size() - 1){
				wallVerts.push_back(xVertex[i]);
				wallVerts.push_back(zVertex[i]);
				wallVerts.push_back(yVertex[i]);
				Vector3 vec1 = Vector3(xVertex[i], yVertex[i], zVertex[i]);
				wallVerts.push_back(xVertex[i+1]);
				wallVerts.push_back(zVertex[i+1]);
				wallVerts.push_back(yVertex[i+1]);
				Vector3 vec2 = Vector3(xVertex[i+1], yVertex[i+1], zVertex[i+1]);
				wallVerts.push_back(xVertex[i+1]);
				wallVerts.push_back(zVertex[i+1]);
				wallVerts.push_back(yVertex[i+1] + WALL_HEIGHT);
				Vector3 vec3 = Vector3(xVertex[i+1], yVertex[i+1], zVertex[i] + WALL_HEIGHT);
				wallVerts.push_back(xVertex[i]);
				wallVerts.push_back(zVertex[i]);
				wallVerts.push_back(yVertex[i] + WALL_HEIGHT);
				for(int k = 0; k < 4; k++){
					wallColor.push_back(0.7f);
					wallColor.push_back(0.3f);
					wallColor.push_back(0.3f);
					wallColor.push_back(1.0f);
				}
				calcNorms(&vec1, &vec2, &vec3, 4, &wallNorms);
			} else {
				wallVerts.push_back(xVertex[i]);
				wallVerts.push_back(zVertex[i]);
				wallVerts.push_back(yVertex[i]);
				Vector3 vec1 = Vector3(xVertex[i], zVertex[i], yVertex[i]);
				wallVerts.push_back(xVertex[0]);
				wallVerts.push_back(zVertex[0]);
				wallVerts.push_back(yVertex[0]);
				Vector3 vec2 = Vector3(xVertex[0], zVertex[0], yVertex[0]);
				wallVerts.push_back(xVertex[0]);
				wallVerts.push_back(zVertex[0]);
				wallVerts.push_back(yVertex[0] + WALL_HEIGHT);
				Vector3 vec3 = Vector3(xVertex[0], zVertex[0], yVertex[0] + WALL_HEIGHT);
				wallVerts.push_back(xVertex[i]);
				wallVerts.push_back(zVertex[i]);
				wallVerts.push_back(yVertex[i] + WALL_HEIGHT);
				for(int k = 0; k < 4; k++){
					wallColor.push_back(0.5f);
					wallColor.push_back(0.4f);
					wallColor.push_back(0.4f);
					wallColor.push_back(1.0f);
				}
				calcNorms(&vec1, &vec2, &vec3, 4, &wallNorms);
			}
		}
	}
}

//polygon normal calculator
void Tile::calcNorms(Vector3* vec1, Vector3* vec2, Vector3* vec3, int pushNum, vector<float>* vecToFill){
	float xNorm, yNorm, zNorm;
	Vector3* uVec = vec3->subtract(vec1);
	Vector3* vVec = vec2->subtract(vec1);
	xNorm = uVec->getY()*vVec->getZ() - uVec->getZ()*vVec->getY();
	yNorm = uVec->getZ()*vVec->getX() - uVec->getX()*vVec->getZ();
	zNorm = uVec->getX()*vVec->getY() - uVec->getY()*vVec->getX();

	Vector3* result = new Vector3(xNorm, zNorm, yNorm);
	result = result->normalize();
	tileNorm = result;

	for(int i = 0; i < pushNum; i++){
		vecToFill->push_back(result->getX());
		vecToFill->push_back(result->getZ());
		vecToFill->push_back(result->getY());
	}
}