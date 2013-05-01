/*
 *	File: main.cpp
 *	Description: Display a spirograph in 3D
 *	
 *	OpenGL base program by Nathaniel Cesario
 *	Modified by Eric Lipschutz
 */

#include "../GL/glew.h"
#include "../GL/freeglut.h"
#include "../GL/glui.h"
//#include <GL/gl.h>

#define GLM_SWIZZLE
#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/matrix_access.hpp"

#include <cmath>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;




/**
 * Simple class for keeping track of shader program and vertex attribute
 * locations.
 */
class Shader {
public:
    Shader(string vertFile, string fragFile) { fromFiles(vertFile, fragFile); }

    /**
     * Creates a shader program based on vertex and fragment source.
     *
     * @param vertFile Path to vertex source
     * @param fragFile Path to fragment source
     */
    void fromFiles(string vertFile, string fragFile) {
        //These are shader objects containing the shader source code
        GLint vSource = setShaderSource(vertFile, GL_VERTEX_SHADER);
        GLint fSource = setShaderSource(fragFile, GL_FRAGMENT_SHADER);

        //Create a new shader program
        program = glCreateProgram();

        //Compile the source code for each shader and attach it to the program.
        glCompileShader(vSource);
        printLog("vertex compile log: ", vSource);
        glAttachShader(program, vSource);

        glCompileShader(fSource);
        printLog("fragment compile log: ", fSource);
        glAttachShader(program, fSource);

        //we could attach more shaders, such as a geometry or tessellation
        //shader here.

        //link all of the attached shader objects
        glLinkProgram(program);
    }

    /**
     * Helper method for reading in the source for a shader and creating a
     * shader object.
     *
     * @param file Filename of shader source
     * @param type Type of shader-> Only GL_VERTEX_SHADER and GL_FRAGMENT_SHADER
     *   are supported here.
     */
    GLint setShaderSource(string file, GLenum type) {
        //read source code
        ifstream fin(file.c_str());
        if (fin.fail()) {
            cerr << "Could not open " << file << " for reading" << endl;
            return -1;
        }
        fin.seekg(0, ios::end);
        int count  = fin.tellg();
        char *data = NULL;
        if (count > 0) {
            fin.seekg(ios::beg);
            data = new char[count+1];
            fin.read(data,count);
            data[count] = '\0';
        }
        fin.close();

        //create the shader
        GLint s = glCreateShader(type);
        glShaderSource(s, 1, const_cast<const char **>(&data), NULL);
        delete [] data;
        return s;
    }

    /**
     * Helper function used for debugging.
     */
    void printLog(std::string label, GLint obj) {
        int infologLength = 0;
        int maxLength = 256;

        if(glIsShader(obj)) {
            glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
        } else {
            glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
        }

        char infoLog[256];

        if (glIsShader(obj)) {
            glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
        } else {
            glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);
        }

        if (infologLength > 0) {
            cerr << label << infoLog << endl;
        }
    }

    GLint program; //shader program
    GLint modelViewLoc; //location of the modelview matrix in the program (M)
    GLint projectionLoc; //location of the projection matrix in the program (P)
    GLint normalMatrixLoc; //location of the normal matrix in the program (M_n)
    GLint vertexLoc, normalLoc; //vertex attribute locations (pos and norm)
      //respectively
    GLint timeLoc; //location of time variable
    GLuint vertexBuffer, normalBuffer; //used to keep track of GL buffer objects
};
Shader *shader = NULL;

int WIN_WIDTH = 512, WIN_HEIGHT = 512; //window width/height
glm::mat4 modelView, projection, camera; //matrices for shaders
float animTime = 0.0f, deltaT = 0.0001; //variables for animation

int main_window;

float r = 0.01f;
float R = 1.0f;
float p = 1.0f;
float s = 0.002f;
float t = 0.0f;
int steps = 5000;
float fov = 45.0f;
float eyeZ = 6.0f;
float rotSpeed = 1.0f;

float points [16][3] = {
		{-.5, 0, 1},
		{.5, 0, 1},
		{.5, 0, 0},
		{-.5, 0, 0},//first tile, top
		{-1.5, 0, 0},
		{-.5, 0, 0},
		{-.5, 0, -1},
		{-1.5, 0, -1},//second tile, top
		{-.5, 0, 0},
		{.5, 0, 0},
		{.5, 0, -1},
		{-.5, 0, -1},//third tile, top
		{.5, 0, 0},
		{1.5, 0, 0},
		{1.5, 0, -1},
		{.5, 0, -1},//fourth tile, top

	};
	float side [4][4] = {
		{1, 1, 0, 1},
		{1, 0, 1, 1},
		{0, 0, 1, 0},
		{1, 1, 1, 0},
	};

	float tee [3] = {0.0f, 0.0f,0.5f};
	float hole [3] = {-1.0f, 0.0f,-0.5f};


float golfGreen[3] = {0.0f, .4f, 0.0f};
float tanColor[3] = {.2f, .2f, 0.1f};
float white[3] = {1.0f, 1.0f, 1.0f};
float grey[3] = {.1f, .1f, .1f};

vector<float> axisVerts; //vertex array
vector<float> axisNorms; //normal array
size_t numAxisVerts; //number of total vertices

vector<float> verts; //vertex array
vector<float> norms; //normal array
size_t numVerts; //number of total vertices




//Draws the spirograph
void drawSpirograph(){
	axisVerts.clear();
	axisNorms.clear();

	verts.clear();
	norms.clear();

	int numTiles = 4; //the number of tiles we have to render



	//float maxRadius = abs(R)+abs(r)+abs(p);
	//float minWinDim = (float)min(WIN_WIDTH, WIN_HEIGHT);
	//float scaleFactor = 1.0f/maxRadius;

	//eyeZ = (3.0*maxRadius)/tan(fov*PI/180.0f); //Set the eye z position to make sure the entire image is visible

	//float rotStep = 0.5;
	//float x, y, z = 0;
	//float rot = 0;

	//The normals are used to define draw color since I don't need them for lighting.

	//Axis
	//float axisLength = 3.0f;
	
	for (int x = 0; x < numTiles; x++)
	{
		//Tiles
		verts.push_back(points[(4*x)][0]);
		verts.push_back(points[(4*x)][1]);
		verts.push_back(points[(4*x)][2]);
		norms.push_back(golfGreen[0]);norms.push_back(golfGreen[1]);norms.push_back(golfGreen[2]);

		verts.push_back(points[(4*x)+1][0]);
		verts.push_back(points[(4*x)+1][1]);
		verts.push_back(points[(4*x)+1][2]);
		norms.push_back(golfGreen[0]);norms.push_back(golfGreen[1]);norms.push_back(golfGreen[2]);

		verts.push_back(points[(4*x)+2][0]);
		verts.push_back(points[(4*x)+2][1]);
		verts.push_back(points[(4*x)+2][2]);
		norms.push_back(golfGreen[0]);norms.push_back(golfGreen[1]);norms.push_back(golfGreen[2]);

		verts.push_back(points[(4*x)+3][0]);
		verts.push_back(points[(4*x)+3][1]);
		verts.push_back(points[(4*x)+3][2]);
		norms.push_back(golfGreen[0]);norms.push_back(golfGreen[1]);norms.push_back(golfGreen[2]);
	
		//Draw Sides
		if (side[x][0] == 1) 
		{  
			//side 1, part 1
		 	verts.push_back((points[4*x][0])+.1);
			verts.push_back((points[4*x][1])+.2);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
				verts.push_back((points[4*x+1][0])+.1);
			verts.push_back((points[4*x+1][1])+.2);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
						
			verts.push_back((points[4*x+1][0])+.1);
			verts.push_back(points[4*x+1][1]);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x][0])+.1);				verts.push_back(points[4*x][1]);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
			
			//side 1, part 2
			verts.push_back((points[4*x][0])+.1);
			verts.push_back((points[4*x][1])+.2);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
				              
			verts.push_back((points[4*x+1][0])+.1);
			verts.push_back((points[4*x+1][1])+.2);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

		 	verts.push_back(points[4*x+1][0]);
			verts.push_back(points[4*x+1][1]);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x][0]);
			verts.push_back(points[4*x][1]);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);	
	
			//side 1, part 3
			verts.push_back((points[4*x][0])+.1);
			verts.push_back(points[4*x][1]);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x+1][0])+.1);
			verts.push_back(points[4*x+1][1]);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x+1][0]);
			verts.push_back(points[4*x+1][1]);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x][0]);
			verts.push_back(points[4*x][1]);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
		}

		if (side[x][1] == 1)
		{  
			//side 2, part 1
			verts.push_back((points[4*x+1][0])+.1);
			verts.push_back((points[4*x+1][1])+.2);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x+2][0])+.1);
			verts.push_back((points[4*x+2][1])+.2);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x+2][0])+.1);
			verts.push_back(points[4*x+2][1]);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x+1][0])+.1);
			verts.push_back(points[4*x+1][1]);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			//side 2, part 2
			verts.push_back((points[4*x+1][0])+.1);
			verts.push_back((points[4*x+1][1])+.2);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x+2][0])+.1);
			verts.push_back((points[4*x+2][1])+.2);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x+2][0]);
			verts.push_back(points[4*x+2][1]);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x+1][0]);
			verts.push_back(points[4*x+1][1]);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			//side 2, part 3
			verts.push_back((points[4*x+1][0])+.1);
			verts.push_back(points[4*x+1][1]);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x+2][0])+.1);
			verts.push_back(points[4*x+2][1]);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x+2][0]);
			verts.push_back(points[4*x+2][1]);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x+1][0]);
			verts.push_back(points[4*x+1][1]);
			verts.push_back(points[4*x+1][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
		}
		if (side[x][2] == 1) 
		{  
			//side 3, part 1
			verts.push_back((points[4*x+2][0])+.1);
			verts.push_back((points[4*x+2][1])+.2);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x+3][0])+.1);
			verts.push_back((points[4*x+3][1])+.2);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x+3][0])+.1);
			verts.push_back(points[4*x+3][1]);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x+2][0])+.1);
			verts.push_back(points[4*x+2][1]);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			//side 3, part 2
			verts.push_back((points[4*x+2][0])+.1);
			verts.push_back((points[4*x+2][1])+.2);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
				
			verts.push_back((points[4*x+3][0])+.1);
			verts.push_back((points[4*x+3][1])+.2);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
				
			verts.push_back(points[4*x+3][0]);
			verts.push_back(points[4*x+3][1]);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
					
			verts.push_back(points[4*x+2][0]);
			verts.push_back(points[4*x+2][1]);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
					
			//side 3, part 3
			verts.push_back((points[4*x+2][0])+.1);
			verts.push_back(points[4*x+2][1]);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
				
			verts.push_back((points[4*x+3][0])+.1);
			verts.push_back(points[4*x+3][1]);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x+3][0]);
			verts.push_back(points[4*x+3][1]);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x+2][0]);
			verts.push_back(points[4*x+2][1]);
			verts.push_back(points[4*x+2][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
		}
		if (side[x][3] == 1) 
		{  
			//side 4, part 1
			verts.push_back((points[4*x+3][0])+.1);
			verts.push_back((points[4*x+3][1])+.2);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x][0])+.1);
			verts.push_back((points[4*x][1])+.2);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x][0])+.1);
			verts.push_back(points[4*x][1]);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x+3][0])+.1);
			verts.push_back(points[4*x+3][1]);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			//side 4, part 2
			verts.push_back((points[4*x+3][0])+.1);
			verts.push_back((points[4*x+3][1])+.2);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x][0])+.1);
			verts.push_back((points[4*x][1])+.2);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x][0]);
			verts.push_back(points[4*x][1]);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x+3][0]);
			verts.push_back(points[4*x+3][1]);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			//side 4, part 3
			verts.push_back((points[4*x+3][0])+.1);
			verts.push_back(points[4*x+3][1]);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back((points[4*x][0])+.1);
			verts.push_back(points[4*x][1]);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
					
			verts.push_back(points[4*x][0]);
			verts.push_back(points[4*x][1]);
			verts.push_back(points[4*x][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);

			verts.push_back(points[4*x+3][0]);
			verts.push_back(points[4*x+3][1]);
			verts.push_back(points[4*x+3][2]);
			norms.push_back(tanColor[0]);norms.push_back(tanColor[1]);norms.push_back(tanColor[2]);
		}
	}
		//draw Golfball
	/*	for (int detailx = 0; detailx < 12; detailx++) 
		{
			for (int detailz = 0; detailz < 12; detailz++) 
			{
				verts.push_back(tee[0]+((cos((detailx/6)*3.14159)))*2);
				verts.push_back(tee[1]+1.0);
				verts.push_back(tee[2]+((cos((detailz/6)*3.14159)))*2);
				norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);
				printf("x value: %f  \n", tee[0]+((cos((detailx/6)*3.14159)))*2);
				printf("z value: %f  \n", tee[2]+((cos((detailz/6)*3.14159)))*2);
			}
		}*/
		//top
		verts.push_back(tee[0]+.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]+.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]+.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]-.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]-.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]-.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]-.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]+.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		//side1
		verts.push_back(tee[0]+.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]+.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]+.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]-.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]+.05);
		verts.push_back(tee[1]);
		verts.push_back(tee[2]-.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]+.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]+.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		//side2
		verts.push_back(tee[0]+.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]-.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]-.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]-.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]-.05);
		verts.push_back(tee[1]);
		verts.push_back(tee[2]-.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]+.05);
		verts.push_back(tee[1]);
		verts.push_back(tee[2]-.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		//side3
		verts.push_back(tee[0]-.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]-.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]-.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]+.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]-.05);
		verts.push_back(tee[1]);
		verts.push_back(tee[2]+.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]-.05);
		verts.push_back(tee[1]);
		verts.push_back(tee[2]-.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		//side4
		verts.push_back(tee[0]-.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]+.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]+.05);
		verts.push_back(tee[1]+.1);
		verts.push_back(tee[2]+.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]+.05);
		verts.push_back(tee[1]);
		verts.push_back(tee[2]+.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);

		verts.push_back(tee[0]-.05);
		verts.push_back(tee[1]);
		verts.push_back(tee[2]+.05);
		norms.push_back(white[0]);norms.push_back(white[1]);norms.push_back(white[2]);
	

	//draw hole
		//top
		verts.push_back(hole[0]+.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]+.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]+.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]-.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]-.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]-.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]-.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]+.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		//side1
		verts.push_back(hole[0]+.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]+.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]+.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]-.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]+.05);
		verts.push_back(hole[1]-.1);
		verts.push_back(hole[2]-.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]+.05);
		verts.push_back(hole[1]-.1);
		verts.push_back(hole[2]+.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		//side2
		verts.push_back(hole[0]+.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]-.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]-.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]-.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]-.05);
		verts.push_back(hole[1]-.1);
		verts.push_back(hole[2]-.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]+.05);
		verts.push_back(hole[1]-.1);
		verts.push_back(hole[2]-.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		//side3
		verts.push_back(hole[0]-.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]-.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]-.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]+.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]-.05);
		verts.push_back(hole[1]-.1);
		verts.push_back(hole[2]+.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]-.05);
		verts.push_back(hole[1]-.1);
		verts.push_back(hole[2]-.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		//side4
		verts.push_back(hole[0]-.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]+.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]+.05);
		verts.push_back(hole[1]+.01);
		verts.push_back(hole[2]+.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]+.05);
		verts.push_back(hole[1]-.1);
		verts.push_back(hole[2]+.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);

		verts.push_back(hole[0]-.05);
		verts.push_back(hole[1]-.1);
		verts.push_back(hole[2]+.05);
		norms.push_back(grey[0]);norms.push_back(grey[1]);norms.push_back(grey[2]);
	

	numVerts = verts.size() / 3;
}

//[from sylvie] - code I added from opengltest to handle mouse movement
typedef enum {
  TRANSLATE,
  ROTATE_X,
  ROTATE_Y,
  ROTATE_Z
} mode;

int    btn[ 3 ] = { 0 };		// Current button state
mode   cur_mode = TRANSLATE;		// Current mouse mode
float  translate[ 3 ] = { 0 };		// Current translation values
float  rotater[ 3 ] = { 0 };		// Current rotation values
int    mouse_x, mouse_y;		// Current mouse position

//void  handle_menu(int);
void  handle_motion(int, int);
void  handle_mouse(int, int, int, int);

//void glutMouseFunc(void handle_mouse (int button, int state, int x, int y));

void handle_menu( int ID )

  //  This routine handles popup menu selections
  //
  //  ID:  Menu entry ID
{
	printf("<< handle_menu called >>");
  switch( ID ) {
  case 0:				// XY translation
    cur_mode = TRANSLATE;
    break;
  case 1:				// X rotation
    cur_mode = ROTATE_X;
    break;
  case 2:				// Y rotation
    cur_mode = ROTATE_Y;
    break;
  case 3:				// Z rotation
    cur_mode = ROTATE_Z;
    break;
  case 4:				// Quit
    exit( 0 );
  }
}					//

void handle_motion( int x, int y )

  //  This routine acts as a callback for GLUT mouse motion events
  //
  //  x, y:  Cursor position
{

	printf("handle_motion called  ");
  float	 x_ratchet;			// X ratchet value
  float	 y_ratchet;			// Y ratchet value


  if ( !btn[ 0 ] ) {			// Left button not depressed?
    return;
  }

  x_ratchet = glutGet( GLUT_WINDOW_WIDTH ) / 10.0;
  y_ratchet = glutGet( GLUT_WINDOW_HEIGHT ) / 10.0;

  //  Windows XP has y = 0 at top, GL has y = 0 at bottom, so reverse y

  y = glutGet( GLUT_WINDOW_HEIGHT ) - y;

  switch( cur_mode ) {
  case TRANSLATE:			// XY translation
    translate[ 0 ] += (float) ( x - mouse_x ) / x_ratchet;
    translate[ 1 ] += (float) ( y - mouse_y ) / y_ratchet;
    break;
  case ROTATE_X:			// X rotation
	  printf("x rotation");
    x_ratchet /= 10.0;
    rotater[ 0 ] += (float) ( x - mouse_x ) / x_ratchet;
    break;
  case ROTATE_Y:			// Y rotation
    x_ratchet /= 10.0;
    rotater[ 1 ] += (float) ( x - mouse_x ) / x_ratchet;
    break;
  case ROTATE_Z:			// Z rotation
    x_ratchet /= 10.0;
    rotater[ 2 ] += (float) ( x - mouse_x ) / x_ratchet;
    break;
  }

  mouse_x = x;				// Update cursor position
  mouse_y = y;
  //update();
  glutPostRedisplay();
}					// End routine handle_motion

void handle_mouse( int b, int s, int x, int y )

  //  This routine acts as a callback for GLUT mouse events
  //
  //  b:     Mouse button (left, middle, or right)
  //  s:     State (button down or button up)
  //  x, y:  Cursor position
{  
	printf ("handle_mouse called -- ");
  if ( s == GLUT_DOWN ) {		// Store button state if mouse down
    btn[ b ] = 1;
  } else {
    btn[ b ] = 0;
  }

  mouse_x = x;
  mouse_y = glutGet( GLUT_WINDOW_HEIGHT ) - y;

  //handle_motion(x, y);
}					// End routine handle_mouse


//[sylvie] end code I added

//updates values based on some change in time
void update() {
    //animTime += dt;

	//drawSpirograph();

	//glMatrixMode( GL_MODELVIEW );		// Setup model transformations
  //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

 // glPushMatrix();
  //glTranslatef( translate[ 0 ], translate[ 1 ], translate[ 2 ] );
  //glRotatef( rotater[ 0 ], 1, 0, 0 );
  //glRotatef( rotater[ 1 ], 0, 1, 0 );
  //glRotatef( rotater[ 2 ], 0, 0, 1 );

	modelView = glm::rotate(modelView, rotater[0], glm::vec3(1.0f, 0.0f, 0.0f));
	//modelView = glm::rotate(modelView, glm::float_t(45), glm::vec3(-1.0f, 0.0f, 0.0f));

	//camera = glm::lookAt(glm::vec3(0.0f,0.0f,5.0f), glm::vec3(0,0,0), glm::vec3(0,1,0));
	//camera = glm::lookAt(glm::vec3(5.0f,5.0f,5.0f), glm::vec3(0,0,0), glm::vec3(0,1,0));

	/*projection = glm::perspective(
            fov,
            glm::float_t(WIN_WIDTH) / glm::float_t(WIN_HEIGHT),
            glm::float_t(0.1),
            glm::float_t(1000.0)
    );*/
	//float aspect = glm::float_t(WIN_WIDTH) / glm::float_t(WIN_HEIGHT);
	//float dims = 5.0f;
	//projection = glm::ortho(-dims*aspect,dims*aspect,-dims,dims,-20.0f,20.0f);

	//modelView = glm::mat4();
   // modelView = glm::rotate(modelView, glm::float_t(45), glm::vec3(1,0,0));
	//modelView = glm::rotate(modelView, glm::float_t(rotSpeed), glm::vec3(0,1,0));


}


//reshape function for GLUT
void reshape(int w, int h) {
	GLUI_Master.auto_set_viewport();
    WIN_WIDTH = w;
    WIN_HEIGHT = h;
    /*projection = glm::perspective(
            fov,
            glm::float_t(WIN_WIDTH) / glm::float_t(WIN_HEIGHT),
            glm::float_t(0.1),
            glm::float_t(1000.0)
    );*/
	//projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,-10.0f,10.0f);
}



//display function for GLUT
void display() {
	//drawSpirograph();

	//GLUI_Master.auto_set_viewport();
    //glViewport(0,0,WIN_WIDTH,WIN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Setup the modelview matrix
//    Mat4x4F modelCam = camera * modelView;
    glm::mat4 modelCam = camera * modelView;

    //grab the normal matrix from the modelview matrix (upper 3x3 entries of
    //modelview).
    glm::mat3 normalMatrix(modelCam);
    normalMatrix = glm::inverse(normalMatrix);
    normalMatrix = glm::transpose(normalMatrix);

	//Set the current shader
    glUseProgram(shader->program);

    //Pass the matrices and animation time to GPU
    glUniformMatrix4fv(
            shader->modelViewLoc, //handle to variable in the shader program
            1, //how many matrices we want to send
            GL_FALSE, //transpose the matrix
            glm::value_ptr(modelCam) //a pointer to an array containing the entries for
              //the matrix
            );
    glUniformMatrix4fv(shader->projectionLoc, 1, GL_FALSE,
            glm::value_ptr(projection));
    glUniformMatrix3fv(shader->normalMatrixLoc, 1, GL_FALSE,
            glm::value_ptr(normalMatrix));
    glUniform1f(shader->timeLoc, animTime);

	//Draw tile

	//Re-upload data buffers
	glBindBuffer(GL_ARRAY_BUFFER, shader->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(),GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, shader->normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(float), norms.data(), GL_DYNAMIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, shader->vertexBuffer); //which buffer we want
      //to use
    glEnableVertexAttribArray(shader->vertexLoc); //enable the attribute
    glVertexAttribPointer(shader->vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //same procedure for the normal array
    glBindBuffer(GL_ARRAY_BUFFER, shader->normalBuffer);
    glEnableVertexAttribArray(shader->normalLoc);
    glVertexAttribPointer(shader->normalLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //draw the vertices/normals we just specified.
    glDrawArrays(GL_QUADS, 0, numVerts);

	//Draw axis

	//Re-upload data buffers
	glBindBuffer(GL_ARRAY_BUFFER, shader->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, axisVerts.size() * sizeof(float), axisVerts.data(),GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, shader->normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, axisNorms.size() * sizeof(float), axisNorms.data(), GL_DYNAMIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, shader->vertexBuffer); //which buffer we want
      //to use
    glEnableVertexAttribArray(shader->vertexLoc); //enable the attribute
    glVertexAttribPointer(shader->vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //same procedure for the normal array
    glBindBuffer(GL_ARRAY_BUFFER, shader->normalBuffer);
    glEnableVertexAttribArray(shader->normalLoc);
    glVertexAttribPointer(shader->normalLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //draw the vertices/normals we just specified.
    glDrawArrays(GL_LINES, 0, numAxisVerts);

    //update animation variables.
    //have time oscillate between 0.0 and 1.0.
   /* if ((animTime >= 0.5 && deltaT > 0.0) ||
            (animTime <= 0.0 && deltaT < 0.0)) {
        deltaT = -deltaT;
    }*/


    update();




    glutSwapBuffers();
}

//idle function for GLUT
void idle() {
	if(glutGetWindow() != main_window){
		glutSetWindow(main_window);
	}

    glutPostRedisplay();
}

//captures keyborad input for GLUT
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27:
        exit(0);
        break;
    }
}

//do some GLUT initialization
void setupGLUT() {
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    main_window = glutCreateWindow("Isometric Projection");

    glutReshapeFunc(reshape);

    glutDisplayFunc(display);

    glutKeyboardFunc(keyboard);

	GLUI_Master.set_glutKeyboardFunc(keyboard);
	//GLUI_Master.set_glutSpecialFunc(keyboard);
	GLUI_Master.set_glutMouseFunc(handle_mouse);
	glutMotionFunc( handle_motion );
	glutCreateMenu( handle_menu );	// Setup GLUT popup menu
  glutAddMenuEntry( "Translate", 0 );
  glutAddMenuEntry( "Rotate X", 1 );
  glutAddMenuEntry( "Rotate Y", 2 );
  glutAddMenuEntry( "Rotate Z", 3 );
  glutAddMenuEntry( "Quit", 4 );
  glutAttachMenu( GLUT_RIGHT_BUTTON );
  //end motion
	GLUI_Master.set_glutReshapeFunc(reshape);

    //glutIdleFunc(idle);

	//GLUI stuff
	/*GLUI *glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_LEFT);
	GLUI_Spinner *r1Spin = glui->add_spinner("Radius 1", GLUI_SPINNER_FLOAT, &r);
	r1Spin->set_float_limits(0.001f,500.0f);
	r1Spin->set_speed(0.0001f);
	glui->add_spinner("Radius 2", GLUI_SPINNER_FLOAT, &R)->set_speed(0.01f);;
	glui->add_spinner("Pen Position", GLUI_SPINNER_FLOAT, &p);//->set_speed(0.01f);;
	glui->add_spinner("Step Size", GLUI_SPINNER_FLOAT, &s)->set_speed(0.01f);
	glui->add_spinner("Steps", GLUI_SPINNER_INT, &steps);
	glui->add_spinner("Rotation Speed", GLUI_SPINNER_FLOAT, &rotSpeed)->set_speed(0.1f);

	glui->set_main_gfx_window(main_window);*/
	GLUI_Master.set_glutIdleFunc(idle);
	GLUI_Master.sync_live_all();
}

//initialize OpenGL background color and vertex/normal arrays
void setupGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	drawSpirograph();

    camera = glm::lookAt(glm::vec3(0.0f,2.0f,3.0f), glm::vec3(0,0,-1), glm::vec3(0,1,0));

    /*projection = glm::perspective(
            fov,
            glm::float_t(WIN_WIDTH) / glm::float_t(WIN_HEIGHT),
            glm::float_t(0.1),
            glm::float_t(1000.0)
    );*/
	projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,-10.0f,10.0f);
}

//setup the shader program
void setupShaders() {
    //create the shader program from a vertex and fragment shader
    shader = new Shader("../shaders/gles.vert", "../shaders/gles.frag");

    shader->modelViewLoc = glGetUniformLocation(shader->program, "M");
    shader->projectionLoc = glGetUniformLocation(shader->program, "P");
    shader->normalMatrixLoc = glGetUniformLocation(shader->program, "M_n");
    shader->timeLoc = glGetUniformLocation(shader->program, "time");

    shader->vertexLoc = glGetAttribLocation(shader->program, "pos");
    shader->normalLoc = glGetAttribLocation(shader->program, "norm");

    //Create buffers for the vertex and normal attribute arrays
    GLuint bufs[2];
    glGenBuffers(2, bufs);

    shader->vertexBuffer = bufs[0];
    shader->normalBuffer = bufs[1];

    glBindBuffer(GL_ARRAY_BUFFER, shader->vertexBuffer);
    glBufferData(
            GL_ARRAY_BUFFER, //what kind of buffer (an array)
            verts.size() * sizeof(float), //size of the buffer in bytes
            verts.data(), //pointer to data we want to fill the buffer with
            GL_DYNAMIC_DRAW //how we intend to use the buffer
            );

    glBindBuffer(GL_ARRAY_BUFFER, shader->normalBuffer);
    glBufferData(
            GL_ARRAY_BUFFER,
            norms.size() * sizeof(float),
            norms.data(),
            GL_DYNAMIC_DRAW
            );
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
	//
	 // glutDisplayFunc( display_obj );	// Setup GLUT callbacks
	//motion:
  //glutMotionFunc( handle_motion );
  //glutMouseFunc( handle_mouse );
  
//void glutMouseFunc (void * handle_mouse(int button, int state, int x, int y));


  

    setupGLUT();
    setupGL();

    glewInit();

    setupShaders();

    glutMainLoop();

    if (shader) delete shader;

    return 0;
}

