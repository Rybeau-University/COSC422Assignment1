//  ========================================================================
//  COSC363: Computer Graphics (2021);  University of Canterbury.
//
//  FILE NAME: TerrainPatches.cpp
//  See Lab-11.pdf for details.
//
//	The program generates and loads the mesh data for a terrain floor (100 verts, 81 elems).
//  Requires files  TerrainPatches.vert, TerrainPatches.frag
//                  TerrainPatches.cont.glsl, TerrainPatches.eval.glsl
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
GLuint eyePosLoc;
GLuint mvMatrixLoc, norMatrixLoc;
bool wireframe = false;
//Texture Globals
GLuint heightMap;
GLuint texID[5];
GLuint snowHeightLoc;
GLuint waterHeightLoc;
GLuint lgtLoc;
float currentWaterHeight = 2.0;
float currentSnowHeight = 6.0;
float lightX = 0;

//Camera Globals
float speed = 2.0;
float camX = 0.0;
float camZ = 30.0;
float camY = 20.0;
float angle = -90.0 * CDR;
glm::vec3 cameraPos   = glm::vec3(camX, camY, camZ);
glm::vec3 cameraFront = glm::vec3(camX+cos(angle), camY, camZ+sin(angle));
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

void loadHeightMaps(){
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

void loadTextures(){
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texID[2]);
    loadTGA("./Textures/grass.tga");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texID[3]);
    loadTGA("./Textures/water.tga");


    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texID[4]);
    loadTGA("./Textures/snow.tga");


    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

//Loads TGA files
void loadTGAs()
{
    glGenTextures(5, texID);
    loadHeightMaps();
    loadTextures();

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

void calculateMatrices(){
    fprintf(stderr, "Computing Matrices\n");
    glm::vec4 light = glm::vec4(lightX, 10.0, -50.0, 1.0);
    fprintf(stderr, "Light Vec X %f, Y %f, Z%f\n", light.x, light.y, light.z);
    glm::mat4 mvpMatrix = proj * view;   //The model-view-projection transformation
    glm::mat4 invMatrix = glm::inverse(view);  //Inverse of model-view matrix for normal transformation
    glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(norMatrixLoc, 1, GL_TRUE, &invMatrix[0][0]);
    glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &view[0][0]);
    glm::vec4 lightEye = view * light;
    fprintf(stderr, "Light Eye X %f, Y %f, Z%f\n", lightEye.x, lightEye.y, lightEye.z);
    glUniform3fv(lgtLoc, 1, &lightEye[0]);
}

//Initialise the shader program, create and load buffer data
void initialise()
{

//--------Textures-----------
	loadTGAs();
	//--------Load shaders----------------------
	GLuint shaderv = loadShader(GL_VERTEX_SHADER, "TerrainPatches.vert");
	GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "TerrainPatches.frag");
	GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "TerrainPatches.cont.glsl");
	GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "TerrainPatches.eval.glsl");
	GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "TerrainPatches.geom");

	//--------Attach shaders---------------------
	GLuint program = glCreateProgram();
	glAttachShader(program, shaderv);
	glAttachShader(program, shaderf);
	glAttachShader(program, shaderc);
	glAttachShader(program, shadere);
	glAttachShader(program, shaderg);

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
	eyePosLoc = glGetUniformLocation(program, "eyePos");
	mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
	mvMatrixLoc = glGetUniformLocation(program, "mvMatrix");
	norMatrixLoc = glGetUniformLocation(program, "norMatrix");
	heightMap = glGetUniformLocation(program, "heightMap");
	lgtLoc = glGetUniformLocation(program, "lightPos");
	glUniform1i(heightMap, 0);

//--------Compute matrices----------------------
	proj = glm::perspective(30.0f*CDR, 1.25f, 20.0f, 500.0f);  //perspective projection matrix
	view = glm::lookAt(cameraPos, cameraFront, cameraUp); //view matrix
	projView = proj * view;  //Product matrix
	glUniform3fv(eyePosLoc, 1, glm::value_ptr(cameraPos));

// Setup texture samplers
    GLuint grassLoc = glGetUniformLocation(program, "grassSampler");
    glUniform1i(grassLoc, 2);
    GLuint waterLoc = glGetUniformLocation(program, "waterSampler");
    glUniform1i(waterLoc, 3);
    GLuint snowLoc = glGetUniformLocation(program, "snowSampler");
    glUniform1i(snowLoc, 4);
    snowHeightLoc = glGetUniformLocation(program, "snowHeight");
    glUniform1f(snowHeightLoc, currentSnowHeight);
    waterHeightLoc = glGetUniformLocation(program, "waterHeight");
    glUniform1f(waterHeightLoc, currentWaterHeight);
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

//Display function to compute uniform values based on transformation parameters and to draw the scene
void display()
{
    calculateMatrices();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vaoID);
	glDrawElements(GL_PATCHES, 81*4, GL_UNSIGNED_SHORT, NULL);
	glFlush();
}

void changeHeightMap(int texture){
    glUniform1i(heightMap, texture);
}

void toggleWireframe(){
    if(wireframe){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    wireframe = !wireframe;

}

void changeSnowHeight(int direction){
    if(currentSnowHeight > 0.5 + currentWaterHeight && direction == -1){
        currentSnowHeight += 0.1 * direction;
        glUniform1f(snowHeightLoc, currentSnowHeight);
    } else if (direction == 1 && currentSnowHeight < 9.0){
        currentSnowHeight += 0.1 * direction;
        glUniform1f(snowHeightLoc, currentSnowHeight);
    }
}

void changeWaterHeight(int direction){
    if(currentSnowHeight - 0.5 > currentWaterHeight && direction == 1){
        currentWaterHeight += 0.1 * direction;
        glUniform1f(waterHeightLoc, currentWaterHeight);
    } else if (direction == -1 && currentWaterHeight > 2.0){
        currentWaterHeight += 0.1 * direction;
        glUniform1f(waterHeightLoc, currentWaterHeight);
    }
}

void onKeyPress(unsigned char key, int x, int y){
    switch (key) {
        case '1':
            changeHeightMap(0);
            break;
        case '2':
            changeHeightMap(1);
            break;
        case ' ':
            fprintf(stderr, "Space");
            toggleWireframe();
            break;
        case 'w':
            changeSnowHeight(1);
            break;
        case 's':
            changeSnowHeight(-1);
            break;
        case 'q':
            changeWaterHeight(1);
            break;
        case 'a':
            changeWaterHeight(-1);
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
    float newX = camX + moveX * direction * speed;
    float newZ = camZ + moveZ * direction * speed;
    if (newX > 150.0){
        newX = 150.0;
        moveX = 0.0;
    }
    if (newX < -150.0){
        newX = -150.0;
        moveX = 0.0;
    }
    if (newZ < -150.0){
        newZ = -150.0;
        moveZ = 0.0;
    }
    if (newZ > 120.0){
        newZ = 120.0;
        moveZ = 0.0;
    }
    camX = newX;
    camZ = newZ;

    cameraPos = glm::vec3 (camX,camY,camZ);
    cameraFront += glm::vec3 (moveX * direction * speed,0.0,moveZ * direction * speed);
    view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    fprintf(stderr, "X %f, Y%f, Z%f\n", cameraPos[0], cameraPos[1], cameraPos[2]);
    glUniform3fv(eyePosLoc, 1, glm::value_ptr(cameraPos));
}

void rotateCamera(float direction){
    angle += direction * 5 * CDR;
    cameraFront = glm::vec3(camX+cos(angle), camY, camZ+sin(angle));
    view = glm::lookAt(cameraPos, cameraFront, cameraUp);
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
        case GLUT_KEY_PAGE_UP:
            lightX++;
            break;
        case GLUT_KEY_PAGE_DOWN:
            lightX--;
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

