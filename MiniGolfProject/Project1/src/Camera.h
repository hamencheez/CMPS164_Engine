#pragma once

#include "Ball.h"
#include <vector>
#include <iostream>

#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/matrix_access.hpp"

#include <iostream>

using namespace std;

class Camera
{
private:
	Ball* minigolfBall;

public:
	Camera(void);
	Camera(Ball* ball);
	~Camera(void);

	//For glm::lookAt
	Vector3* camPos;
	Vector3* camTarg;
	Vector3* camUP;
	
	float camPosX, camPosY, camPosZ;
	float camTargX, camTargY, camTargZ;
	float distToKeep, distRange;
	bool freeLook, trackBall;

	enum CameraSetting {FREE, TRACK_BALL, TOP_DOWN};
	enum CamMoveDir {dirForward, dirBack, dirLeft, dirRight, dirUp, dirDown};

	CameraSetting camSetting;

	void setCamPos(float xPos, float yPos, float zPos){
		camPosX = xPos;
		camPosY = yPos;
		camPosZ = zPos;
		camPos->setPos(xPos, yPos, zPos);
	}
	
	void setCamTarg(float xPos, float yPos, float zPos){
		camTargX = xPos;
		camTargY = yPos;
		camTargZ = zPos;
		camTarg->setPos(xPos, yPos, zPos);
	}



	Ball* getBall(){return minigolfBall;}
	void setBall(Ball* ball){minigolfBall = ball;}

	glm::vec3 getCamPos();//{return glm::vec3(camPosX, camPosY, camPosZ);}
	glm::vec3 getCamTarg();//{return glm::vec3(camTargX, camTargY, camTargZ);}

	void update();
	void keepDistance();
	void moveCam(CamMoveDir dir);
	void moveTarg();
	void reset();

	// for debug:
	void printCamPos();
	void printCamTarg();

	//get camSetting function
	CameraSetting getCamSetting();
	void setCamSetting(CameraSetting camSet);
};



