#pragma once

#include "GameObject.h"

class PacanariDamaman: public GameObject
{
public:
	Ball* ball;
	Cup* cup;
	Tee* tee;

	vector<int> levelPars;
	vector<string> levelNames;

	PacanariDamaman(void);
	~PacanariDamaman(void);

	bool Update();
	void GameInit();

private:
	bool ballMoving;
};

