//Tile.h

#pragma once

#include <vector>
#include "TileObject.h"
#include "SphereLarge.h"
#include "SphereSmall.h"

using namespace std;

class Physics;

class Tile {

public:
	int tileIndex;
	int vertices;
	
	Tile(void){};
	~Tile(void){};

	Vector3* tileNorm;
	Vector3* downVec;
	float slope;

	vector<SphereLarge> objSpL;
	vector<SphereSmall> objSpS;
	vector<float> xVertex;
	vector<float> yVertex;
	vector<float> zVertex;
	vector<int> neighbors;

	Tile(int tileInd, int numVerts){
		tileIndex = tileInd;
		vertices = numVerts;
	}

	Tile(int tileId, int numVerts, vector<float> xPos, vector<float> yPos,
						vector<float> zPos, vector<int> tileNeighbors) {
		tileIndex = tileId;
		vertices = numVerts;
		xVertex = xPos;
		yVertex = yPos;
		zVertex = zPos;
		neighbors = tileNeighbors;
	}

	void addVert(float xPos, float yPos, float zPos){
		xVertex.push_back(xPos);
		yVertex.push_back(yPos);
		zVertex.push_back(zPos);
	}
	
	void addNeighbor(int side){neighbors.push_back(side);}
	void buildAll();
	void buildAllTileObjects();
	
	float Tile::getHeightOfPoint(int x, int z);

	vector<float> getTVerts(){return tileVerts;}
	vector<float> getTNorms(){return tileNorms;}
	vector<float> getTColor(){return tileColor;}
	vector<float> getWVerts(){return wallVerts;}
	vector<float> getWNorms(){return wallNorms;}
	vector<float> getWColor(){return wallColor;}
	
private:
	vector<float> tileVerts;
	vector<float> tileNorms;
	vector<float> tileColor;
	vector<float> wallVerts;
	vector<float> wallNorms;
	vector<float> wallColor;
	vector<float> testNorms;

	void buildTile();
	void buildWalls();

	void calcNorms(Vector3* vec1, Vector3* vec2, Vector3* vec3, int pushNum, vector<float>* arrayToFill);
};