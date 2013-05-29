#include "Camera.h"

const float CAMERA_MOVE_SPEED = 0.5f;
const float CAMERA_DISTANCE_FROM_BALL = 1.0f;
const float CAMERA_DISTANCE_FROM_BALL_RANGE = 0.1f;

Camera::Camera(void){
}

Camera::Camera(Ball* ball)
{
	minigolfBall = ball;
	freeLook = false;
	trackBall = true;
	camSetting = TRACK_BALL;
	distToKeep = CAMERA_DISTANCE_FROM_BALL;
	distRange = CAMERA_DISTANCE_FROM_BALL_RANGE;

	camUP = new Vector3(0.0f, 0.0f, 1.0f);
	camTarg = new Vector3(0.0f, 0.0f, 0.0f);
	camPos = new Vector3(0.0f, 0.0f, 0.0f);
}


Camera::~Camera(void)
{
}

//The goal is update based on camera setting, doesn't quite work at the moment
//need to revise moveCam
void Camera::update(CameraSetting camSetting){
	switch(camSetting){
		case FREE: {
			if(trackBall) camTarg = minigolfBall->position;	
		}break;
		case TRACK_BALL:{
			keepDistance();
			camTarg = minigolfBall->position;	
		} break;
		case TOP_DOWN: {
			camPos = minigolfBall->position;
			camPos->setY(camPos->getY() + distToKeep);
		} break;
		default: cout << "Cam setting not recognized" << endl; break;
	}
	delete camPos;
	delete camTarg;
}

//Untested, in theory should work
void Camera::keepDistance(){
	Vector3* dif = camTarg->subtract(camPos);
	if(dif->getLength() > distToKeep + distRange){
		camPos = dif->normalize();
		camPos->multiply(distToKeep + distRange);
	} else if(dif->getLength() < distToKeep - distRange){
		camPos = dif->normalize();
		camPos->multiply(distToKeep - distRange);
	}
}

//Needs some debugging to figure out why the camera isn't
//changing based on this function
void Camera::moveCam(CamMoveDir dir){
	switch(dir){
		case dirForward:{
			Vector3* dif = camTarg->subtract(camPos);
			dif->normalize();
			dif->multiply(CAMERA_MOVE_SPEED);
			camPos->add(dif);				
		} break;
		case dirBack:{
			Vector3* dif = camTarg->subtract(camPos);
			dif->normalize();
			dif->multiply(CAMERA_MOVE_SPEED);
			camPos->subtract(dif);				
		} break;
		case dirLeft:{
			Vector3* dif = camTarg->subtract(camPos);
			dif->multiply(-1);
			dif->cross(camUP);
			dif->normalize();
			dif->multiply(CAMERA_MOVE_SPEED);
			camPos->add(dif);				
		} break;
		case dirRight:{
			Vector3* dif = camTarg->subtract(camPos);
			dif->cross(camUP);
			dif->normalize();
			dif->multiply(CAMERA_MOVE_SPEED);
			camPos->add(dif);
		} break;
	}
}

void Camera::moveTarg(){
	trackBall = false;
}

void Camera::reset(){
	freeLook = false;
	trackBall = true;

}