#include "Sphere.h"

Sphere createSphere(float radius, int parallels, int meridians, vec3 sphereColor) {

	Sphere sphere = malloc(sizeof(struct strSphere));

	sphere->radius = radius;
	sphere->parallels = parallels;
	sphere->meridians = meridians;

	sphere->sphereColor[0] = sphereColor[0];
	sphere->sphereColor[1] = sphereColor[1];
	sphere->sphereColor[2] = sphereColor[2];

	//printf("%.2f, %d, %d", sphere->radius, sphere->parallels, sphere->meridians);

	return sphere;
}

void sphereBind(Sphere sphere, GLuint vertexPositionLoc, GLuint vertexTexcoordLoc, GLuint vertexNormalLoc) {

	GLuint indexes[(sphere->parallels + 1) * (sphere->parallels + 1) * 3 * 2];
	float texcoords[(sphere->parallels + 1) * (sphere->parallels + 1) * 3];
	float vertexA[(sphere->parallels + 1) * (sphere->parallels + 1) * 3];
	float normalA[(sphere->parallels + 1) * (sphere->parallels + 1) * 3];
	float  colors[(sphere->parallels + 1) * (sphere->parallels + 1) * 3];

	//float r, g, b;											// colors
	float x, y, z, xy;                                      // vertex position
	float nx, ny, nz, lengthInv = 1.0f / sphere->radius;    // vertex normal
	float u, v; 											// texture

	float sectorStep = 2 * M_PI / sphere->meridians;
	float stackStep = M_PI / sphere->parallels;
	float sectorAngle, stackAngle;

	srand(time(NULL));

	//Vertex / normals Array
	int colorIndex = 0, vertexIndex = 0, normalIndex = 0, texIndex = 0;
	for(int i = 0; i <= sphere->parallels; ++i)
	{
	    stackAngle = (M_PI / 2) - (i * stackStep);          // starting from pi/2 to -pi/2
	    xy = sphere->radius * cosf(stackAngle);             // r * cos(u)
	    z = sphere->radius * sinf(stackAngle);              // r * sin(u)
	    for(int j = 0; j <= sphere->meridians; ++j)
	   	    {
	   	        sectorAngle = j * sectorStep;           // starting from 0 to 2pi

	   	        // vertex position (x, y, z)
	   	        x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
	   	        y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

	   	        vertexA[vertexIndex++] = x;
	   			vertexA[vertexIndex++] = y;
	   			vertexA[vertexIndex++] = z;
	   	        // normalized vertex normal (nx, ny, nz)
	   	        nx = x * lengthInv;
	   	        ny = y * lengthInv;
	   	        nz = z * lengthInv;
	   			normalA[normalIndex++] = nx;
	   			normalA[normalIndex++] = ny;
	   			normalA[normalIndex++] = nz;
	   			// texture coordinates (between 0, 1)
	   			u = asinf(nx)/M_PI + 0.5f;
	   			v = asinf(ny)/M_PI + 0.5f;
	   			//printf("v: %.2f, u: %.2f \n", v, u);
	   			texcoords[texIndex++] = u;
	   			//printf("Index: %d, Value: %.2f  ", texIndex, u);
	   			texcoords[texIndex++] = v;
	   			//printf("Index: %d, Value: %.2f \n", texIndex, v);
	    }
	}
	printf("\n # Vertex %d", vertexIndex);


	//Index array
	int k1 = 0, k2 = 0;
	int indexIndex = 0;
	for(int i = 0; i < sphere->parallels; ++i)
	{
	    k1 = i * (sphere->meridians + 1);     // beginning of current stack
	    k2 = k1 + (sphere->meridians + 1);      // beginning of next stack
	    for(int j = 0; j < sphere->meridians; ++j, ++k1, ++k2)
	    {
	        // 2 triangles per sector excluding first and last stacks
	        // k1 => k2 => k1+1
	        if(i != 0)
	        {
	        	indexes[indexIndex++] = k1;
	        	indexes[indexIndex++] = k2;
	        	indexes[indexIndex++] = k1 + 1;
	        }
	        // k1+1 => k2 => k2+1
	        if(i != (sphere->parallels-1))
	        {
	            indexes[indexIndex++] = k1 + 1;
	        	indexes[indexIndex++] = k2;
	        	indexes[indexIndex++] = k2 + 1;
	        }
	    }
	}

	//for(int i = 0; i < sizeof(indexes) / sizeof(int); i++)
	    printf("\n # Indexes %d\n", indexIndex);


	indexLength = indexIndex;

	//Send to OPENGL
	glUseProgram(programId);
	glGenVertexArrays(1, &sphereVA);
	glBindVertexArray(sphereVA);
	glGenBuffers(4, sphereBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, sphereBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexA), vertexA, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPositionLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPositionLoc);

	glBindBuffer(GL_ARRAY_BUFFER, sphereBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalA), normalA, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexNormalLoc);

	glBindBuffer(GL_ARRAY_BUFFER, sphereBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexTexcoordLoc, 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexTexcoordLoc);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereBuffer[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

}

void sphereDraw(Sphere sphere, GLuint textures[], int textureIndex) {
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	glBindVertexArray(sphereVA);
	glBindTexture(GL_TEXTURE_2D, textures[textureIndex]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereBuffer[3]);
	glDrawElements(GL_TRIANGLES, indexLength, GL_UNSIGNED_INT, 0);
}
