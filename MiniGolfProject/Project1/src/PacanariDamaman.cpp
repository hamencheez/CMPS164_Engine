#include "PacanariDamaman.h"

const float MIN_BALL_SPEED = 0.001f;
const float MAX_BALL_SPEED = 0.007f;
const float BOOST_SPEED = 0.2f;
const float BALL_ACCEL = 0.0003;
const float ARROW_TURN_SPEED = 10.0f;
const float ANGLE_DRAG_SPEED = 0.4f;
const float SPHERE_LARGE_SCALE = 2.0f;
const float SPHERE_SMALL_SCALE = 0.5f;
const float PACANARI_SCALE = 0.27f;

const float LIMB_SWING_SPEED = 15.0f;
const float LIMB_MAX_ANGLE = 60.0f;
const float LIMB_MIN_ANGLE = -60.0f;

PacanariDamaman::PacanariDamaman(void)
{
}

PacanariDamaman::~PacanariDamaman(void)
{
}

void PacanariDamaman::GameInit(){	
	levelPars = resMan->getLevelPars();
	levelNames = resMan->getLevelNames();
	ballMoving = nextLevelCheat = swingDir = leftMouseDown = false;
	arrowAngle = rightAngle = leftAngle = 0;
	totalScore = boost = 0;
	ballRot = ballRotSpeed = 0;
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
	objIndices[5] = make_pair(5, 4);
	objIndices[6] = make_pair(6, 4);
	objIndices[7] = make_pair(7, 4);
	setDefaultModelView(defaultView);
		
	cup = Cup();
	ball = Ball();
	cup.setPos(cups[0].x, cups[0].y, cups[0].z);
	modelViews[1] = glm::translate(defaultView,glm::vec3(cup.x, cup.z, cup.y));
	ball.Initialize(tees[0].x, tees[0].y, tees[0].z);
	camManager = new Camera(&ball);


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
			objLarge.push_back(levels[currLevel - 1].at(i).objSpL[j]);
			SphereLarge* temp = &objLarge.back();
			temp->id = objIndices.size() - 1;
		}
		for(int j = 0; j < levels[currLevel - 1].at(i).objSpS.size(); j++){
			glm::mat4 tempMat;
			objIndices[objIndices.size()] = make_pair(objIndices.size(), 0);
			tempMat = glm::translate(tempMat, glm::vec3(levels[currLevel - 1].at(i).objSpS[j].position->getX(),
												levels[currLevel - 1].at(i).objSpS[j].position->getZ(),
												levels[currLevel - 1].at(i).objSpS[j].position->getY()));
			modelViews.push_back(tempMat);		
			isVisible.push_back(true);	
			objSmall.push_back(levels[currLevel - 1].at(i).objSpS[j]);
			SphereSmall* temp = &objSmall.back();
			temp->id = objIndices.size() - 1;
		}
	}
}

void PacanariDamaman::checkObjCollect(){	
	vector<int> tempToRemoveLarge, tempToRemoveSmall;
	for(int i = 0; i < objLarge.size(); i++){
		if(objLarge[i].collide(ball.position) && isVisible[objLarge[i].id]){
			totalScore += 100;
			tempToRemoveLarge.push_back(i);
			boost++;
		}
	}
	for(int i = 0; i < objSmall.size(); i++){
		if(objSmall[i].collide(ball.position) && isVisible[objSmall[i].id]){
			totalScore += 10;
			tempToRemoveSmall.push_back(i);
		}
	}

	for(int i = 0; i < tempToRemoveLarge.size(); i++){
	//		modelViews.erase(modelViews.begin() + objLarge[tempToRemoveLarge[i]].id);
	//		objIndices.erase(objLarge[tempToRemoveLarge[i]].id);
	//		objLarge.erase(objLarge.begin() + tempToRemoveLarge[i]);
			isVisible[objLarge[tempToRemoveLarge[i]].id] = false;
	}
	for(int i = 0; i < tempToRemoveSmall.size(); i++){
	//		modelViews.erase(modelViews.begin() + objSmall[tempToRemoveSmall[i]].id);
	//		objIndices.erase(objSmall[tempToRemoveSmall[i]].id);
	//		objSmall.erase(objSmall.begin() + tempToRemoveSmall[i]);
			isVisible[objSmall[tempToRemoveSmall[i]].id] = false;
	}
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

	checkObjCollect();

	//Check to see if we're in the cup
	if(checkWinCondition() || nextLevelCheat){
		objIndices.clear();
		modelViews.clear();
		objIndices[0] = make_pair(0, 0);
		objIndices[1] = make_pair(1, 1);
		objIndices[2] = make_pair(2, 2);
		objIndices[3] = make_pair(3, 3);
		objIndices[4] = make_pair(4, 4);
		objIndices[5] = make_pair(5, 4);
		objIndices[6] = make_pair(6, 4);
		objIndices[7] = make_pair(7, 4);
		setDefaultModelView(defaultView);
		playNextLevel();
		nextLevelCheat = false;
		return false;
	}

	Vector3* normal = currTile->tileNorm;
	float tempAngle = arrowAngle * 3.1415926/180;
	if(leftMouseDown){
		Vector3* velToAdd = new Vector3(cos(tempAngle), 0, sin(tempAngle));
		velToAdd = velToAdd->normalize();
		velToAdd = velToAdd->multiply(BALL_ACCEL);
		ball.velocity = physMan->applyForces(ball.velocity->add(velToAdd), currTile->slope, currTile->downVec);
	} else {
		ball.velocity = physMan->applyForces(ball.velocity, currTile->slope, currTile->downVec);
	}

	if(ball.velocity->getLength() > MAX_BALL_SPEED){
		ball.velocity->normalize();
		ball.velocity->multiply(MAX_BALL_SPEED);
	}

	//Apply physicsManager forces
	//std::cout << "Down vector is (" << curr.down->getX() << ", " << curr.down->getY() << ", " <<curr.down->getZ() << ")" <<std::endl;
	modelViews[0] = glm::translate(defaultView, glm::vec3(ball.position->getX(), ball.position->getZ(), ball.position->getY()));
	updatePacanariDamaman();

	//if ((ball.velocity->getLength() < MIN_BALL_SPEED) && currTile->slope == 0) { 
	//	ballMoving = false;
	//	ball.velocity = new Vector3(0.0, 0.0, 0.0);
	//} else ballMoving = true;

	return true;
}

void PacanariDamaman::updatePacanariDamaman(){
	
	
	modelViews[2] = modelViews[0];
	modelViews[2] = glm::scale(modelViews[2], glm::vec3(PACANARI_SCALE, PACANARI_SCALE, PACANARI_SCALE));
	modelViews[2] = glm::rotate(modelViews[2], arrowAngle - 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	modelViews[2] = glm::translate(modelViews[2], glm::vec3(0, 0, 0.15));
	modelViews[3] = modelViews[2];
	modelViews[4] = modelViews[5] = modelViews[6] = modelViews[7] = modelViews[2];
	modelViews[4] = glm::translate(modelViews[2], glm::vec3(0.07, 0.0, 0.07));
	modelViews[5] = glm::translate(modelViews[2], glm::vec3(-0.07, 0.0, 0.07));
	modelViews[6] = glm::translate(modelViews[2], glm::vec3(0.07, 0.0, 0.24));
	modelViews[7] = glm::translate(modelViews[2], glm::vec3(-0.07, 0.0, 0.24));
	
	ballRotSpeed = ball.velocity->getLength() * 10.0f;

	if(ballRot > 360) ballRot = 0;
	ballRot += ballRotSpeed;
	float rotAngle = ballRot * 3.1415926 / 180;
	modelViews[0] = glm::rotate(modelViews[0], rotAngle, glm::vec3(cos(arrowAngle), 0, sin(arrowAngle)));

	if(leftMouseDown){
		if(swingDir){
			if(rightAngle < LIMB_MAX_ANGLE){
				rightAngle += LIMB_SWING_SPEED;
				leftAngle -= LIMB_SWING_SPEED;
			} else {
				swingDir = !swingDir;
			}
		} else {
			if(rightAngle > LIMB_MIN_ANGLE){
				rightAngle -= LIMB_SWING_SPEED;
				leftAngle += LIMB_SWING_SPEED;
			} else swingDir = !swingDir;
		}
	}

	modelViews[4] = glm::rotate(modelViews[4], rightAngle, glm::vec3(1.0, 0, 0));
	modelViews[5] = glm::rotate(modelViews[5], leftAngle, glm::vec3(1.0, 0, 0));
	modelViews[6] = glm::rotate(modelViews[6], leftAngle, glm::vec3(1.0, 0, 0));
	modelViews[7] = glm::rotate(modelViews[7], rightAngle, glm::vec3(1.0, 0, 0));

}

bool PacanariDamaman::checkWinCondition(){	
	float x = abs(ball.position->getX() - cup.x);
	float z = abs(ball.position->getZ() - cup.z);

	if (x < 0.12 && z < 0.12) {
		return true;
	}
	return false;
}

void PacanariDamaman::buildGameLevel(int level){
	ball.Initialize(tees[level].x, tees[level].y, tees[level].z);
	cup.setPos(cups[level].x, cups[level].y, cups[level].z);
	modelViews[1] = glm::translate(defaultView,glm::vec3(cup.x, cup.z, cup.y));
	camManager->setCamPos(2, 2, 2);
	buildTileObjects();
}

void PacanariDamaman::keyboard(unsigned char key, int x, int y){
    /*switch (key) {
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
    }*/
	switch (key) {
    	case 27: exit(0); break;
		case 97: arrowAngle+=ARROW_TURN_SPEED; break;
		case 98: modelViews[3] = glm::rotate(modelViews[3], arrowAngle, glm::vec3(0.0f, 0.0f, 1.0f)); break;
    	case 99: 
			if (camManager->getCamSetting() == Camera::FREE){  camManager->setCamSetting(Camera::TOP_DOWN); }
			else camManager->setCamSetting(Camera::FREE);
			break;
		case 100: arrowAngle-=ARROW_TURN_SPEED; break;
		case 101: arrowAngle-=ARROW_TURN_SPEED; break;
		case 102: playNextLevel(); nextLevelCheat = true; break;
		case 113: arrowAngle+=ARROW_TURN_SPEED; break;
    	case 114: buildGameLevel(currLevel - 1); break;
		case 115: camManager->moveCam(Camera::dirBack); break;
		//case 119: if(ball.velocity->getLength() < MAX_BALL_SPEED) ball.addVelocity(BALL_ACCEL, arrowAngle); break;
		case 119: if(boost)ball.launchBall(MAX_BALL_SPEED, arrowAngle); break;
    default: break;
	}
}

void PacanariDamaman::mouse(int button, int state, int x, int y){
	switch (button) {
		case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
			 leftMouseDown = true;
			 glutPostRedisplay();
         }
         else if (state == GLUT_UP) {
			 leftMouseDown = false;
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