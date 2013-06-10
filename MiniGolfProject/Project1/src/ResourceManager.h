//ResourceManger.h

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "Tee.h"
#include "Cup.h"
#include "Ball.h"
#include "Physics.h"
#include "Vector3.h"

using namespace std;

class ResourceManager {

	private:
		vector<Tile> tiles;
		vector<vector<Tile>> levels;
		vector<Cup> cups;
		vector<Tee> tees;
		map<string, vector<float>> objData;
		map<string, vector<float>> vertData;
		vector<int> levelPar;
		vector<string> levelNames;

		void addTile(Tile &newTile, int levelInd);
		void addCup(Cup &newCup);
		void addTee(Tee &newTee);
		void clearVecs();

	public:
		ResourceManager();
		~ResourceManager();
		
		void readTileFile(char* inputFile);
		void readCourseFile(char* inputFile);
		void readObjFile(char* inputFile);

		map<string, vector<float>>* getObjData(){return &objData;};
		vector<float> getObjVector(string ID);
		vector<vector<Tile>> getLevels();
				
		vector<Tile> tileList();
		vector<Cup> getCups(){return cups;};
		vector<Tee> getTees(){return tees;};
		vector<int> getLevelPars(){return levelPar;};
		vector<string> getLevelNames(){return levelNames;};

		Tile getTile(Vector3* pos, Tile last, Physics* physics, int level);
};