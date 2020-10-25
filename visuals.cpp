#include <stdio.h>     // - Just for some ASCII messages
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <time.h>

#include "gl/glut.h"   // - An interface and windows management library
#include "visuals.h"   // Header file for our OpenGL functions

model md;
static float tx = 0.0;

static float rotx = 0.0;
static float roty = 0.0;
static float rotz = 0.0;

static float earth_angle = 0;
static float moon_angle = 0;

static bool animate = false;

using namespace std;

static int x[500];
static int y[500];
static int z[500];
static bool lightUp = true;
static float lightVolume = 0.5;

void axes()
{
	glColor3f(0.6, 0.6, 0.6);
	glPushMatrix();
	glTranslatef(0, 0, -1.0);
	glBegin(GL_LINES);
	glVertex2f(0.0, 0.0);
	glVertex2f(100.0, 0.0);
	glVertex2f(0.0, 0.0);
	glVertex2f(0.0, 100.0);
	glEnd();
	glPopMatrix();
}

void createStars() {

	int X, Y, Z, quarter, depth;
	srand(time(NULL));
	for (int i = 0; i < 500; i++) {
		X = (rand() % 600) - 300;
		Y = (rand() % 600) - 300;
		Z = (rand() % 600) - 300;

		x[i] = X;
		y[i] = Y;
		z[i] = Z;
	}
}

void renderStars() {
	glColor3f(0.6, 0.6, 0.6);
	int X, Y, Z, quarter, depth;
	for (int i = 0; i < 500; i++) {
		X = x[i];
		Y = y[i];
		Z = z[i];

		glPushMatrix();

		glTranslatef(X, Y, Z);
		glutSolidSphere(0.3, 35, 24);

		glPopMatrix();
	}
}

void Render()
{
	//CLEARS FRAME BUFFER ie COLOR BUFFER& DEPTH BUFFER (1.0)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clean up the colour of the window
														 // and the depth buffer
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glTranslatef(0, 0, -100);

	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);
	glRotatef(rotz, 0, 0, 1);

	glColor3f(1, 1, 0);
	glutSolidSphere(10, 35, 24);

	DisplayModel(md);

	glColor4f(1, 1, 0, lightVolume);
	glutSolidSphere(15, 12, 13);

	renderStars();

	axes();

	glutSwapBuffers();             // All drawing commands applied to the 
								   // hidden buffer, so now, bring forward
								   // the hidden buffer and hide the visible one
}

//-----------------------------------------------------------

void Resize(int w, int h)
{
	// define the visible area of the window ( in pixels )
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);

	// Setup viewing volume

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0, (float)w / (float)h, 1.0, 500.0);
}

void Idle()
{
	if (animate) {
		if (lightUp) {
			lightVolume += 0.01;

			if (lightVolume > 0.99)
				lightUp = false;

		}
		else {
			lightVolume -= 0.01;
			if (lightVolume < 0.1)
				lightUp = true;

		}

		earth_angle += EARTH_SPEED;
		moon_angle += MOON_SPEED;
	}
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: exit(0);   // Ascii: 27 = Escape
		break;
	case 'W': case 'w': rotx += ROTATION_SPEED;
		break;
	case 'S': case 's': rotx -= ROTATION_SPEED;
		break;
	case 'A': case 'a': roty += ROTATION_SPEED;
		break;
	case 'D': case 'd': roty -= ROTATION_SPEED;
		break;
	case 'Q': case 'q': rotz += ROTATION_SPEED;
		break;
	case 'E': case 'e': rotz -= ROTATION_SPEED;
		break;
	case ' ': animate = !animate;
		glutPostRedisplay();
		break;
	}

	glutPostRedisplay();
}


void Setup()  // TOUCH IT !! 
{
	ReadFile(&md);

	//Parameter handling
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);  //renders a fragment if its z value is less or equal of the stored value
	glClearDepth(1);

	// polygon rendering mode
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//Set up light source
	GLfloat light_position[] = { 0.0, 30.0, 50.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	GLfloat ambientLight[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat diffuseLight[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	//01
	glFrontFace(GL_CCW);


	createStars();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void ReadFile(model *md)
{

	ifstream in("planet.obj");                  // Open the file for reading OBJINFO.TXT

	if (in.fail())
		exit(-1);
	char sep;
	char buffer[512];

	md->vertices = 0;
	md->faces = 0;

	while (!in.eof()) {

		in >> buffer;

		if (strcmp(buffer, "v") == 0)
		{
			float a, b, c;

			//format : a b c
			in >> a >> b >> c;

			int i = md->vertices++;
			md->obj_points[i].x = a;
			md->obj_points[i].y = b;
			md->obj_points[i].z = c;

		}
		else if (strcmp(buffer, "f") == 0)
		{

			int a, b, c;
			int t;

			// format : a//t b//t c//t
			in >> a >> sep >> sep >> t >> b >> sep >> sep >> t >> c >> sep >> sep >> t;

			int i = md->faces++;
			md->obj_faces[i].vtx[0] = a;
			md->obj_faces[i].vtx[1] = b;
			md->obj_faces[i].vtx[2] = c;
		}

		in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	}

	in.close();
}

void DisplayModel(model md)
{
	glPushMatrix();

	glColor3f(0.0, 0.0, 1.0);

	float earth_cos = cos(earth_angle) * EARTH_DISTANCE;
	float earth_sin = sin(earth_angle) * EARTH_DISTANCE;
	glTranslatef(earth_cos, 0.0, earth_sin);

	glPushMatrix();
	glScalef(EARTH_SCALE, EARTH_SCALE, EARTH_SCALE);

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < md.faces; i++)
	{
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[0] - 1].x, md.obj_points[md.obj_faces[i].vtx[0] - 1].y, md.obj_points[md.obj_faces[i].vtx[0] - 1].z);
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[1] - 1].x, md.obj_points[md.obj_faces[i].vtx[1] - 1].y, md.obj_points[md.obj_faces[i].vtx[1] - 1].z);
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[2] - 1].x, md.obj_points[md.obj_faces[i].vtx[2] - 1].y, md.obj_points[md.obj_faces[i].vtx[2] - 1].z);
	}

	glEnd();

	glPopMatrix();

	float moon_cos = cos(moon_angle) * MOON_DISTANCE;
	float moon_sin = sin(moon_angle) * MOON_DISTANCE;
	glTranslatef(0, moon_cos, moon_sin);

	glColor3f(0.6f, 0.6f, 0.6f);

	glPushMatrix();
	glScalef(MOON_SCALE, MOON_SCALE, MOON_SCALE);

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < md.faces; i++)
	{
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[0] - 1].x, md.obj_points[md.obj_faces[i].vtx[0] - 1].y, md.obj_points[md.obj_faces[i].vtx[0] - 1].z);
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[1] - 1].x, md.obj_points[md.obj_faces[i].vtx[1] - 1].y, md.obj_points[md.obj_faces[i].vtx[1] - 1].z);
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[2] - 1].x, md.obj_points[md.obj_faces[i].vtx[2] - 1].y, md.obj_points[md.obj_faces[i].vtx[2] - 1].z);
	}

	glEnd();

	glPopMatrix();

	glPopMatrix();

}