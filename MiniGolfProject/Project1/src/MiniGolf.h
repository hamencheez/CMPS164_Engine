#pragma once

#include "GameObject.h"

class MiniGolf: public GameObject
{
public:
	Ball ball;
	Cup cup;
	Tee tee;

	vector<int> levelPars;
	vector<string> levelNames;

	MiniGolf(void);
	~MiniGolf(void);

	bool Update();
	void GameInit();

	void keyboard(unsigned char key, int x, int y);
	void mouse(int button, int state, int x, int y);
	void mouseMove(int x, int y);

private:
	float arrowAngle;
	bool ballMoving;

	void buildGameLevel(int level);
};

