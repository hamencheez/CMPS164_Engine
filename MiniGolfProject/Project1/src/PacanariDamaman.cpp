#include "PacanariDamaman.h"

const float MIN_BALL_SPEED = 0.005f;
const float MAX_BALL_SPEED = 0.03f;
const float ARROW_TURN_SPEED = 10.0f;
const float ANGLE_DRAG_SPEED = 0.4f;

PacanariDamaman::PacanariDamaman(void)
{
}


PacanariDamaman::~PacanariDamaman(void)
{
}

void PacanariDamaman::GameInit(){	
	levelPars = resMan->getLevelPars();
	levelNames = resMan->getLevelNames();
	ballMoving = false;
	ball->Initialize(tees[0].x, tees[0].y, tees[0].z);
}

bool PacanariDamaman::Update(){
	ball->position = ball->position->add(ball->velocity);
	//golfBall->setVelocity(0, 0, 0);	
	ball->aim = 0;

	//Find the current tile we are on
	Tile* currTile = getTile(ball->position, physMan, currLevel - 1);
	ball->position->setY(physMan->getHeightOfBall(currTile, ball));
	//std::cout <<"Height of ball is " << height <<std::endl;
	
	//Check to see if we need to bounce off a wall
	physMan->checkForCollision(currTile, ball);

	//Check to see if we're in the cup
//	checkForBallInHole();

	//Apply physicsManager forces
	Vector3* normal = currTile->tileNorm;
	ball->velocity = physMan->applyForces(ball->velocity, currTile->slope, currTile->downVec);
	
	//std::cout << "Down vector is (" << curr.down->getX() << ", " << curr.down->getY() << ", " <<curr.down->getZ() << ")" <<std::endl;
	modelViews[0] = glm::translate(defaultView, glm::vec3(ball->position->getX(), ball->position->getZ(), ball->position->getY()));
	modelViews[1] = modelViews[0];
	modelViews[1] = glm::rotate(modelViews[1], 0 - 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		
	if ((ball->velocity->getLength() < MIN_BALL_SPEED) && currTile->slope == 0) { 
		ballMoving = false;
		ball->velocity = new Vector3(0.0, 0.0, 0.0);
	} else ballMoving = true;
	//draw_string(0.0, 0.0, 0.0, "scojfadofj");	

	//If the ball is not moving, we need to aim the ball
	//Calculate the angle between the mouse point and the ball position
	modelViews[1] = modelViews[0];

	modelViews[1] = glm::rotate(modelViews[1], 0 - 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	return true;
}