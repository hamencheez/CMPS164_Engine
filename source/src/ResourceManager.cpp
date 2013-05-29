
#include "ResourceManager.h"

#include <iostream>
#include <fstream>

using namespace std;

ResourceManager::ResourceManager() {
	//Fill in if you need to
	levelVerts = levelNorms = levelColor = vector<float>();
	wallVerts = wallNorms = wallColor = vector<float>();
	ballVerts = ballColor = ballNorms = vector<float>();
	arrowVerts = arrowColor = arrowNorms = vector<float>();
}

ResourceManager::ResourceManager(Ball* newBall) {
	ball = newBall;
}

void ResourceManager::addTile(Tile &newTile) {
	tiles.push_back(newTile);
}

void ResourceManager::addCup(Cup &newCup) {
	cup = newCup;
}

void ResourceManager::addTee(Tee &newTee) {
	tee = newTee;
}

std::vector<Tile> ResourceManager::tileList() {
	return tiles;
}

Tile ResourceManager::getTile(int id) {
	for (int itor = 0; itor < tiles.size(); ++itor) {
		if (tiles.at(itor).tileIndex) return tiles.at(itor);
	}
	cerr <<"Tile ID not recognized" << endl;
	exit(-1);
}

Tile ResourceManager::getTile(Vector3* pos, Physics* physics) {
	float minY = 99999;
	float maxY = -99999;

	for (unsigned int itor = 0; itor < tiles.size(); ++itor) {
		if (physics->pointOnPlane(tiles.at(itor).xVertex, tiles.at(itor).zVertex, pos->getX(), pos->getZ())) {
		
			for (int i = 0; i < tiles.at(itor).yVertex.size(); i++) {
				if (tiles.at(itor).yVertex.at(i) < minY) minY = tiles.at(itor).yVertex.at(i);
				if (tiles.at(itor).yVertex.at(i) > maxY) maxY = tiles.at(itor).yVertex.at(i);
			}
			//if (pos->getY() >= (minY - 0.1)){
			//	if(pos->getY() <= (maxY + 0.1)) {
					return tiles.at(itor);
			//	}
			//}
		}
	}

	std::cout << "Tile not found" <<std::endl;
	
	//Return first tile
	return tiles.at(0);
}

//iterates through tile list, building all of the necessary verts
//then initializes ball
void ResourceManager::buildArrays(){
	levelVerts = levelNorms = levelColor = wallVerts = wallNorms = wallColor= vector<float>();
	vector<float> activeVec = vector<float>();

	for(int i = 0; i < tiles.size(); i++){
		tiles[i].buildAll();
		activeVec = tiles[i].getTVerts();
		levelVerts.insert(levelVerts.end(), activeVec.begin(), activeVec.end());
		activeVec = tiles[i].getTNorms();
		levelNorms.insert(levelNorms.end(), activeVec.begin(), activeVec.end());
		activeVec = tiles[i].getTColor();
		levelColor.insert(levelColor.end(), activeVec.begin(), activeVec.end());
		activeVec = tiles[i].getWVerts();
		wallVerts.insert(wallVerts.end(), activeVec.begin(), activeVec.end());
		activeVec = tiles[i].getWNorms();
		wallNorms.insert(wallNorms.end(), activeVec.begin(), activeVec.end());
		activeVec = tiles[i].getWColor();
		wallColor.insert(wallColor.end(), activeVec.begin(), activeVec.end());
	}

	numLevelVerts = levelVerts.size();
	numLevelNorms = levelNorms.size();
	numLevelColors = levelColor.size();
	numWallVerts = wallVerts.size();
	numWallNorms = wallNorms.size();
	numWallColors = wallColor.size();
	numBallVerts = ballVerts.size();
	numBallNorms = ballNorms.size();
	numBallColors = ballColor.size();
	numArrowVerts = arrowVerts.size();
	numArrowNorms = arrowNorms.size();
	numArrowColors = arrowColor.size();

	for(int i = 0; i < numLevelVerts - 3; i+=3){
		testNorms.push_back(levelVerts[i]);
		testNorms.push_back(levelVerts[i+1]);
		testNorms.push_back(levelVerts[i+2]);
		testNorms.push_back(levelVerts[i] + levelNorms[i]);
		testNorms.push_back(levelVerts[i+1] + levelNorms[i+1]);
		testNorms.push_back(levelVerts[i+2] + levelNorms[i+2]);
	}

	float minY = 9999.0f;
	for(int i = 0; i < numBallVerts - 3; i+=3){
		if(ballVerts[i+1] < minY) minY = ballVerts[i+1];
	}
	tee.y += abs(minY);
	ball->initialize(tee.x, tee.y, tee.z);
}

void ResourceManager::readTileFile(char* filename) {	
	FILE * f;
	
	char word[80];
	int tileInd, numVerts, neighbor;
	float x, y, z;

	f = fopen (filename, "r");

	if (f!=NULL)
	 {
	  while(fscanf(f, "%s", &word) > 0){
		  if(strcmp(word, "tile") == 0){
			  fscanf(f, "%d %d ", &tileInd, &numVerts);
			  Tile newTile = Tile(tileInd, numVerts);
			  for(int i = 0; i < numVerts; i++){
				  fscanf(f, "%f %f %f", &x, &y, &z);
				  newTile.addVert(x, y, z);
			  }
			  for(int i = 0; i < numVerts; i++){
				  fscanf(f, "%d", &neighbor);
				  newTile.addNeighbor(neighbor);
			  }
			  addTile(newTile);
		  } else if(strcmp(word, "cup") == 0){
			  fscanf(f, "%d %f %f %f", &tileInd, &x, &y, &z);
			  Cup newCup = Cup(tileInd, x, y, z);
			  cup = newCup;
		  } else if(strcmp(word, "tee") == 0){
			  fscanf(f, "%d %f %f %f", &tileInd, &x, &y, &z);
			  Tee newTee = Tee(tileInd, x, y, z);
			  tee = newTee;
		  } else{
			  cout << "Do not recognize tile type" << endl;
			  exit(-1);
		  }
	  }
    fclose (f);
	}
	buildArrays();
}

void ResourceManager::readObjFileVerts(char* filename){
	vector<vector<float>> rawVerts;
	rawVerts = vector<vector<float>>();
	Vector3 vec1;
	Vector3 vec2;
	Vector3 vec3;
	Vector3* res;
	FILE * f;

	bool ball;
	char word[80];
	float x, y, z;
	int ind1, ind2, ind3;

	int lineNum = 0;
	f = fopen (filename, "r");	

	if (f!=NULL)
	{
		fscanf(f, "%s", &word);
		if(strcmp(word, "arrow") == 0){
			ball = false;
		} else if(strcmp(word, "ball") == 0){
			ball = true;
		} else {
			cout << "invalid obj file" << endl;
		}
		while(fscanf(f, "%s", &word) > 0){
			if(strcmp(word, "v") == 0){
				rawVerts.push_back(vector<float>());
				fscanf(f, "%f %f %f", &x, &y, &z);
			//	if(ball){
					x = x*7/100;
					y = y*7/100;
					z = z*7/100;
		//		}
				rawVerts.at(lineNum).push_back(x);
				rawVerts.at(lineNum).push_back(z);
				rawVerts.at(lineNum).push_back(y);
				lineNum++;
			} else if(strcmp(word, "f") == 0){
				if(ball){
					fscanf(f, "%d %d %d", &ind1, &ind2, &ind3);
					ballVerts.push_back(rawVerts[ind1-1].at(0));
					ballVerts.push_back(rawVerts[ind1-1].at(1));
					ballVerts.push_back(rawVerts[ind1-1].at(2));
					ballVerts.push_back(rawVerts[ind2-1].at(0));
					ballVerts.push_back(rawVerts[ind2-1].at(1));
					ballVerts.push_back(rawVerts[ind2-1].at(2));
					ballVerts.push_back(rawVerts[ind3-1].at(0));
					ballVerts.push_back(rawVerts[ind3-1].at(1));
					ballVerts.push_back(rawVerts[ind3-1].at(2));

					for(int i = 0; i < 3; i++){
						ballColor.push_back(0.9f);
						ballColor.push_back(0.9f);
						ballColor.push_back(0.9f);
						ballColor.push_back(1.0f);
					}
					lineNum++;
				} else if(!ball) {
					fscanf(f, "%d %d %d", &ind1, &ind2, &ind3);
					arrowVerts.push_back(rawVerts[ind1-1].at(0));
					arrowVerts.push_back(rawVerts[ind1-1].at(1));
					arrowVerts.push_back(rawVerts[ind1-1].at(2));
					arrowVerts.push_back(rawVerts[ind2-1].at(0));
					arrowVerts.push_back(rawVerts[ind2-1].at(1));
					arrowVerts.push_back(rawVerts[ind2-1].at(2));
					arrowVerts.push_back(rawVerts[ind3-1].at(0));
					arrowVerts.push_back(rawVerts[ind3-1].at(1));
					arrowVerts.push_back(rawVerts[ind3-1].at(2));

					for(int i = 0; i < 3; i++){
						arrowColor.push_back(0.2f);
						arrowColor.push_back(0.85f);
						arrowColor.push_back(0.2f);
						arrowColor.push_back(1.0f);
					}
					lineNum++;
				}
			}
		}
		rawVerts.clear();
	}
	cout << "successfull obj file read" << endl;
    fclose (f);
}