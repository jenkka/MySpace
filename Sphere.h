#ifndef SPHERE_H
#define SPHERE_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Utils.h"
#include "Mat4.h"
#include "Transforms.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

typedef float vec3[3];
#define M_PI 3.14159265358979323846
#define RESET 0xFFFF

struct strSphere{
	float radius;
	int   parallels;
	int   meridians;
	vec3  sphereColor;
};

typedef struct strSphere * Sphere;

static GLuint modelMatrixLoc, programId;
static GLuint sphereBuffer[4];
static GLuint sphereVA;

float *vertices;
float *normals ;

static Mat4 modelMatrix;

int indexLength;
int currentVertexSize, currentNormalSize;

Sphere createSphere(float radius, int parallels, int meridians, vec3 sphereColor);
void sphereBind(Sphere sphere, GLuint vertexPosLoc, GLuint vertexColorLoc, GLuint vertexNormalLoc);
void sphereDraw(Sphere sphere, GLuint textures[], int textureIndex);

#endif
