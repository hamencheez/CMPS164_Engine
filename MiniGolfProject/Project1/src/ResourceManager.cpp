
#include "ResourceManager.h"


using namespace std;

ResourceManager::ResourceManager() {
	levels = vector<vector<Tile>>();
	cups = vector<Cup>();
	tees = vector<Tee>();
	levelPar = vector<int>();
}


void ResourceManager::addTile(Tile &newTile, int levelInd) {
	levels[levelInd].push_back(newTile);
}

vector<float> ResourceManager::getObjVector(string ID){
	return objData[ID];
}

vector<vector<Tile>> ResourceManager::getLevels(){
	return levels;
}

void ResourceManager::readCourseFile(char* filename) {	
	FILE * f;
	
	string courseName;
	char word[80];
	int levelInd, tileInd, numVerts, neighbor;
	int par, numberHoles;
	float x, y, z;

	levelInd = 0;

	f = fopen (filename, "r");

	if (f!=NULL)
	 {
	  while(fscanf(f, "%s", &word) > 0){
		  if(strcmp(word, "begin_hole") == 0){
			  levels.push_back(vector<Tile>());
		  }
		  else if(strcmp(word, "end_hole") == 0){
			  levelInd++;
		  }
		  else if(strcmp(word, "par") == 0){
			  fscanf(f, "%d", &par);
			  levelPar.push_back(par);
		  }
		  else if(strcmp(word, "name") == 0){
			  fgets(word, 80, f);			  
			  levelNames.push_back(word);
		  }
		  else if(strcmp(word, "course") == 0){
			  fscanf(f, "\"%[^\"]\" %d", &word, &numberHoles);
			  fgets(word, 80, f);			  
		  }
		  else if(strcmp(word, "tile") == 0){
			  fscanf(f, "%d %d ", &tileInd, &numVerts);
			  Tile newTile = Tile(tileInd, numVerts);
			  for(int i = 0; i < numVerts; i++){
				  fscanf(f, "%f %f %f", &x, &y, &z);
				  newTile.addVert(x, y, z);
			  }
			  for(int i = 0; i < numVerts; i++){
				  fscanf(f, "%d", &neighbor);
				  newTile.addNeighbor(neighbor);
			  }
			  addTile(newTile, levelInd);
		  } else if(strcmp(word, "cup") == 0){
			  fscanf(f, "%d %f %f %f", &tileInd, &x, &y, &z);
			  Cup newCup = Cup(x, y, z);
			  cups.push_back(newCup);
		  } else if(strcmp(word, "tee") == 0){
			  fscanf(f, "%d %f %f %f", &tileInd, &x, &y, &z);
			  Tee newTee = Tee(x, y, z);
			  tees.push_back(newTee);
		  } else{
			  cout << "Do not recognize tile type" << endl;
			  exit(-1);
		  }
	  }
    fclose (f);
	}
}

void ResourceManager::readObjFile(char* filename){
	vector<vector<float>> rawVerts, rawNorms;
	rawVerts = vector<vector<float>>();
	Vector3 vec1;
	Vector3 vec2;
	Vector3 vec3;
	Vector3* res;
	FILE * f;

	bool ball, cup, norms;
	char word[80];
	float w, x, y, z;
	float colorX, colorY, colorZ, colorA;
	int ind1, ind2, ind3;
	int nind1, nind2, nind3;
	string vecID, vecIDVert, vecIDNorm, vecIDColor;	

	int lineNumVerts, lineNumNorms;
	lineNumVerts = lineNumNorms = 0;
	f = fopen (filename, "r");	

	if (f!=NULL)
	{
		while(fscanf(f, "%s", &word) > 0){
			if(strcmp(word, "v") == 0){
				rawVerts.push_back(vector<float>());
				fscanf(f, "%f %f %f", &x, &y, &z);
				x = x*4/100;
				y = y*4/100;
				z = z*4/100;
				rawVerts.at(lineNumVerts).push_back(x);
				rawVerts.at(lineNumVerts).push_back(z);
				rawVerts.at(lineNumVerts).push_back(y);
				lineNumVerts++;
			} else if(strcmp(word, "vn") == 0){
				rawNorms.push_back(vector<float>());
				fscanf(f, "%f %f %f", &x, &y, &z);
				rawNorms.at(lineNumNorms).push_back(x);
				rawNorms.at(lineNumNorms).push_back(z);
				rawNorms.at(lineNumNorms).push_back(y);
				lineNumNorms++;
			} else if(strcmp(word, "f") == 0){
				fscanf(f, "%d %d %d %d %d %d", &ind1, &nind1, &ind2, &nind2, &ind3, &nind3);
				objData[vecIDVert].push_back(rawVerts[ind1-1].at(0));
				objData[vecIDVert].push_back(rawVerts[ind1-1].at(1));
				objData[vecIDVert].push_back(rawVerts[ind1-1].at(2));
				objData[vecIDVert].push_back(rawVerts[ind2-1].at(0));
				objData[vecIDVert].push_back(rawVerts[ind2-1].at(1));
				objData[vecIDVert].push_back(rawVerts[ind2-1].at(2));
				objData[vecIDVert].push_back(rawVerts[ind3-1].at(0));
				objData[vecIDVert].push_back(rawVerts[ind3-1].at(1));
				objData[vecIDVert].push_back(rawVerts[ind3-1].at(2));
				
				objData[vecIDNorm].push_back(rawNorms[nind1-1].at(0));
				objData[vecIDNorm].push_back(rawNorms[nind1-1].at(1));
				objData[vecIDNorm].push_back(rawNorms[nind1-1].at(2));
				objData[vecIDNorm].push_back(rawNorms[nind2-1].at(0));
				objData[vecIDNorm].push_back(rawNorms[nind2-1].at(1));
				objData[vecIDNorm].push_back(rawNorms[nind2-1].at(2));
				objData[vecIDNorm].push_back(rawNorms[nind3-1].at(0));
				objData[vecIDNorm].push_back(rawNorms[nind3-1].at(1));
				objData[vecIDNorm].push_back(rawNorms[nind3-1].at(2));

				for(int i = 0; i < 3; i++){
					objData[vecIDColor].push_back(colorX);
					objData[vecIDColor].push_back(colorY);
					objData[vecIDColor].push_back(colorZ);
					objData[vecIDColor].push_back(colorA);
				}
			} else {
				vecID = word;
				vecIDVert = vecIDNorm = vecIDColor = vecID;
				vecIDVert.append("Vert");
				vecIDNorm.append("Norm");
				vecIDColor.append("Color");
				objData[vecIDVert] = vector<float>();
				objData[vecIDNorm] = vector<float>();
				objData[vecIDColor] = vector<float>();
				rawVerts.clear();
				rawNorms.clear();
				lineNumVerts = lineNumNorms = 0;
				if(fscanf(f, "%f %f %f %f", &x, &y, &z, &w) == 4){
					colorX = x;
					colorY = y;
					colorZ = z;
					colorA = w;
				} else {
					cout << "invalid color" << endl;
				}
			}
		}
	}
	cout << "successfull obj file read" << endl;
    fclose (f);
}