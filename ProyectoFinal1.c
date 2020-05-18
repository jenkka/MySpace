#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>

#include "Utils.h"
#include "Mat4.h"
#include "Transforms.h"
#include "Sphere.h"

#define RESET 0xFFFF
#define M_PI 3.14159265358979323846

typedef enum { IDLE, LEFT, RIGHT, UP, DOWN, FRONT, BACK } MOTION_TYPE;
typedef float vec3[3];

static GLuint vertexPositionLoc,  vertexNormalLoc, projectionMatrixLoc,  viewMatrixLoc;
static GLuint vertexColorLoc, vertexPositionLoc, vertexNormalLoc;
static GLuint cameraPositionLoc;
static GLuint ambientLightLoc, diffuseLightLoc, lightPositionLoc, materialALoc, materialDLoc, materialSLoc, exponentLoc;
static Mat4   projectionMatrix, viewMatrix;
static GLuint programId, modelMatrixLoc, vertexTexcoordLoc;
static Mat4   modelMatrix;
static GLuint textures[4];

static MOTION_TYPE motionType 	= 0;

static float cameraSpeed     	= 1;
static float cameraX        	= 0.0;
static float cameraZ         	= 5.0;
static float cameraY		 	= 0.0;
static float cameraAngle     	= 0.0;
static float xAngle             = 0.0;
static float yAngle             = 0.0;

static float lightPosition[] 	= {-5.0, 0.0, -1.0};
static float ambientLight[]  	= {1.0, 1.0, 1.0};
static float diffuseLight[]  	= {1.0, 1.0, 1.0};
static float materialA[]     	= {0.5, 0.5, 0.5};
static float materialD[]     	= {0.5, 0.5, 0.5};
static float materialS[]		= {0.0, 0.0, 0.0};
static float exponent			= 0;

float radius = 1.5;
int parallels = 40, meridians = 40;
vec3 sphereColor = { 1.0, 0.08, 0.6 };

Sphere sphere;

static void initTexture(const char* filename, GLuint textureId) {
	unsigned char* data;
	unsigned int width, height;
	glBindTexture(GL_TEXTURE_2D, textureId);
	loadBMP(filename, &data, &width, &height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

static void initTextures()
{
	glGenTextures(4, textures);
	initTexture("textures/Venus.bmp",    textures[0]);
	initTexture("textures/Earth.bmp",    textures[1]);
}

static void initShaders() {
	sphere = createSphere(radius, parallels, meridians, sphereColor);

	GLuint vShader = compileShader("shaders/phong.vsh", GL_VERTEX_SHADER);
		if(!shaderCompiled(vShader)) return;
		GLuint fShader = compileShader("shaders/phong.fsh", GL_FRAGMENT_SHADER);
		if(!shaderCompiled(fShader)) return;

		programId = glCreateProgram();
		glAttachShader(programId, vShader);
		glAttachShader(programId, fShader);
		glLinkProgram(programId);

		vertexPositionLoc   = glGetAttribLocation(programId, "vertexPosition");
		vertexNormalLoc     = glGetAttribLocation(programId, "vertexNormal");
		vertexTexcoordLoc 	= glGetAttribLocation(programId, "vertexTexcoord");
		modelMatrixLoc      = glGetUniformLocation(programId, "modelMatrix");
		viewMatrixLoc       = glGetUniformLocation(programId, "viewMatrix");
		projectionMatrixLoc = glGetUniformLocation(programId, "projMatrix");

		ambientLightLoc     = glGetUniformLocation(programId, "ambientLight");
		diffuseLightLoc     = glGetUniformLocation(programId, "diffuseLight");
		lightPositionLoc    = glGetUniformLocation(programId, "lightPosition");
		materialALoc        = glGetUniformLocation(programId, "materialA");
		materialDLoc        = glGetUniformLocation(programId, "materialD");
		materialSLoc        = glGetUniformLocation(programId, "materialS");
		exponentLoc 		= glGetUniformLocation(programId, "exponent");
		cameraPositionLoc   = glGetUniformLocation(programId, "camera");

		sphereBind(sphere, vertexPositionLoc, vertexTexcoordLoc, vertexNormalLoc);
		printf("FInished binding");
}

static void initLights()
{
	glUseProgram(programId);
	glUniform3fv(ambientLightLoc,  1, ambientLight);
	glUniform3fv(diffuseLightLoc,  1, diffuseLight);
	glUniform3fv(lightPositionLoc, 1, lightPosition);
	glUniform3fv(materialALoc,     1, materialA);
	glUniform3fv(materialDLoc,     1, materialD);
	glUniform3fv(materialSLoc,     1, materialS);
	glUniform1f(exponentLoc, exponent);
}

static void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_PRIMITIVE_RESTART);
	//glPrimitiveRestartIndex(RESET);

	switch(motionType) {
  		case  LEFT  : 	 cameraX -= cameraSpeed; break;
  		case  RIGHT :  	cameraX += cameraSpeed; break;
		case  FRONT :  	cameraZ -= cameraSpeed; break;
		case  BACK  :  	cameraZ += cameraSpeed; break;
		case  UP    :	cameraY += cameraSpeed; break;
		case  DOWN  :	cameraY -= cameraSpeed; break;
		case  IDLE  :  ;
	}

//	Envío de proyección, vista y posición de la cámara al programa 1 (cuarto, rombo)
	glUseProgram(programId);
	glUniformMatrix4fv(projectionMatrixLoc, 1, true, projectionMatrix.values);
	mIdentity(&viewMatrix);
	rotateY(&viewMatrix, -cameraAngle);
	translate(&viewMatrix, -cameraX, -cameraY, -cameraZ);
	glUniformMatrix4fv(viewMatrixLoc, 1, true, viewMatrix.values);
	glUniform3f(cameraPositionLoc, cameraX, cameraY, cameraZ);

	//Create sphere
	rotateX(&modelMatrix, xAngle);
	rotateY(&modelMatrix, yAngle);
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	sphereDraw(sphere, textures, 1);
	mIdentity(&modelMatrix);

	glutSwapBuffers();
}

static void reshapeFunc(int w, int h) {
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    float aspect = (float) w / h;
    setPerspective(&projectionMatrix, 45, aspect, -0.1, -500);
}

static void timerFunc(int id)
{
	glutTimerFunc(10, timerFunc, id);
	glutPostRedisplay();
}

static void specialKeyReleasedFunc(int key,int x, int y)
{
	motionType = IDLE;
}

static void keyReleasedFunc(unsigned char key,int x, int y)
{
	motionType = IDLE;
}

static void specialKeyPressedFunc(int key, int x, int y)
{
	switch(key) {
		case 100:	yAngle	   	-=15; break;
		case 102:	yAngle	   	+=15; break;
		case 101: 	xAngle	   	+=15; break;
		case 103: 	xAngle	   	-=15; break;
	}
}

static void keyPressedFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'a':
		case 'A': motionType = LEFT; break;
		case 'd':
		case 'D': motionType = RIGHT; break;
		case 'w':
		case 'W': motionType = FRONT; break;
		case 's':
		case 'S': motionType = BACK; break;
		case 'e':
		case 'E': motionType = UP; break;
		case 'q':
		case 'Q': motionType = DOWN; break;

		case 27 : exit(0);
	}
 }


int main23487(int argc, char **argv)
{
	setbuf(stdout, NULL);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("SphereTest.c");
    glutDisplayFunc(displayFunc);
    glutReshapeFunc(reshapeFunc);
    glutTimerFunc(10, timerFunc, 1);
    glutKeyboardFunc(keyPressedFunc);
    glutKeyboardUpFunc(keyReleasedFunc);
    glutSpecialFunc(specialKeyPressedFunc);
    glutSpecialUpFunc(specialKeyReleasedFunc);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    initTextures();
    initShaders();
    initLights();

    glClearColor(1.0, 1.0, 1.0, 0);
    glutMainLoop();

	return 0;
}








