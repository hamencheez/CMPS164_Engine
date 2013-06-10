#pragma once

#include "GameObject.h"

class PacanariDamaman: public GameObject
{
public:
	Ball ball;
	Cup cup;
	Tee tee;

	int totalScore;

	vector<SphereLarge> objLarge;
	vector<SphereSmall> objSmall;
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
	bool leftMouseDown;
	int boost;
	void updatePacanariDamaman();
	void buildTileObjects();
	bool checkWinCondition();
	void checkObjCollect();

	float arrowAngle, rightAngle, leftAngle;
	bool swingDir;

	bool ballMoving;
	bool nextLevelCheat;

	void buildGameLevel(int level);
};

