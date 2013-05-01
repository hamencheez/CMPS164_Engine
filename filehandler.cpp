#include "stdafx.h"
#include "FileHandler.h"

FileHandler::FileHandler(void)
{

}


FileHandler::~FileHandler(void)
{

}

vector<vector<int>> FileHandler::getNeighborIndices(){
    return neighbors;
}

vector<float> FileHandler::readLevel(const char *filename){

    int i, pnt, res, ind;
    int i1, i2, i3, i4, tileInd;
    float vertx, verty, vertz;
    char    c;
    bool hasTee;
    string tileType;
    FILE    *f;

    neighbors = vector<vector<int>>();

    f = fopen(filename, "r");
    if (!f)
    {
    	cout << "Can't read the file " <<  filename << endl;
    	exit(-1);
    }

    ind = 0;
    //cout << verts.size()/3 << " = num coords" << endl;
    //cout << numPolygons << " num poly" << endl;
    
    while(!feof(f));
    {
        if (fscanf(f, "%s ", &tileType) != 1)
        {
            cout << "Can not read tile type on line " << ind << " in file " << filename << endl;
            exit(-1);
        }
        else{
            if(fscanf(f, "%s ", &tileType) == 1){
                if(tileType.compare("tile") == 0){
                    for(int i = 0; i < 4; i++){
                        fscanf(f, "%d %f %f %f ", &tileInd, &vertx, &verty, &vertz); 
			            verts[tileInd].push_back(vector<float>());
                        verts[tileInd].push_back(vertx);
                        verts[tileInd].push_back(verty);
                        verts[tileInd].push_back(vertz);
                    }
                    fscanf(f, "%d %d %d %d", i1, i2, i3, i4);     

			        neighbors.push_back(vector<int>());
                    neighbors[tileInd].push_back(i1);
                    neighbors[tileInd].push_back(i2);
                    neighbors[tileInd].push_back(i3);
                    neighbors[tileInd].push_back(i4);
                }
                else if(tileType.compare("tee") == 0){
                    fscanf(f, "%d %f %f %f ", &tileInd, &vertx, &verty, &vertz); 
                    teeVerts.push_back(vertx);
                    teeVerts.push_back(verty);
                    teeVerts.push_back(vertz);
                    teeTileInd = tileInd;
                }
                else if(tileType.compare("cup") == 0){
                    fscanf(f, "%d %f %f %f ", &tileInd, &vertx, &verty, &vertz); 
                    cupVerts.push_back(vertx);
                    cupVerts.push_back(verty); 
                    cupVerts.push_back(vertz);
                    cupTileInd = tileInd;
                }
                else {
                    cout << "Do not recognize tile type on line " << ind << " in file " << filename << endl;
                    exit(-1);
                }
            }
        }
        ind++;
    }
    
    fclose(f);
    
};
