//  ========================================================================
//  COSC363: Computer Graphics (2021);  University of Canterbury.
//
//  FILE NAME: TerrainPatches.cpp
//  See Lab-11.pdf for details.
//
//	The program generates and loads the mesh data for a terrain floor (100 verts, 81 elems).
//  Requires files  TerrainPatches.vert, TerrainPatches.frag
//                  TerrainPatches.cont, TerrainPatches.eval
//  ========================================================================
#define  GLM_FORCE_RADIANS
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "loadTGA.h"
#include <glm/gtc/type_ptr.hpp>
using namespace std;

//Globals
GLuint vaoID;
GLuint mvpMatrixLoc;
float CDR = 3.14159265/180.0;     //Conversion from degrees to rad (required in GLM 0.9.6)
float verts[100*3];       //10x10 grid (100 vertices)
GLushort elems[81*4];       //Element array for 81 quad patches
glm::mat4 projView;
glm::mat4 proj, view;   //Projection and view matrices

//Texture Globals
GLuint heightMap;
GLuint texID[2];

//Camera Globals
float speed = 2.0;
float camX = 0.0;
float camZ = 30.0;
float camY = 20.0;
glm::vec3 cameraPos   = glm::vec3(camX, camY, camZ);
glm::vec3 cameraFront = glm::vec3(0.0, 10.0, -40.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

//Generate vertex and element data for the terrain floor
void generateData()
{
	int indx, start;
	//verts array
	for(int i = 0; i < 10; i++)   //100 vertices on a 10x10 grid
	{
		for(int j = 0; j < 10; j++)
		{
			indx = 10*i + j;
			verts[3*indx] = 10*i - 45;		//x
			verts[3*indx+1] = 0;			//y
			verts[3*indx+2] = -10*j;		//z
		}
	}

	//elems array
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 9; j++)
		{
			indx = 9*i +j;
			start = 10*i + j;
			elems[4*indx] = start;
			elems[4*indx+1] = start+10;
			elems[4*indx+2] = start+11;
			elems[4*indx+3] = start+1;			
		}
	}
}

//Loads terrain texture
void loadTextures()
{

    glGenTextures(2, texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    loadTGA("./HeightMaps/MtRuapehu.tga");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texID[1]);
    loadTGA("./HeightMaps/MtCook.tga");


    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}


//Loads a shader file and returns the reference to a shader object
GLuint loadShader(GLenum shaderType, string filename)
{
	ifstream shaderFile(filename.c_str());
	if(!shaderFile.good()) cout << "Error opening shader file." << endl;
	stringstream shaderData;
	shaderData << shaderFile.rdbuf();
	shaderFile.close();
	string shaderStr = shaderData.str();
	const char* shaderTxt = shaderStr.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderTxt, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
		const char *strShaderType = NULL;
		cerr <<  "Compile failure in shader: " << strInfoLog << endl;
		delete[] strInfoLog;
	}
	return shader;
}

//Initialise the shader program, create and load buffer data
void initialise()
{
//	glm::mat4 proj, view;   //Projection and view matrices

	//--------Load terrain height map-----------
	loadTextures();

	//--------Load shaders----------------------
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "TerrainPatches.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "TerrainPatches.frag");
	GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "TerrainPatches.cont");
	GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "TerrainPatches.eval");

	//--------Attach shaders---------------------
	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
	glAttachShader(program, shaderc);
	glAttachShader(program, shadere);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	glUseProgram(program);

	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	heightMap = glGetUniformLocation(program, "heightMap");
	GLuint eyePosLoc = glGetUniformLocation(program, "eyePos");
	glUniform1i(heightMap, 0);

//--------Compute matrices----------------------
	proj = glm::perspective(30.0f*CDR, 1.25f, 20.0f, 500.0f);  //perspective projection matrix
	view = glm::lookAt(cameraPos, cameraFront, cameraUp); //view matrix
	projView = proj * view;  //Product matrix
	glUniform1f(eyePosLoc, 30.0 );

//---------Load buffer data-----------------------
	generateData();

	GLuint vboID[2];
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glBindVertexArray(0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

//Display function to compute uniform values based on transformation parameters and to draw the scene
void display()
{
    projView = proj * view;  //Product matrix
	glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &projView[0][0]);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawElements(GL_PATCHES, 81*4, GL_UNSIGNED_SHORT, NULL);
	glFlush();
}

void changeHeightMap(int texture){
    glUniform1i(heightMap, texture);
}

void onKeyPress(unsigned char key, int x, int y){
    switch (key) {
        case '1':
            changeHeightMap(0);
            break;
        case '2':
            changeHeightMap(1);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void calculateMove(float direction) {
    float lookX = cameraFront[0];
    float lookZ = cameraFront[2];
    float moveX = lookX - camX;
    float moveZ = lookZ - camZ;
    if (abs(moveX)>abs(moveZ)){
        moveZ /= abs(moveX);
        moveX /= abs(moveX);
    } else {
        moveX /= abs(moveZ);
        moveZ /= abs(moveZ);
    }
    camX += moveX * direction * speed;
    camZ += moveZ * direction * speed;
    cameraPos = glm::vec3 (camX,camY,camZ);
    cameraFront += glm::vec3 (moveX * direction * speed,0.0,moveZ * direction * speed);
    view = glm::lookAt(cameraPos, cameraFront, cameraUp);
//    view = glm::translate(view, glm::vec3(moveX * direction, 0.0, moveZ * direction));
}

void rotateCamera(float direction){
    float angle = 5*CDR;
    float yArray[16] = {cos(angle * direction),0,-sin(angle*direction), 0,0,1,0,0, sin(angle*direction), 0,cos(angle*direction),0,0,0,0,1};
    glm::mat4 yRotation= glm::make_mat4(yArray);
//    float yArray3x3[9] = {cos(angle * direction),0,sin(angle*direction),0,1,0, -sin(angle*direction), 0,cos(angle*direction)};
//    glm::mat3 yRotation3x3= glm::make_mat3(yArray3x3);
    view = view * yRotation;
//    cameraFront = yRotation3x3 * cameraFront;
}

void onSpecialKey(int key, int x, int y){
    switch(key){
        case GLUT_KEY_UP:
            calculateMove(1);
            break;
        case GLUT_KEY_DOWN:
            calculateMove(-1);
            break;
        case GLUT_KEY_LEFT:
            rotateCamera(-1);
            break;
        case GLUT_KEY_RIGHT:
            rotateCamera(1);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Terrain");
	glutInitContextVersion (4, 2);
	glutInitContextProfile ( GLUT_CORE_PROFILE );

	if(glewInit() == GLEW_OK)
	{
		cout << "GLEW initialization successful! " << endl;
		cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
	}
	else
	{
		cerr << "Unable to initialize GLEW  ...exiting." << endl;
		exit(EXIT_FAILURE);
	}

	initialise();
	glutKeyboardFunc(onKeyPress);
	glutDisplayFunc(display);
    glutSpecialFunc(onSpecialKey);
	glutMainLoop();
}

