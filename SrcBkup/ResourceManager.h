//ResourceManger.h

#pragma once

#include <vector>
#include "Tee.h"
#include "Cup.h"
#include "Physics.h"
#include "Vector3.h"

using namespace std;

class ResourceManager {

	private:
		vector<Tile> tiles;
		vector<vector<Tile>> levels;
		vector<Cup> cups;
		vector<Tee> tees;
		vector<int> levelPar;
		vector<float> levelVerts;
		vector<float> levelNorms;
		vector<float> levelColor;
		vector<float> wallVerts;
		vector<float> wallNorms;
		vector<float> wallColor;
		vector<float> ballVerts;
		vector<float> ballNorms;
		vector<float> ballColor;
		vector<float> arrowVerts;
		vector<float> arrowNorms;
		vector<float> arrowColor;
		vector<float> cupVerts;
		vector<float> cupNorms;
		vector<float> cupColor;
		vector<float> testNorms;

		int numLevelVerts, numLevelNorms, numLevelColors;
		int numWallVerts, numWallNorms, numWallColors;
		int numBallVerts, numBallNorms, numBallColors;
		int numArrowVerts, numArrowNorms, numArrowColors;

		//void pushNorms(int tileInd, int vertexInd, bool wallNorms);
		void buildArrays();
		void buildArrays(int levelInd);
		void buildTiles();
		//void buildWalls();

		void addTile(Tile &newTile, int levelInd);
		void addTile(Tile &newTile);
		void addCup(Cup &newCup);
		void addTee(Tee &newTee);
		void clearVecs();

	public:
		ResourceManager();
		ResourceManager(Ball* ball);
		~ResourceManager();

		Ball* ball;
		Cup cup;
		Tee tee;

		int maxLevel;

		void readTileFile(char* inputFile);
		void readCourseFile(char* inputFile);
		void readObjFileVerts(char* inputFile);

		void playNextLevel(int level);

		vector<float> getLevelVerts(){return levelVerts;}
		vector<float> getLevelNorms(){return levelNorms;}
		vector<float> getLevelColor(){return levelColor;}
		vector<float> getWallVerts(){return wallVerts;}
		vector<float> getWallNorms(){return wallNorms;}
		vector<float> getWallColor(){return wallColor;}
		vector<float> getBallVerts(){return ballVerts;}
		vector<float> getBallNorms(){return ballNorms;}
		vector<float> getBallColor(){return ballColor;}
		vector<float> getArrowVerts(){return arrowVerts;}
		vector<float> getArrowNorms(){return arrowNorms;}
		vector<float> getArrowColor(){return arrowColor;}
		vector<float> getCupVerts(){return cupVerts;}
		vector<float> getCupNorms(){return cupNorms;}
		vector<float> getCupColor(){return cupColor;}
		vector<float> getTestNorms(){return testNorms;}

		int getNumLevelVerts(){return numLevelVerts;}
		int getNumLevelNorms(){return numLevelNorms;}
		int getNumLevelColor(){return numLevelColors;}
		int getNumWallVerts(){return numWallVerts;}
		int getNumWallNorms(){return numWallNorms;}
		int getNumWallColor(){return numWallColors;}
		int getNumBallVerts(){return numBallVerts;}
		int getNumBallNorms(){return numBallNorms;}
		int getNumBallColor(){return numBallColors;}
		int getNumArrowVerts(){return numArrowVerts;}
		int getNumArrowNorms(){return numArrowNorms;}
		int getNumArrowColor(){return numArrowColors;}
		
		vector<Tile> tileList();
		Ball* getBall(){return ball;}
		Tee getTee(int level){return tees[level];}
		Tee getTee(){return tee;}
		Cup getCup(int level){return cups[level];}
		Cup getCup(){return cup;}

		Tile getTile(int id);
		Tile getTile(Vector3* pos, Physics* physics);
		Tile getTile(Vector3* pos, Physics* physics, int level);
};