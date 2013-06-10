#pragma once

#include "../GL/glew.h"
#include "../GL/freeglut.h"
#include "../GL/glui.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/matrix_access.hpp"

#include <vector>
#include "ResourceManager.h"
#include "Camera.h"
#include "HUD.h"

using namespace std;

class GameObject
{
public:
	GameObject(void);
	~GameObject(void);
			
	vector<float> getLevelVerts(){return levelVerts;}
	vector<float> getLevelNorms(){return levelNorms;}
	vector<float> getLevelColor(){return levelColor;}
	vector<float> getWallVerts(){return wallVerts;}
	vector<float> getWallNorms(){return wallNorms;}
	vector<float> getWallColor(){return wallColor;}

	vector<glm::mat4> modelViews;
	map<int, pair<int, int>> objIndices;
	vector<vector<float>> objVerts, objNorms, objColor;

	virtual bool Update() = 0;
	virtual void GameInit() = 0;
	void Initialize(map<string, vector<float>>* objData, vector<vector<Tile>> levelVecs,
				vector<Cup> cupVec, vector<Tee> teeVec, Physics* physManager, glm::mat4 mView,
				ResourceManager* resManager);
	bool playNextLevel();
	
	virtual void keyboard(unsigned char key, int x, int y) = 0;
	virtual void mouse(int button, int state, int x, int y) = 0;
	virtual void mouseMove(int x, int y) = 0;

	glm::vec3 getCamPos();
	glm::vec3 getCamTarg();
	glm::vec3 getCamUP();

protected:
	Camera* camManager;
	map<string, vector<float>>* gameObjectData;
	int currLevel;
	ResourceManager* resMan;
	Physics* physMan;
	vector<Cup> cups;
	vector<Tee> tees;
	vector<float> levelVerts;
	vector<float> levelNorms;
	vector<float> levelColor;
	vector<float> wallVerts;
	vector<float> wallNorms;
	vector<float> wallColor;
	
	glm::mat4 defaultView;
	vector<vector<Tile>> levels;

	void setDefaultModelView(glm::mat4 mView);
	void buildLevel(int level);
	virtual void buildGameLevel(int level) = 0;
	void clearVecs();

	Tile* getTile(Vector3* pos, Physics* physics, int level);

};

