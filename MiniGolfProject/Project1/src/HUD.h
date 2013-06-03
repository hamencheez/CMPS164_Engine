#pragma once

#include <math.h>
#include <vector>
#include "Vector3.h"
using namespace std;

class HUD {
private:
	vector<float> splashVerts;
	vector<float> HUDverts;
	vector<float> splashColor;
	vector<float> HUDColor;
	vector<float> arrow;
	float score; //tracker for the current score
	int course; //tracker for the current hole
	int PAR; //the course's PAR

public:
	vector<float> splashOptions; //this will contain the coordinates for the selections' colors
	void compensateHUD(Vector3 camPos, Vector3 camTarg); //this will move the HUD to move with the camera.
	void buildSplash();
	void buildHUD(); // this builds the HUD, including the key, the scores, and the arrow
	void addToScore(float add);
	void updateCourseNumber(int number);
	void updatePAR(int number);
};