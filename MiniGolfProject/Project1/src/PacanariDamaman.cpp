#include "PacanariDamaman.h"

const float MIN_BALL_SPEED = 0.005f;
const float MAX_BALL_SPEED = 0.03f;
const float ARROW_TURN_SPEED = 10.0f;
const float ANGLE_DRAG_SPEED = 0.4f;
const float SPHERE_LARGE_SCALE = 4.0f;
const float PACANARI_SCALE = 0.4f;

PacanariDamaman::PacanariDamaman(void)
{
}

PacanariDamaman::~PacanariDamaman(void)
{
}

void PacanariDamaman::GameInit(){	
	levelPars = resMan->getLevelPars();
	levelNames = resMan->getLevelNames();
	ballMoving = nextLevelCheat = false;
	arrowAngle = 0;
	strokeNum = totalScore = 0;
	objVerts.push_back(gameObjectData->at("ballVert"));
	objVerts.push_back(gameObjectData->at("cupVert"));
	objVerts.push_back(gameObjectData->at("pacanariBodyVert"));
	objVerts.push_back(gameObjectData->at("pacanariCollarVert"));
	objVerts.push_back(gameObjectData->at("pacanariLimbVert"));
	objNorms.push_back(gameObjectData->at("ballNorm"));
	objNorms.push_back(gameObjectData->at("cupNorm"));
	objNorms.push_back(gameObjectData->at("pacanariBodyNorm"));
	objNorms.push_back(gameObjectData->at("pacanariCollarNorm"));
	objNorms.push_back(gameObjectData->at("pacanariLimbNorm"));
	objColor.push_back(gameObjectData->at("ballColor"));
	objColor.push_back(gameObjectData->at("cupColor"));
	objColor.push_back(gameObjectData->at("pacanariBodyColor"));
	objColor.push_back(gameObjectData->at("pacanariCollarColor"));
	objColor.push_back(gameObjectData->at("pacanariLimbColor"));
	objIndices[0] = make_pair(0, 0);
	objIndices[1] = make_pair(1, 1);
	objIndices[2] = make_pair(2, 2);
	objIndices[3] = make_pair(3, 3);
	objIndices[4] = make_pair(4, 4);
	setDefaultModelView(defaultView);
		
	cup = Cup();
	ball = Ball();
	cup.setPos(cups[0].x, cups[0].y, cups[0].z);
	modelViews[1] = glm::translate(defaultView,glm::vec3(cup.x, cup.z, cup.y));
	ball.Initialize(tees[0].x, tees[0].y, tees[0].z);
	camManager = new Camera(&ball);
	camManager->setCamPos(5, 5, 7);
	camManager->setCamTarg(0, 0, 0);

	modelViews[2] = glm::scale(modelViews[2], glm::vec3(PACANARI_SCALE, PACANARI_SCALE, PACANARI_SCALE));

}

void PacanariDamaman::buildTileObjects(){
	for(int i = 0; i < levels[currLevel - 1].size(); i++){
		levels[currLevel - 1].at(i).buildAllTileObjects();
		for(int j = 0; j < levels[currLevel - 1].at(i).objSpL.size(); j++){
			glm::mat4 tempMat;
			objIndices[objIndices.size()] = make_pair(objIndices.size(), 0);
			isVisible.push_back(true);
			tempMat = glm::scale(defaultView, glm::vec3(SPHERE_LARGE_SCALE, SPHERE_LARGE_SCALE, SPHERE_LARGE_SCALE));
			tempMat = glm::translate(tempMat, glm::vec3(levels[currLevel - 1].at(i).objSpL[j].position->getX(),
												levels[currLevel - 1].at(i).objSpL[j].position->getZ(),
												levels[currLevel - 1].at(i).objSpL[j].position->getY()));
			modelViews.push_back(tempMat);
		}
		for(int j = 0; j < levels[currLevel - 1].at(i).objSpS.size(); j++){
			glm::mat4 tempMat;
			objIndices[objIndices.size()] = make_pair(objIndices.size(), 0);
			tempMat = glm::translate(tempMat, glm::vec3(levels[currLevel - 1].at(i).objSpS[j].position->getX(),
												levels[currLevel - 1].at(i).objSpS[j].position->getZ(),
												levels[currLevel - 1].at(i).objSpS[j].position->getY()));
			modelViews.push_back(tempMat);		
			isVisible.push_back(true);	
		}
	}
}

void PacanariDamaman::UpdateObjPositions(){	

}

bool PacanariDamaman::Update(){
	ball.position = ball.position->add(ball.velocity);
	camManager->setCamTarg(ball.position->getX(), ball.position->getZ(), ball.position->getY());
	//golfball.setVelocity(0, 0, 0);	
	ball.aim = arrowAngle;
	//Find the current tile we are on
	Tile* currTile = getTile(ball.position, physMan, currLevel - 1);
	ball.position->setY(physMan->getHeightOfBall(currTile, &ball));
	//std::cout <<"Height of ball is " << height <<std::endl;
	
	//Check to see if we need to bounce off a wall
	physMan->checkForCollision(currTile, &ball);

	//Check to see if we're in the cup
	if(checkWinCondition() || nextLevelCheat){
		playNextLevel();
		nextLevelCheat = false;
		return false;
	}

	//Apply physicsManager forces
	Vector3* normal = currTile->tileNorm;
	ball.velocity = physMan->applyForces(ball.velocity, currTile->slope, currTile->downVec);
	
	//std::cout << "Down vector is (" << curr.down->getX() << ", " << curr.down->getY() << ", " <<curr.down->getZ() << ")" <<std::endl;
	modelViews[0] = glm::translate(defaultView, glm::vec3(ball.position->getX(), ball.position->getZ(), ball.position->getY()));
	modelViews[2] = modelViews[0];
	modelViews[2] = glm::rotate(modelViews[0], arrowAngle - 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		
	if ((ball.velocity->getLength() < MIN_BALL_SPEED) && currTile->slope == 0) { 
		ballMoving = false;
		ball.velocity = new Vector3(0.0, 0.0, 0.0);
	} else ballMoving = true;
	//draw_string(0.0, 0.0, 0.0, "scojfadofj");	

	modelViews[3] = modelViews[4] = modelViews[2];
	//If the ball is not moving, we need to aim the ball
	//Calculate the angle between the mouse point and the ball position
	return true;
}

bool PacanariDamaman::checkWinCondition(){	
	float x = abs(ball.position->getX() - cup.x);
	float z = abs(ball.position->getZ() - cup.z);

	if (x < 0.12 && z < 0.12) {
		std::cout <<"Level Completed: Strokes: " << strokeNum << " Total Score: "<< totalScore <<endl;
		totalScore += strokeNum;
		strokeNum = 0;
		return true;
	}
	return false;
}

void PacanariDamaman::buildGameLevel(int level){
	ball.Initialize(tees[level].x, tees[level].y, tees[level].z);
	cup.setPos(cups[level].x, cups[level].y, cups[level].z);
	modelViews[1] = glm::translate(defaultView,glm::vec3(cup.x, cup.z, cup.y));
	//objIndices.clear();
	//objIndices[0] = make_pair(0, 0);
	//objIndices[1] = make_pair(1, 1);
	//objIndices[2] = make_pair(2, 2);
	//objIndices[3] = make_pair(3, 3);
	//objIndices[4] = make_pair(4, 4);
	buildTileObjects();
}

void PacanariDamaman::keyboard(unsigned char key, int x, int y){
    switch (key) {
    	case 27: exit(0); break;
		case 97: camManager->moveCam(Camera::dirLeft); break;
		case 98: modelViews[3] = glm::rotate(modelViews[3], arrowAngle, glm::vec3(0.0f, 0.0f, 1.0f)); break;
    	case 99: 
			if (camManager->getCamSetting() == Camera::FREE){  camManager->setCamSetting(Camera::TOP_DOWN); }
			else camManager->setCamSetting(Camera::FREE);
			break;
		case 100: camManager->moveCam(Camera::dirRight); break;
		case 101: arrowAngle-=ARROW_TURN_SPEED; break;
		case 102: playNextLevel(); nextLevelCheat = true; break;
		case 113: arrowAngle+=ARROW_TURN_SPEED; break;
    	case 114: buildGameLevel(currLevel - 1); break;
		case 115: camManager->moveCam(Camera::dirBack); break;
		case 119: camManager->moveCam(Camera::dirForward); break;
    	default: break;
    }
}

void PacanariDamaman::mouse(int button, int state, int x, int y){
	switch (button) {
		case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
			 if(!ballMoving){
				// golfBall->launchBall(MAX_BALL_SPEED);
				 ball.launchBall(MAX_BALL_SPEED, arrowAngle);
				 strokeNum++;
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

void PacanariDamaman::mouseMove(int x, int y){

}