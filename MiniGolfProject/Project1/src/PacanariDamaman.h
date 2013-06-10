#pragma once

#include "GameObject.h"

class PacanariDamaman: public GameObject
{
public:
	Ball ball;
	Cup cup;
	Tee tee;

	int strokeNum, totalScore;

	vector<int> levelPars;
	vector<string> levelNames;

	PacanariDamaman(void);
	~PacanariDamaman(void);

	void UpdateObjPositions();
	bool Update();
	void GameInit();

	void keyboard(unsigned char key, int x, int y);
	void mouse(int button, int state, int x, int y);
	void mouseMove(int x, int y);

private:
	void updatePacanariDamaman();
	void buildTileObjects();
	bool checkWinCondition();

	float arrowAngle, rightAngle, leftAngle;
	bool swingDir;

	bool ballMoving;
	bool nextLevelCheat;

	void buildGameLevel(int level);
};

