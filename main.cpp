#include<windows.h>
#include<freeglut.h>
#include <iostream>
#include<math.h>
#include <stdlib.h>  
#include <cstdlib> 
#include "SOIL.h"
#include "imageloader.h"

// pozitia observatorului 
float x = 0.0f, y = -5.0f, z = 1.75f;
// pozitia sursei
float xs = -20.0, ys = -5.0, zs = 10.0, ts = 1.0;
// directia camerei (in lungul axei Oy)
float lx = 0.0f, ly = 1.0f, lz = 0;
float fraction = 0.15f;
float alpha = 0.3;
GLuint list;

GLfloat alb[] = { 1.0, 1.0, 1.0, 0.0 };
GLfloat negru[] = { 0.0, 0.0, 0.0, 0.0 };

GLfloat ctrlpoints[4][4][3] = {
   {{ -0.5, -10.5, 0.0}, { -1.5, -9.25, 0.2},
	{1.5, -15.5, 0.5}, {2.5, -10.5, 0.5}},
   {{ -1.5, -0.5, 0.25}, { -0.5, -0.5, 1.15},
	{1.5, -0.5, 0.25}, {-3.5, -0.5, 0.0}},
   {{ -1.5, 0.5, 0.25}, { -1.5, 0.5, 0.0},
	{0.5, 0.5, 0.25}, {1.5, 0.5, 0.25}},
   {{ -1.5, 10.5, 0.0}, { -0.5, 14.5, 0.0},
	{1.25, 14.5, 0.0}, {1.5, 14.5, 0.0}}
};

GLfloat texpts[2][2][2] = { {{0.0, 0.0}, {0.0, 1.0}},
			{{1.0, 0.0}, {1.0, 1.0}} };

#define	imageWidth 64
#define	imageHeight 64
GLubyte image[3 * imageWidth * imageHeight];
GLuint texture1;

void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte* estring;

	estring = gluErrorString(errorCode);
	fprintf(stderr, "Quadric Error: %s\n", estring);
	exit(0);
}

void reshapeAndProjection(int w, int h)
{
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 0.1f, 500.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void loadTexture(void)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image("test_text.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void lumina(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	// initializari si proprietatile universale
	GLfloat alphaAmb = 1.0;
	GLfloat alphaDifuz = 1.0;
	GLfloat difuzLumina[4];
	GLfloat unghiCutOff = 180.0;
	alphaAmb = 0.6;
	alphaDifuz = 0.6;
	difuzLumina[0] = alphaDifuz; difuzLumina[1] = alphaDifuz; difuzLumina[2] = alphaDifuz; difuzLumina[3] = 0;
	unghiCutOff = 180;

	// Sursa de lumina
	GLfloat ambientLumina[] = { alphaAmb, alphaAmb, alphaAmb, alphaAmb };
	GLfloat pozitial0[] = { xs, ys, zs, ts };
	GLfloat directieSpot[] = { 0, 0, 1, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, pozitial0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLumina);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, difuzLumina);
	glLightfv(GL_LIGHT0, GL_SPECULAR, alb);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, directieSpot);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, unghiCutOff);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 3.0);

};

void fog1() {
	GLfloat fogColor[4] = { 0.6, 0.6, 0.6, 0.7 };
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.015);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 0);
	glFogf(GL_FOG_END, 0);

	glClearColor(0.7, 0.7, 0.7, 0.5);
}

void drawRoof() {
	GLfloat acoperis[] = { 0.12, 0.1, 0.09, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, acoperis);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);

	glBegin(GL_TRIANGLES);
	// Fata
	glVertex3f(0.0f, 0.7f, 0.0f);
	glVertex3f(-2.2f, -0.4f, 2.2f);
	glVertex3f(2.2f, -0.4f, 2.2f);
	// Dreapta
	glVertex3f(0.0f, 0.7f, 0.0f);
	glVertex3f(2.2f, -0.4f, 2.2f);
	glVertex3f(2.2f, -0.4f, -2.2f);
	// Spate
	glVertex3f(0.0f, 0.7f, 0.0f);
	glVertex3f(2.2f, -0.4f, -2.2f);
	glVertex3f(-2.2f, -0.4f, -2.2f);
	// Stanga
	glVertex3f(0.0f, 0.7f, 0.0f);
	glVertex3f(-2.2f, -0.4f, -2.2f);
	glVertex3f(-2.2f, -0.4f, 2.2f);
	glEnd();
}

void drawHouse() {
	GLfloat casa[] = { 0.13, 0.1, 0.09, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, casa);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.9f);
	glutSolidCube(3.20);

	//usa
	GLfloat usa[] = { 0.009, 0.008, 0.007, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, usa);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.9f);
	glPushMatrix();
	glTranslatef(0.5, -1.75f, -0.5f);
	glScalef(0.50, 0.15, 0.95);
	glutSolidCube(2);
	glPopMatrix();

	//suport
	GLfloat suport[] = { 0.4, 0.3, 0.3, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, suport);
	glPushMatrix();
	glTranslatef(-1.02, -2.0, 0.05);
	glScalef(0.30, 0.95, 0.15);
	glutSolidCube(1);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, negru);
}

void quadric() {
	GLUquadricObj* qobj;
	GLfloat mat_ambient[] = { 0.7, 0.7, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	list = glGenLists(1);
	qobj = gluNewQuadric();

	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	glNewList(list, GL_COMPILE);
	//gluSphere(qobj, 0.35, 15, 10);
	glRotatef(180, 0, 1, 0);
	glTranslatef(0.0, 0.0, -0.75);
	gluCylinder(qobj, 0.5, 0.1, 1.0, 15, 5);
	glEndList();

	glShadeModel(GL_SMOOTH);
	glTranslatef(0.0, 2.0, 3.0);
	glPushMatrix();
	glCallList(list);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, negru);
	glMaterialfv(GL_FRONT, GL_SPECULAR, negru);
	glMaterialfv(GL_FRONT, GL_SHININESS, negru);
}

void quadric2() {
	GLUquadricObj* qobj;
	GLfloat mat_ambient[] = { 0.7, 0.7, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	list = glGenLists(1);
	qobj = gluNewQuadric();

	gluQuadricDrawStyle(qobj, GLU_FILL); /* smooth shaded */
	gluQuadricNormals(qobj, GLU_SMOOTH);
	glNewList(list, GL_COMPILE);
	gluSphere(qobj, 0.55, 15, 10);
	glTranslatef(0.0, 0.0, -0.75);
	glEndList();

	glShadeModel(GL_SMOOTH);
	glTranslatef(0.0, 2.0, 3.0);
	glPushMatrix();
	glCallList(list);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, negru);
	glMaterialfv(GL_FRONT, GL_SPECULAR, negru);
	glMaterialfv(GL_FRONT, GL_SHININESS, negru);
}

void drawTree() {

	GLfloat copac2[] = { 0.2, 0.9, 0.5, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, copac2);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.5f);

	// coroana
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 1.75f);
	glutSolidCone(0.25f, 0.55f, 20, 20);
	glTranslatef(0.0f, 0.0f, -0.35f);
	glutSolidCone(0.35f, 0.5f, 20, 20);
	glTranslatef(0.0f, 0.0f, -0.35f);
	glutSolidCone(0.45, 0.55f, 20, 20);
	glTranslatef(0.0f, 0.0f, -0.30f);
	glutSolidCone(0.60f, 0.60f, 20, 20);
	glTranslatef(0.0f, 0.0f, -0.30f);
	glutSolidCone(0.65f, 0.80f, 20, 20);

	// trunchi

	GLfloat trunchi2[] = { 0.15, 0.12, 0.08, 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, trunchi2);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.1f);
	glRotatef(45, 0, 0, 1);
	glScalef(0.15, 0.15, 0.45);
	glTranslatef(0.0f, 0.0f, -0.50f);
	glutSolidCube(1);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, alb);
}

void drawLuminaCasa() {

	GLfloat chestie[] = { 0.4, 0.3, 0.3, 0.005 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, chestie);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_COLOR);

	glPushMatrix();
	glScalef(0.21, 0.20, 0.4);
	glutSolidCube(1);
	glPopMatrix();

	GLfloat chestie2[] = { 0.6, 0.5, 0.5, 0.005 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, chestie2);
	glPushMatrix();
	glScalef(-0.65, -0.65, -0.65);
	glTranslatef(0.0, 0.0, -0.35);
	glutSolidSphere(0.25, 10, 10);
	glPopMatrix();

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void stalp() {
	glPushMatrix();
	glTranslatef(-1.9, -9.85, -1.25);
	quadric();
	glTranslatef(0, 0, -0.85);
	glScalef(0.45, 0.45, 3.6);

	GLfloat casa[] = { 0.12, 0.1, 0.09, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, casa);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.9f);
	glutSolidCube(0.5);
	glPopMatrix();
}

void makeImage(void)
{
	int i, j;
	float ti, tj;

	for (i = 0; i < imageWidth; i++) {
		ti = 2.0 * 3.14159265 * i / imageWidth;
		for (j = 0; j < imageHeight; j++) {
			tj = 2.0 * 3.14159265 * j / imageHeight;

			image[3 * (imageHeight * i + j)] = (GLubyte)127 * (1.0 + sin(ti));
			image[3 * (imageHeight * i + j) + 1] = (GLubyte)127 * (1.0 + cos(2 * tj));
			image[3 * (imageHeight * i + j) + 2] = (GLubyte)127 * (1.0 + cos(ti + tj));
		}
	}
}

GLuint loadTexturePath(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, texture1); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
		0,                            //0 for now
		GL_RGB,                       //Format OpenGL uses for image
		image->width, image->height,  //Width and height
		0,                            //The border of the image
		GL_RGB, //GL_RGB, because pixels are stored in RGB format
		GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
		//as unsigned numbers
		image->pixels);               //The actual pixel data
	return textureId; //Returns the id of the texture
}

void bezier() {

	glClearColor(0.99, 0.9, 1.0, 0.0);
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
		0, 1, 12, 4, &ctrlpoints[0][0][0]);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2,
		0, 1, 4, 2, &texpts[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	makeImage();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	Image* image = loadBMP("text_texxt.bmp");
	texture1 = loadTexturePath(image);

	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glDisable(GL_TEXTURE_2D);

}

void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	lumina();
	fog1();

	// Pozitia observatorului
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 0.0f, 1.0f);

	glDisable(GL_FOG);
	// Sol
	GLfloat sol[] = { 0.05, 0.05, 0.14, 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sol);
	glEnable(GL_TEXTURE_2D);
	loadTexture();
	//glColor4f(0.1, 0.1, 0.1, 1);
	glNormal3f(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord3f(-100.0f, -100.0f, 0.0f);  glVertex3f(-100.0f, -100.0f, 0.0f);
	glTexCoord3f(-100.0f, 100.0f, 0.0f);  glVertex3f(-100.0f, 100.0f, 0.0f);
	glTexCoord3f(100.0f, 100.0f, 0.0f);  glVertex3f(100.0f, 100.0f, 0.0f);
	glTexCoord3f(100.0f, -100.0f, 0.0f);  glVertex3f(100.0f, -100.0f, 0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	// Fundal
	GLfloat noapte[] = { 0.05, 0.15, 0.25, 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, noapte);
	glutSolidSphere(200, 30, 30);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, negru);
	
	//Casa
	glPushMatrix();
	glTranslatef(0, 15.0f, 1.5f);
	glRotatef(10, 0, 0, 1);
	drawHouse();
	glTranslatef(0, 0, 2.0f);
	glRotatef(90, 1, 0, 0);

	drawRoof();
	glPopMatrix();
	glEnable(GL_FOG);

	// Copaci
	for (int i = -4; i < 4; i++)
		for (int j = -1; j < 4; j++) {
			glPushMatrix();
			glScalef(2, 2, 2.75);
			glTranslatef(i * 2.0, j * 3.0, 0);
			if (i != 0) drawTree();
			glPopMatrix();
		}

	glPushMatrix();
	glTranslatef(-0.60, 12.5, 1.65);
	glRotatef(10, 0, 0, 1);
	drawLuminaCasa();
	glPopMatrix();

	stalp();
	glPushMatrix();
	glTranslatef(5.25, 0.0, 0.0);
	stalp();
	glPopMatrix();

	GLfloat path[] = { 0.15, 0.15, 0.15, 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, path);
	bezier();

	glPushMatrix();
	glTranslatef(-13.0, 0.0, 5.75);
	quadric2();
	glPopMatrix();

	glutSwapBuffers();
	glFlush();
}

void processNormalKeys(unsigned char key, int x, int y)
{
	float angle = 0.02f;
	float angles = 0.05f;
	switch (key) {
	case 'a':
		lx -= angle;
		break;
	case 'd':
		lx += angle;
		break;
	case 'w':
		lz += angle;
		break;
	case 's':
		lz -= angle;
		break;
	case '1':
		xs -= angles;
		break;
	case '2':
		xs += angles;
		break;
	case '3':
		ys -= angles;
		break;
	case '4':
		ys += angles;
		break;
	case '5':
		zs -= angles;
		break;
	case '6':
		zs += angles;
		break;
	}
	if (key == 27)
		exit(0);
}
void processSpecialKeys(int key, int xx, int yy) {
	switch (key) {
	case GLUT_KEY_LEFT:
		x -= fraction;
		break;
	case GLUT_KEY_RIGHT:
		x += fraction;
		break;
	case GLUT_KEY_UP:
		y += fraction;
		break;
	case GLUT_KEY_DOWN:
		y -= fraction;
		break;
	}
}


int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(1350, 750);
	glutCreateWindow("3D");

	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshapeAndProjection);
	glutIdleFunc(renderScene);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();

	return 1;
}

