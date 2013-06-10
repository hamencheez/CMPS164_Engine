#include "MiniGolf.h"

const float MIN_BALL_SPEED = 0.005f;
const float MAX_BALL_SPEED = 0.03f;
const float ARROW_TURN_SPEED = 10.0f;
const float ANGLE_DRAG_SPEED = 0.4f;

MiniGolf::MiniGolf(void)
{
}

MiniGolf::~MiniGolf(void)
{
}

void MiniGolf::GameInit(){	
	levelPars = resMan->getLevelPars();
	levelNames = resMan->getLevelNames();
	ballMoving = false;
	arrowAngle = 0;
	cup = Cup();
	ball = Ball();
	ball.Initialize(tees[0].x, tees[0].y, tees[0].z);
	objVerts.push_back(gameObjectData->at("ballVert"));
	objVerts.push_back(gameObjectData->at("arrowVert"));
	objVerts.push_back(gameObjectData->at("cupVert"));
	objNorms.push_back(gameObjectData->at("ballNorm"));
	objNorms.push_back(gameObjectData->at("arrowNorm"));
	objNorms.push_back(gameObjectData->at("cupNorm"));
	objColor.push_back(gameObjectData->at("ballColor"));
	objColor.push_back(gameObjectData->at("arrowColor"));
	objColor.push_back(gameObjectData->at("cupColor"));
	objIndices[0] = make_pair(0, 0);
	objIndices[1] = make_pair(1, 1);
	objIndices[2] = make_pair(2, 2);
	setDefaultModelView(defaultView);
		
	camManager = new Camera(&ball);
	camManager->setCamPos(5, 5, 7);
	camManager->setCamTarg(0, 0, 0);
}

bool MiniGolf::Update(){
	ball.position = ball.position->add(ball.velocity);
	//golfball.setVelocity(0, 0, 0);	
	ball.aim = arrowAngle;
	//Find the current tile we are on
	Tile* currTile = getTile(ball.position, physMan, currLevel - 1);
	ball.position->setY(physMan->getHeightOfBall(currTile, &ball));
	//std::cout <<"Height of ball is " << height <<std::endl;
	
	//Check to see if we need to bounce off a wall
	physMan->checkForCollision(currTile, &ball);

	//Check to see if we're in the cup
	//if(checkForBallInHole()){
	//	playNextLevel();
	//}

	//Apply physicsManager forces
	Vector3* normal = currTile->tileNorm;
	ball.velocity = physMan->applyForces(ball.velocity, currTile->slope, currTile->downVec);
	
	//std::cout << "Down vector is (" << curr.down->getX() << ", " << curr.down->getY() << ", " <<curr.down->getZ() << ")" <<std::endl;
	modelViews[0] = glm::translate(defaultView, glm::vec3(ball.position->getX(), ball.position->getZ(), ball.position->getY()));
	modelViews[1] = modelViews[0];
	modelViews[1] = glm::rotate(modelViews[1], 0 - 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		
	if ((ball.velocity->getLength() < MIN_BALL_SPEED) && currTile->slope == 0) { 
		ballMoving = false;
		ball.velocity = new Vector3(0.0, 0.0, 0.0);
	} else ballMoving = true;
	//draw_string(0.0, 0.0, 0.0, "scojfadofj");	

	//If the ball is not moving, we need to aim the ball
	//Calculate the angle between the mouse point and the ball position
	modelViews[1] = modelViews[0];
	modelViews[1] = glm::rotate(modelViews[1], arrowAngle - 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	return true;
}

void MiniGolf::buildGameLevel(int level){
	ball.Initialize(tees[level].x, tees[level].y, tees[level].z);
	cup.setPos(cups[level].x, cups[level].y, cups[level].z);
}

void MiniGolf::keyboard(unsigned char key, int x, int y){
    switch (key) {
    	case 27: exit(0); break;
		case 97: camManager->moveCam(Camera::dirLeft); break;
		case 98: modelViews[3] = glm::rotate(modelViews[3], arrowAngle, glm::vec3(0.0f, 0.0f, 1.0f)); break;
    	case 99: 
			if (camManager->getCamSetting() == Camera::FREE){  camManager->setCamSetting(Camera::TOP_DOWN); }
			else camManager->setCamSetting(Camera::FREE);
			break;
		case 100: camManager->moveCam(Camera::dirRight); break;
		//case 101: arrowAngle-=ARROW_TURN_SPEED; break;
		//case 102: newLevel(); break;
		//case 113: arrowAngle+=ARROW_TURN_SPEED; break;
    	//case 114: resetBall(); break;
		case 115: camManager->moveCam(Camera::dirBack); break;
		case 119: camManager->moveCam(Camera::dirForward); break;
    	default: break;
    }
}

void MiniGolf::mouse(int button, int state, int x, int y){
	switch (button) {
		case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
			 if(!ballMoving){
				// golfBall->launchBall(MAX_BALL_SPEED);
				// strokeNum++;
			 }
			 glutPostRedisplay();
         }
         else if (state == GLUT_UP) {
			 glutPostRedisplay();
         }
         break;
      case GLUT_RIGHT_BUTTON:
         if (state == GLUT_DOWN) {
 			 glutPostRedisplay();
         }
         else if (state == GLUT_UP) {
 			 glutPostRedisplay();
         }
         break;
      default:
         break;
   }
}		

void MiniGolf::mouseMove(int x, int y){

}