/*
 *	File: main.cpp

 */

//#include <GL/gl.h>

#define GLM_SWIZZLE

#include "GameObject.h"
#include "MiniGolf.h"
#include "PacanariDamaman.h"

#include <cmath>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

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
        glAttachShader(program, vSource);

        glCompileShader(fSource);
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
		cout << file << " loaded successfully" << endl;
        return s;
    }

    GLint program; //shader program
    GLint modelViewLoc; //location of the modelview matrix in the program (M)
    GLint projectionLoc; //location of the projection matrix in the program (P)
    GLint normalMatrixLoc; //location of the normal matrix in the program (M_n)
    GLint useLight, userColor; //uniform for blocking lighting for axes
    GLint vertexLoc, normalLoc; //vertex attribute locations (pos and norm)
      //respectively
    GLint colorLoc; //Model color
    GLint indexLoc;
    GLint timeLoc; 
    GLint lightPosLoc;
    GLint viewPosLoc;
    GLuint vertexBuffer, normalBuffer, colorBuffer, texBuffer, texArray; //used to keep track of GL buffer objects
    GLuint indexBuffer, axesBuffer, aNormalBuffer, aColorBuffer; //index, axes, and axes colors
};
Shader *shader = NULL;

int WIN_WIDTH = 1280, WIN_HEIGHT = 720; //window width/height
glm::mat4 modelView, modelViewBall, modelViewCup, modelViewArrow, projection, camera, defaultView, transMat; //matrices for shaders
glm::vec3 lightPos(0,0,1), viewPos(10,10,10);
float animTime = 0.0f, deltaT = 0.0001; //variables for animation

ResourceManager* resManager;
Camera* cameraManager;
Physics* physicsManager;
Ball* golfBall;
GameObject* currGame;
MiniGolf* golfGame;
PacanariDamaman* pacaGame;

vector<float> verts, norms, color, levelVerts, levelNorms, levelColor, wallVerts, wallNorms, wallColor;
vector<glm::mat4> modelViews;
vector<vector<float>> objVerts, objNorms, objColor;
map<int, pair<int, int>> objIndices;
vector<glm::vec3> objPositions;
vector<bool> objVisible;

bool xSelect, ySelect, zSelect, scaleSelect, cameraSelect;
bool mouseDownL, mouseDownR, bBox, smoothShade, freeLook, ballMoving;
int numCoords, numPolygons;
int downX, downY, upX, upY, currX, currY;
int oldX, oldY;
int oldState;

int strokeNum, totalScore;

float factorTrans = 0.05f;
float factorRot = .4f;
float factorScale = .01f;
float arrowAngle = 0.0f;
float toAddX;
float toAddY;

Tile currTile;
Tile oldTile;

glm::vec3 camTarg;
glm::vec3 camUP;
glm::vec3 camPos;
glm::vec3 ballPos;

int level;


void bindLevelVecs(){
	levelVerts = currGame->getLevelVerts();
	levelNorms = currGame->getLevelNorms();
	levelColor = currGame->getLevelColor();
	wallVerts  = currGame->getWallVerts();
	wallNorms  = currGame->getWallNorms();
	wallColor  = currGame->getWallColor();
	objIndices = currGame->objIndices;
	objVerts = currGame->objVerts;
	objNorms = currGame->objNorms;
	objColor = currGame->objColor;
	objPositions = currGame->objPositions;
}

void draw_string( float x, float y, float z, char *txt )
  //  Draws the text string at (x,y,z)
{
  glRasterPos3f( x, y, z );
  while( *txt != '\0' ) {
    glutBitmapCharacter( GLUT_BITMAP_9_BY_15, *txt );
    txt++;
  }
}	

//reshape function for GLUT
void reshape(int w, int h) {
    WIN_WIDTH = w;
    WIN_HEIGHT = h;
    projection = glm::perspective(
            glm::float_t(45),
            glm::float_t(WIN_WIDTH) / glm::float_t(WIN_HEIGHT),
            glm::float_t(0.1),
            glm::float_t(1000.0)
    );
}

/*
void resetBall(){
	golfBall->velocity->setPos(0, 0, 0);
	ballPos = glm::vec3(resManager->getTee(level).x, resManager->getTee(level).y, resManager->getTee(level).z);
}

void newLevel(){
	level++;
	totalScore += strokeNum;
	strokeNum = 0;
	if(resManager->playNextLevel(level)){
		golfBall->velocity = new Vector3(0, 0, 0);
		ballPos = glm::vec3(resManager->getTee(level).x, resManager->getTee(level).y, resManager->getTee(level).z);
		bindVectors();
	} else {
		cout << "YOU WIN!" << endl;
	}
}

void checkForBallInHole() {
	float x = abs(golfBall->position->getX() - resManager->getCup(level).x);
	float z = abs(golfBall->position->getZ() - resManager->getCup(level).z);

	if (x < 0.12 && z < 0.12) {
		std::cout <<"Level Completed: Strokes: " << strokeNum << " Total Score: "<< totalScore <<endl;
		newLevel();
	//	dataIO->readFile(database, resManager);
	}
}

void updateBall() {
	ballPos.x += golfBall->velocity->getX();
	ballPos.z += golfBall->velocity->getZ();
	//golfBall->setVelocity(0, 0, 0);	
	golfBall->aim = arrowAngle;

	//Find the current tile we are on
	currTile = resManager->getTile(golfBall->position, currTile, physicsManager, level);
	ballPos.y = physicsManager->getHeightOfBall(currTile, golfBall);
	//golfBall->setPos(ballPos.x, ballPos.y, ballPos.z);
	//std::cout <<"Height of ball is " << height <<std::endl;

	cameraManager->camTarg->setX(golfBall->position->getX());
	cameraManager->camTarg->setY(golfBall->position->getZ());
	cameraManager->camTarg->setZ(golfBall->position->getY());

	//Check to see if we need to bounce off a wall
	//physicsManager->checkForCollision(currTile, golfBall);

	//Check to see if we're in the cup
	checkForBallInHole();

	//Apply physicsManager forces
	Vector3* normal = currTile.tileNorm;
	golfBall->velocity = physicsManager->applyForces(golfBall->velocity, currTile.slope, currTile.downVec);
	
	//std::cout << "Down vector is (" << curr.down->getX() << ", " << curr.down->getY() << ", " <<curr.down->getZ() << ")" <<std::endl;
	modelViewBall = glm::translate(modelView, glm::vec3(ballPos.x, ballPos.z, ballPos.y));
	modelViewArrow = modelViewBall;
	modelViewArrow = glm::rotate(modelViewArrow, arrowAngle - 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		
	if ((golfBall->velocity->getLength() < MIN_BALL_SPEED) && currTile.slope == 0) { 
		ballMoving = false;
		golfBall->velocity = new Vector3(0.0, 0.0, 0.0);
	} else ballMoving = true;
	//draw_string(0.0, 0.0, 0.0, "scojfadofj");	

	//If the ball is not moving, we need to aim the ball
	//Calculate the angle between the mouse point and the ball position
	modelViewArrow = modelViewBall;
	if(mouseDownR){
		float angleOffset = -90.0f + toAddX * ANGLE_DRAG_SPEED;
		modelViewArrow = glm::rotate(modelViewArrow, arrowAngle + angleOffset, glm::vec3(0.0f, 0.0f, 1.0f));
	} else {
		modelViewArrow = glm::rotate(modelViewArrow, arrowAngle - 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	}	
}
*/


void drawObject(glm::mat4 modelViewTemp, vector<float> verts, vector<float> norms, vector<float> color, int useLight){
	glm::mat4 mat = camera * modelViewTemp;
    glUniformMatrix4fv(shader->modelViewLoc, 1, GL_FALSE, glm::value_ptr(mat));

    glUniform1i(shader->useLight, useLight);

    glBindBuffer(GL_ARRAY_BUFFER, shader->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW );

    glBindBuffer(GL_ARRAY_BUFFER, shader->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(float), norms.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, shader->colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(float), color.data(), GL_DYNAMIC_DRAW);
	
    //draw the vertices/normals we just specified.
    glDrawArrays(GL_TRIANGLES, 0, verts.size());
}

void checkMouse()
{
	//if(mouseDownL && !scaleSelect) translate();
	//else if(mouseDownL && scaleSelect) scale();
	//else if(mouseDownR) rotate();
	//if(!mouseDownL && !mouseDownR)	oldX = oldY = 0;
}

//display function for GLUT
void display() {
    glViewport(0,0,WIN_WIDTH,WIN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //checkMouse();
	//updateBall();

	if(!currGame->Update()) bindLevelVecs();
	modelViews = currGame->modelViews;
	objVisible = currGame->isVisible;
	
//	camPos = cameraManager->getCamPos();
//	camTarg = cameraManager->getCamTargPos();
    camera = glm::lookAt(currGame->getCamPos(), currGame->getCamTarg(), currGame->getCamUP());

    //Setup the modelview matrix
    //Mat4x4F modelCam = camera * modelView;
    glm::mat4 modelCam = camera * modelView;

    //grab the normal matrix from the modelview matrix (upper 3x3 entries of
    //modelview).
    glm::mat3 normalMatrix(modelCam);
    normalMatrix = glm::inverse(normalMatrix);
    normalMatrix = glm::transpose(normalMatrix);

    //Tell OpenGL which shader program we want to use. In this case, we are only
    //using one, but in general we might have many shader programs.
    glUseProgram(shader->program);

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
    glUniform3fv(shader->lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(shader->viewPosLoc, 1, glm::value_ptr(viewPos));

	////////////////////////////////////////////////////////////////////////////////////
	//Draw the tiles
	
    modelCam = camera * modelView;
    glUniformMatrix4fv(shader->modelViewLoc, 1, GL_FALSE, glm::value_ptr(modelCam));

    glUniform1i(shader->useLight, 1);

    glBindBuffer(GL_ARRAY_BUFFER, shader->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, levelVerts.size() * sizeof(float), 
				levelVerts.data(), GL_DYNAMIC_DRAW );
    glEnableVertexAttribArray(shader->vertexLoc);
    glVertexAttribPointer(shader->vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL );

    glBindBuffer(GL_ARRAY_BUFFER, shader->normalBuffer); 
	glBufferData(GL_ARRAY_BUFFER, levelNorms.size() * sizeof(float), 
			levelNorms.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(shader->normalLoc); 
    glVertexAttribPointer(shader->normalLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL );

    glBindBuffer(GL_ARRAY_BUFFER, shader->colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, levelColor.size() * sizeof(float),
                levelColor.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(shader->colorLoc);
    glVertexAttribPointer(shader->colorLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	
    //draw the vertices/normals we just specified.
    glDrawArrays(GL_TRIANGLES, 0, levelVerts.size());

	////////////////////////////////////////////////////////////////////////////////////
	//Draw the walls

    modelCam = camera * modelView;
    glUniformMatrix4fv(shader->modelViewLoc, 1, GL_FALSE, glm::value_ptr(modelCam));

    glUniform1i(shader->useLight, 1);
	glUniform4f(shader->userColor, 0.85f, 0.85f, 0.85f, 0.85f);

    glBindBuffer(GL_ARRAY_BUFFER, shader->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, wallVerts.size() * sizeof(float), 
				wallVerts.data(), GL_STATIC_DRAW );

    glBindBuffer(GL_ARRAY_BUFFER, shader->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, wallNorms.size() * sizeof(float), 
				wallNorms.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, shader->colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, wallColor.size() * sizeof(float),
				wallColor.data(), GL_STATIC_DRAW);
	
    //draw the vertices/normals we just specified.
    glDrawArrays(GL_QUADS, 0, wallVerts.size());
	
	/////////////////////////////////////////////////////////////////////////////////
	//Draw all game objects

	for(int i = 0; i < objIndices.size(); i++){
		if(objVisible[i]){
			drawObject(modelViews[objIndices[i].first],   objVerts[objIndices[i].second],
					 objNorms[objIndices[i].second],	objColor[objIndices[i].second], 1);
		}
	}
	
    glutSwapBuffers();
}

//idle function for GLUT
void idle() {
    glutPostRedisplay();
}

//captures keyboard input for GLUT
//Sets variables for doing transforms

void keyboard(unsigned char key, int x, int y) {
	currGame->keyboard(key, x, y);
	/*
    switch (key) {
    	case 27: exit(0); break;
		case 97: cameraManager->moveCam(Camera::dirLeft); break;
		case 98: modelViewArrow = glm::rotate(modelViewArrow, arrowAngle, glm::vec3(0.0f, 0.0f, 1.0f)); break;
    	case 99: 
			if (cameraManager->getCamSetting() == Camera::FREE){  cameraManager->setCamSetting(Camera::TOP_DOWN); }
			else cameraManager->setCamSetting(Camera::FREE);
			break;
		case 100: cameraManager->moveCam(Camera::dirRight); break;
		//case 101: arrowAngle-=ARROW_TURN_SPEED; break;
		//case 102: newLevel(); break;
		//case 113: arrowAngle+=ARROW_TURN_SPEED; break;
    	//case 114: resetBall(); break;
		case 115: cameraManager->moveCam(Camera::dirBack); break;
		case 119: cameraManager->moveCam(Camera::dirForward); break;
    	default: break;
    }*/
}

//Callback for mouse button events.
//Sets variables for position and button state.
void mouse(int button, int state, int x, int y)
{
	currGame->mouse(button, state, x, y);
	/*
    switch (button) {
		case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
			 mouseDownL = true;
			 if(!ballMoving){
				// golfBall->launchBall(MAX_BALL_SPEED);
				 strokeNum++;
			 }
			 glutPostRedisplay();
         }
         else if (state == GLUT_UP) {
			 mouseDownL = false;
			 glutPostRedisplay();
         }
         break;
      case GLUT_RIGHT_BUTTON:
         if (state == GLUT_DOWN) {
        	 mouseDownR = true;
			 if(oldState != state){
				 oldX = currX;
			 } //else {
				toAddX = currX - oldX;
			// }
			 //arrowPower += currY - oldY;
 			 glutPostRedisplay();
         }
         else if (state == GLUT_UP) {
 			 mouseDownR = false;
			 if(oldState != state){
				 arrowAngle += toAddX;
 				 //oldX = currX = oldY = currY = 0;
			 }
			 //arrowPower += currY - oldY;
 			 glutPostRedisplay();
         }
         break;
      default:
         break;
   }
	oldState = state;*/
   glutPostRedisplay();
}

//Callback for mouse movement while a button is down.
//Also applies transformations, as doing so in Display was causing the
//    transforms to slide, rather than just move.
void mouseMove(int x, int y){
	currGame->mouseMove(x, y);
	glutPostRedisplay();
}

//do some GLUT initialization
void setupGLUT() {
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow("3D Minigolf Program");

    glutReshapeFunc(reshape);

    glutDisplayFunc(display);

    glutKeyboardFunc(keyboard);

    glutMouseFunc(mouse);

    glutMotionFunc(mouseMove);

    glutIdleFunc(idle);
}


//initialize OpenGL background color and vertex/normal arrays
void setupGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
	
    projection = glm::perspective(
            glm::float_t(45),
            glm::float_t(WIN_WIDTH) / glm::float_t(WIN_HEIGHT),
            glm::float_t(0.1),
            glm::float_t(1000.0)
    );

    defaultView = modelView;
	modelViewBall = modelView;
	modelViewCup = modelView;
	modelViewArrow = modelView;

}

//setup the shader program
void setupShaders() {
    //create the shader program from a vertex and fragment shader
    shader = new Shader("../shaders/light.vert", "../shaders/light.frag");

    //Here's where we setup handles to each variable that is used in the shader
    //program. See the shader source code for more detail on what the difference
    //is between uniform and vertex attribute variables.
    shader->modelViewLoc = glGetUniformLocation(shader->program, "M");
    shader->projectionLoc = glGetUniformLocation(shader->program, "P");
    shader->normalMatrixLoc = glGetUniformLocation(shader->program, "M_n");
    shader->timeLoc = glGetUniformLocation(shader->program, "time");
    shader->lightPosLoc = glGetUniformLocation(shader->program, "L_p");
    shader->viewPosLoc = glGetUniformLocation(shader->program, "E");
    shader->useLight = glGetUniformLocation(shader->program, "useLight");
    shader->userColor = glGetUniformLocation(shader->program, "userColor");

    //notice that, since the vertex attribute norm is not used in the shader
    //program, shader->normalLoc = -1. If we access norm in the shader program,
    //then this value will be >= 0.
    shader->vertexLoc = glGetAttribLocation(shader->program, "pos");
    shader->normalLoc = glGetAttribLocation(shader->program, "norm");
    shader->colorLoc = glGetAttribLocation(shader->program, "color");

    //Create buffers for the vertex and normal attribute arrays
    GLuint bufs[8];
    glGenBuffers(8, bufs);

    shader->vertexBuffer = bufs[0];
    shader->normalBuffer = bufs[1];
    shader->colorBuffer = bufs[2];
    shader->axesBuffer = bufs[3];
    shader->aNormalBuffer = bufs[4];
    shader->aColorBuffer = bufs[5];
    shader->indexBuffer = bufs[6];

    //This is where we pass the vertex/normal data to the GPU.
    //In general, the procedure for working with buffers is:
    //  1. Tell OpenGL which buffer we're using (glBindBuffer)
    //  2. Tell OpenGL what to do with the buffer (e.g. fill buffer, use the
    //     in the buffer, etc).
    //
    //Here we are filling the buffers (glBufferData). The last parameter
    //(GL_STATIC_DRAW), says that our intention is to not change the values in
    //these buffers. If we were going to be modifying these positions frequently
    //at runtime, we might want to make this GL_DYNAMIC_DRAW instead. For right
    //now, it's not too important which you choose.

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

    glBindBuffer(GL_ARRAY_BUFFER, shader->colorBuffer);
    glBufferData(
            GL_ARRAY_BUFFER,
            color.size() * sizeof(float),
            color.data(),
            GL_DYNAMIC_DRAW
            );

}

int main(int argc, char **argv) {
	resManager = new ResourceManager();
	resManager->readCourseFile(argv[1]);
	resManager->readObjFile(argv[2]);
	//resManager->readObjFile(argv[3]);

	physicsManager = new Physics();

	golfGame = new MiniGolf();
	golfGame->Initialize(resManager->getObjData(), resManager->getLevels(), resManager->getCups(),
						resManager->getTees(), physicsManager, defaultView, resManager);
	currGame = golfGame;

	cameraManager = new Camera();

	bindLevelVecs();

	strokeNum = totalScore = 0;
	
    glutInit(&argc, argv);
    setupGLUT();
    setupGL();

	ballMoving = false;
    oldX = oldY = 0;
	level = 0;

    glewInit();

    setupShaders();

    glutMainLoop();

    if (shader) delete shader;

    return 0;
}

