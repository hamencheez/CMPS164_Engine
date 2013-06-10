#pragma once

#include "GameObject.h"

class MiniGolf: public GameObject
{
public:
	Ball ball;
	Cup cup;
	Tee tee;

	int strokeNum, totalScore;

	vector<int> levelPars;
	vector<string> levelNames;

	MiniGolf(void);
	~MiniGolf(void);

	void UpdateObjPositions();
	bool Update();
	void GameInit();

	void keyboard(unsigned char key, int x, int y);
	void mouse(int button, int state, int x, int y);
	void mouseMove(int x, int y);

private:
	bool checkWinCondition();
	float arrowAngle;
	bool ballMoving;

	void buildGameLevel(int level);
};

