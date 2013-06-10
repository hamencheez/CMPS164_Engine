#include "Camera.h"

const float CAMERA_MOVE_SPEED = 0.5f;
const float CAMERA_DISTANCE_FROM_BALL = 7.0f;
const float CAMERA_DISTANCE_FROM_BALL_RANGE = 0.1f;

int camCount = 0;
int counter = 4;
Vector3* oldCamTarg = new Vector3(0.0f, 0.0f, 0.0f);
Vector3* oldCamPos = new Vector3(0.0f, 0.0f, 0.0f);

Camera::Camera(void){
}

Camera::Camera(Ball* ball)
{
	minigolfBall = ball;
	freeLook = false;
	trackBall = true;
	camSetting = FREE;
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
void Camera::update(){
	//printf("\nentering update");
	//camPos->print();
	//camTarg->print();
	switch(camSetting){
		case FREE: {
			//test:
			if(counter>0){
				printf("\nbeginning of free:");
				camPos->print();
				camTarg->print();
				counter--;
			}
			if(camCount != 0){ 
				//camPos = minigolfBall->position;
				printf("\nfirst time FREE, pos & targ:");
				camPos->print();
				camTarg->print();	
				printf("\nolds:");
				oldCamPos->print();
				oldCamTarg->print();
				camPos->setPos(oldCamPos->getX(), oldCamPos->getY(), oldCamPos->getZ());
				//camTarg = oldCamTarg;
				printf("\nFREE, after:");
				camPos->print();
				camTarg->print();	
				printf("\nolds:");
				oldCamPos->print();
				oldCamTarg->print();
			}
			if(trackBall) camTarg = minigolfBall->position;	
			if(counter>0){
				printf("\nleaving update, case FREE:");
				camPos->print();
				camTarg->print();
				counter--;
			}
			camCount = 0;
		}break;
		case TRACK_BALL:{
			//testremove:
			keepDistance();
			camTarg = minigolfBall->position;	
			printf("\nleaving update, case track ball");
			camCount = 0;
		} break;
		case TOP_DOWN:{
			if(camCount != 2){ 
				printf("\nfirst time in topdown?");
				camPos->print();
				camTarg->print();	
				printf("\nolds:");
				oldCamPos->print();
				oldCamTarg->print();
				oldCamPos->setPos(camPos->getX(), camPos->getY(), camPos->getZ());
				//oldCamTarg = camTarg;
				//camPos = minigolfBall->position;	
				setCamPos(0, 0, 0);
				camPos->setZ((camPos->getZ()) + distToKeep); 
				printf("\ntopdown pos and trag after:");
				camPos->print();
				camTarg->print();	
				printf("\nolds:");
				oldCamPos->print();
				oldCamTarg->print();
				
			}
			//printf("\nleaving update, case top down");
			camCount = 2;
			counter = 4;
		} break;
		default: cout << "Cam setting not recognized" << endl; break;
	}
	//test:
	//delete camPos;
	//delete camTarg;
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
			dif = dif->normalize();
			dif = dif->multiply(CAMERA_MOVE_SPEED);
			camPos = camPos->add(dif);				
		} break;
		case dirBack:{
			Vector3* dif = camTarg->subtract(camPos);
			dif = dif->normalize();
			dif = dif->multiply(CAMERA_MOVE_SPEED);
			camPos = camPos->subtract(dif);
		} break;
		case dirLeft:{
			Vector3* dif = camTarg;
			dif = dif->subtract(camPos);
			dif = dif->multiply(-1);
			dif = dif->cross(camUP);
			dif = dif->normalize();
			dif = dif->multiply(CAMERA_MOVE_SPEED);
			camPos = camPos->add(dif);
		} break;
		case dirRight:{
			Vector3* dif = camTarg->subtract(camPos);
			dif = dif->cross(camUP);
			dif = dif->normalize();
			dif = dif->multiply(CAMERA_MOVE_SPEED);
			camPos = camPos->add(dif);
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

void Camera::printCamPos() { this->camPos->print();}

void Camera::printCamTarg() { camTarg->print();}

glm::vec3 Camera::getCamPos(){return glm::vec3(camPos->getX(), camPos->getY(), camPos->getZ());}
glm::vec3 Camera::getCamTarg(){return glm::vec3(camTarg->getX(), camTarg->getY(), camTarg->getZ());}

Camera::CameraSetting  Camera::getCamSetting(){
	return camSetting;
}

void Camera::setCamSetting(CameraSetting camSet){
	camSetting = camSet;
}