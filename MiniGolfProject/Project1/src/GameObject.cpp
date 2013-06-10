#include "GameObject.h"

GameObject::GameObject(void)
{
}


GameObject::~GameObject(void)
{
}

void GameObject::Initialize(map<string, vector<float>>* objData, vector<vector<Tile>> levelVecs,
				vector<Cup> cupVec, vector<Tee> teeVec, Physics* physManager, glm::mat4 mView,
				ResourceManager* resManager)
{
	gameObjectData = objData;
	levels = levelVecs;
	cups = cupVec;
	tees = teeVec;
	resMan = resManager;
	physManager = physMan;
	currLevel = 1;
	GameInit();
	buildLevel(currLevel - 1);
	defaultView = mView;
	defaultPos = glm::vec3(0, 0, 0);
}

void GameObject::setDefaultModelView(glm::mat4 mView){
	for(int i = 0; i < objIndices.size(); i++){
		modelViews.push_back(mView);
		isVisible.push_back(true);
	}
}

bool GameObject::playNextLevel(){
	clearVecs();
	currLevel++;
	if(currLevel < levels.size() - 1){
		buildLevel(currLevel - 1);
		return true;
	} else {
		return false;
	}
}

void GameObject::clearVecs() {
	levelVerts.clear();
	levelNorms.clear();
	levelColor.clear();
	wallVerts.clear();
	wallNorms.clear();
	wallColor.clear();
}

void GameObject::buildLevel(int levelInd){
	vector<float> activeVec = vector<float>();

	for(int i = 0; i < levels[levelInd].size(); i++){
		levels[levelInd].at(i).buildAll();
		activeVec = levels[levelInd].at(i).getTVerts();
		levelVerts.insert(levelVerts.end(), activeVec.begin(), activeVec.end());
		activeVec = levels[levelInd].at(i).getTNorms();
		levelNorms.insert(levelNorms.end(), activeVec.begin(), activeVec.end());
		activeVec = levels[levelInd].at(i).getTColor();
		levelColor.insert(levelColor.end(), activeVec.begin(), activeVec.end());
		activeVec = levels[levelInd].at(i).getWVerts();
		wallVerts.insert(wallVerts.end(), activeVec.begin(), activeVec.end());
		activeVec = levels[levelInd].at(i).getWNorms();
		wallNorms.insert(wallNorms.end(), activeVec.begin(), activeVec.end());
		activeVec = levels[levelInd].at(i).getWColor();
		wallColor.insert(wallColor.end(), activeVec.begin(), activeVec.end());
	}	
	buildGameLevel(levelInd);
}

Tile* GameObject::getTile(Vector3* pos, Physics* physics, int level) {
	float minY = 99999;
	float maxY = -99999;

	for (unsigned int itor = 0; itor < levels[level].size(); ++itor) {
		if (physics->pointContained(&levels[level].at(itor), pos)) {
		
			for (int i = 0; i < levels[level].at(itor).yVertex.size(); i++) {
				if (levels[level].at(itor).yVertex.at(i) < minY) minY = levels[level].at(itor).yVertex.at(i);
				if (levels[level].at(itor).yVertex.at(i) > maxY) maxY = levels[level].at(itor).yVertex.at(i);
			}
			if (pos->getY() >= (minY - 0.4)){
				if(pos->getY() <= (maxY + 0.4)) {
				return &levels[level].at(itor);
				}
			}
		}
	}

	for (unsigned int itor = 0; itor < levels[level].size(); ++itor) {
		if (physics->pointOnPlane(levels[level].at(itor).xVertex, levels[level].at(itor).zVertex, pos->getX(), pos->getZ())) {
		
				return &levels[level].at(itor);
		}
	}

	std::cout << "Tile not found" <<std::endl;
	
	//Return first tile
	return &levels[level].at(0);
}

glm::vec3 GameObject::getCamPos(){
	return camManager->getCamPos();
}

glm::vec3 GameObject::getCamTarg(){
	return camManager->getCamTarg();
}

glm::vec3 GameObject::getCamUP(){
	return glm::vec3(0.0f, 0.0f, 1.0f);
}