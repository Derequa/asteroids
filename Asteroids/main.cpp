#include "GL/glut.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "objects.h"
#include "datastructures.h"
#include "smack.h"

/**
    @file assignment1.cpp
    @author Derek Batts
    This program computes, renders, and transforms models for the asteroids game.
    This was completed with heavy use of the tuturials and code from:
        http://www.videotutorialsrock.com/index.php
    As well as the demo program available on the course webpage.
 */

#define NUM_ASTEROIDS 6
#define Z_LEVEL -14.0f
using namespace std;

void handle_menu(int ID);
void handleKeypress(unsigned char key, int x, int y);
void handleKeyRelease(unsigned char key, int x, int y);
void handleSpecialPress(int key, int x, int y);
void handleSpecialRelease(int key, int x, int y);
void initLighting();
void initRendering();
void handleResize(int w, int h);
void drawScene();
void update(int value);
void initAsteroidList(AsteroidList* asteroids);

//The angle to increment the object by
float __angle = 30.0f;
Asteroid* referenceRoid;
AsteroidList* asteroids;
PlayerShip* p;
MissleList* playerShots;
ExplosionList* explosions;

/**
    This is the main function. Its starts things and stuff.
    @param argc The number of arguments given
    @param argv The argument vector (where dey is)
    @return This function never returns directly, all control is handed over to glutMainLoop
*/
int main(int argc, char** argv)
{
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//Set the window size
	glutInitWindowSize(800, 800);

	//Create a window
	glutCreateWindow("Derek Batts - CSC 461 - Assignment 2");
	initLighting();
	//Initialize rendering
	initRendering();

	//Set handler functions for drawing, keypress, and window resize
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutKeyboardUpFunc(handleKeyRelease);
	glutSpecialFunc(handleSpecialPress);
	glutSpecialUpFunc(handleSpecialRelease);
	glutReshapeFunc(handleResize);

	glutCreateMenu(handle_menu);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutTimerFunc(25, update, 0);

	//Enable backface stuff
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	p = initPlayer();
	asteroids = createList();
	explosions = createExplosions();
	initAsteroidList(asteroids);

	//Setup player ship
	p->positionVector[X_] = -3.0f;
	p->positionVector[Y_] = 0.0f;
	p->positionVector[Z_] = -14.0f;
	p->orientation[Y_] = 1.0f;

	playerShots = createListMissle();

	//Start the main loop. glutMainLoop does not return :(
	glutMainLoop();

	//For you my dear compiler
	return EXIT_SUCCESS;
}

/**
	Initialize the 3D rendering
*/
void initRendering()
{
	//Dark unkown magic to make 3D drawing work
	glEnable(GL_DEPTH_TEST);
}

/**
	This function handles the updating, or the rotation for the render window
 */
void update(int value)
{
	//Update each asteroid
	for (AsteroidNode* a = asteroids->head; a != NULL; a = a->next)
		updateAsteroid(a->value);

	//Update explosions
	for (ExplosionNode* e = explosions->head; e != NULL;){
		//Check age
		if (e->value->age >= EXPLOSION_MAX_AGE){
			Explosion* temp = e->value;
			e = e->next;
			//Remove if it has reached its limit
			if (removeExplosion(temp, explosions))
				free(temp);
		}
		else {
			updateExplosion(e->value);
			e = e->next;
		}
	}

	//Update the player ship
	updatePlayer(p);

	//Loop through all the current missles
	for (MissleNode* m = playerShots->head; m != NULL;){
		//Update each missle
		updateMissle(m->value);
		//Increment the remove counter if we find an old missle
		if (m->value->age >= MISSLE_AGE_MAX){
			MissleNode* temp = m;
			m = m->next;
			removeMissle(temp->value, playerShots);
		}
		else m = m->next;
	}

	//Loop through each asteroid in our list
	for (AsteroidNode* a = asteroids->head; a != NULL;){
		bool changed = false;
		//Check if it collides with the player
		if (detectCollideAsteroidShip(a->value, p)){
			Asteroid* temp = a->value;
			a = a->next;
			changed = true;
			handleCollideAsteroidShip(asteroids, temp, p, explosions);
		}
		else {
			//Loop through each missle in our list of missles
			for (MissleNode* m = playerShots->head; (m != NULL) && (a != NULL);){
				//Check if the missle collides with the asteroid
				if (detectCollideAsteroidShot(a->value, m->value)){
					Missle* temp = m->value;
					Asteroid* othertemp = a->value;
					m = m->next;
					a = a->next;
					changed = true;
					handleCollideAsteroidShot(asteroids, othertemp, playerShots, temp, explosions);
					break;
				}
				else m = m->next;
			}
			//Check if a collision happened
			if (!changed && (a != NULL))
				a = a->next;
		}
	}

	//Check for no asteroids
	if (asteroids->size == 0){
		//Reset game
		initAsteroidList(asteroids);
		p->positionVector[X_] = PLAYER_INIT_POSX;
		p->positionVector[Y_] = PLAYER_INIT_POSY;
		p->spin = 0.0f;
		clearMissles(playerShots);
	}

	//Increment the angle we rotate the ship by
	__angle += 1.3f;
	if (__angle > 360)
		__angle -= 360;

	//Redraw and wait again
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

/**
	Draw the 3D scene
*/
void drawScene()
{
	//Clear information from the last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Switch to the drawing perspective
	glMatrixMode(GL_MODELVIEW);
	//Reset the perspective
	glLoadIdentity();

	//Save the matrix state
	glPushMatrix();
		//Move +3 in x direction and back in the z direction and rotate the object we place
		glTranslatef(3.0f, 0.0f, -14.0f);
		glRotatef(__angle, 1.0f, 0.0f, 0.0f);
		float enemyMat[] = { 0.0f, 1.0f, 1.0f, 0.0f };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, enemyMat);
		//Draw the alien ship with a torus and a sphere, making a saturn-y looking thing
		glutSolidSphere(0.5, 6, 6);
		glutSolidTorus(0.3, 0.5, 4, 8);
	//Reload matrix state
	glPopMatrix();

	//Loop through all the asteroids
	for (AsteroidNode* node = asteroids->head; node != NULL; node = node->next){
		//Save matri xstate for each asteroid
		glPushMatrix();
		Asteroid* a = node->value;
			//Move to the asteroids position
			glTranslatef(a->positionVector[X_], a->positionVector[Y_], a->positionVector[Z_]);
			//Rotate to the asteroids orientation
			glRotatef(a->spin, a->orientation[X_], a->orientation[Y_], a->orientation[Z_]);
			//Scale to the asteroid's size
			glScalef(a->scale[X_], a->scale[Y_], a->scale[Z_]);
			//Use the asteroids material
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, a->mat);
			//Draw all the triangles in the asteroid
			glBegin(GL_TRIANGLES);
			for (int j = 0; (j + 2) < a->numVerticies; j += 3){
				//Draw each triangle and its normal
				for (int k = 0; k < 3; k++){
					glNormal3fv(a->normals[j / 3]);
					glVertex3fv(a->verticies[j + k]);
				}
			}
			glEnd();
		glPopMatrix();
	}

	//Save the matrix before we draw the player ship
	glPushMatrix();
		//Move to the players position
		glTranslatef(p->positionVector[X_], p->positionVector[Y_], p->positionVector[Z_]);
		//Rotate the ship back on the X axis (make it look flat)
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		//Rotate on the Y axis (point it to the right)
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		//Rotate the ship to its orientation
		glRotatef(p->spin, p->orientation[X_], p->orientation[Y_], p->orientation[Z_]);
		//Scale the ship to be smaller
		glScalef(p->scale[X_], p->scale[Y_], p->scale[Z_]);
		//Draw all the triangles in the player ship
		glBegin(GL_TRIANGLES);
		//Set the material
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p->mat);
		for (int i = 0; (i + 2) < p->numVerticies;  i += 3){
			//Draw each triangle and its normal
			for (int j = 0; j < 3; j++){
				glNormal3fv(p->normals[i / 3]);
				glVertex3fv(p->verticies[i + j]);
			}
		}
		glEnd();
	glPopMatrix();

	//Setup for drawing missles
	glPointSize(MISSLE_SIZE);
	//Draw all the missles in the queue
	for (MissleNode* node = playerShots->head; node != NULL; node = node->next){
		//Save the matrix
		glPushMatrix();
			//Get the current missle
			Missle* m = node->value;
			//Set its material
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, m->mat);
			//Draw the point and its normal
			glBegin(GL_POINTS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glVertex3f(m->positionVector[X_], m->positionVector[Y_], m->positionVector[Z_]);
			glEnd();
		glPopMatrix();
	}

	//Setup for drawing explody bits
	glPointSize(EXPLOSION_PT_SIZE);
	for (ExplosionNode* e = explosions->head; e != NULL; e = e->next){
		//Save the matrix
		glPushMatrix();
			//Get the explosion
			Explosion* exp = e->value;
			//Set material and move to the explosion
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, exp->mat);
			glTranslatef(exp->positionVector[X_], exp->positionVector[Y_], exp->positionVector[Z_]);
			//Draw all the points
			glBegin(GL_POINTS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			for (int i = 0; i < EXPLOSION_NUM_PTS; i++){
				glVertex3f(exp->points[i][X_], exp->points[i][Y_], exp->points[i][Z_]);
			}
			glEnd();

		glPopMatrix();
	}
	

	glFlush();
	//Send the 3D scene to the window
	glutSwapBuffers();
}

/**
	This function handles interaction with a right click menu
	@param ID Menu entry ID
 */
void handle_menu(int ID)
{
	switch (ID) {
	case 0:
		//Leave if the Quit entry is clicked
		exit(0);
	}
}

/**
	Called when the window is resized
	@param w The new screen width
	@param h The new screen height
 */
void handleResize(int w, int h)
{
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, w, h);
	//Switch to setting camera perspective
	glMatrixMode(GL_PROJECTION);
	//Set the camera perspective
	glLoadIdentity();
	//Pass in camera angle, width-to-height ratio, the near z clipping coordinate, and the far z clipping coordinate
	gluPerspective(45.0, (double) w /(double) h, 1.0, 200.0);

}

/**
	Called when a key is pressed
	@param key The key that was pressed
	@param x The mouse's x coordinate
	@param y The mouse's y coordinate
*/
void handleKeypress(unsigned char key, int x, int y)
{
	Missle* newM = NULL;
	switch (key)
	{
		//Exit on escape press
	case 27:
		exit(EXIT_SUCCESS);
		//Set the X flag in the objects module
	case 'x':
		setKeyX(true);
		break;
	case 'X':
		setKeyX(true);
		break;
		//Try to fire a shot and add it to the queue
	case'z':
		newM = fireShot(p);
		if (newM != NULL)
			addToMissleList(newM, playerShots);
		break;
	case'Z':
		newM = fireShot(p);
		if (newM != NULL)
			addToMissleList(newM, playerShots);
		break;
	}
}

/**
	Called when a key is released
	@param key The key that was pressed
	@param x The mouse's x coordinate
	@param y The mouse's y coordinate
*/
void handleKeyRelease(unsigned char key, int x, int y)
{
	switch (key)
	{
		//Unset the X flag in the objects module
	case 'x':
		setKeyX(false);
		break;
	case 'X':
		setKeyX(false);
		break;
	}
}

/**
	Called when a special key is pressed
	@param key The special that was pressed
	@param x The mouse's x coordinate
	@param y The mouse's y coordinate
*/
void handleSpecialPress(int key, int x, int y)
{
	switch (key)
	{
		//Set the flag for a right keypress in the objects module
	case GLUT_KEY_LEFT:
		setKeyLeft(true);
		break;
		//Set the flag for a left keypress in the objects module
	case GLUT_KEY_RIGHT:
		setKeyRight(true);
		break;
	}
}

/**
	Called whenever a special key is released
	@param key The key that was released
	@param x The mouse's x coordinate
	@param y The mouse's y coordinate
*/
void handleSpecialRelease(int key, int x, int y)
{
	switch (key)
	{
		//Unset the flag for a left keypress in the objects module
	case GLUT_KEY_LEFT:
		setKeyLeft(false);
		break;
		//Unset the flag for a right keypress in the objects module
	case GLUT_KEY_RIGHT:
		setKeyRight(false);
		break;
	}
}

/**
	This initializes lighting magic
	Its pretty much all stuff from the demo program
*/
void initLighting()
{
	float  amb[] = { 0, 0, 0, 1 };	// Ambient material property
	float  lt_amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };	// Ambient light property
	float  lt_dif[] = { 0.8f, 0.8f, 0.8f, 1.0f };	// Ambient light property
	float  lt_pos[] = {			// Light position
		0, .39392, .91914, 0
	};
	float  lt_spc[] = { 0, 0, 0, 1 };	// Specular light property

	//  Set default ambient light in scene

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

	//  Set Light 0 position, ambient, diffuse, specular intensities

	glLightfv(GL_LIGHT0, GL_POSITION, lt_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lt_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lt_dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lt_spc);

	//  Enable Light 0 and GL lighting

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glShadeModel(GL_FLAT);		// Flat shading
	glEnable(GL_NORMALIZE);		// Normalize normals

	glClearDepth(1.0);			// Setup background colour
	glClearColor(0, 0, 0, 0);
}

/**
	This function initializes a given array of asteroids randomly according to the specified rules
	@param a The reference to an array of Asteroid pointers
*/
void initAsteroidList(AsteroidList* list)
{
	//Loop through the whole array
	for (int i = 0; i < NUM_ASTEROIDS; i++){
		//Create an asteroid
		Asteroid* a = initAsteroid();

		//Randomly set its spin
		a->spin = rand() % 90;
		//Randomly set the spin factor
		GLfloat spinFactor;
		for (spinFactor = 100.0f; (spinFactor < -2.0) || (spinFactor > 2.0) || (spinFactor == -0.0f) || (spinFactor == 0.0f); spinFactor = (float)-1 * (rand() % 2) * ((float)3 / (rand() % 15)));
			a->spinFactor = spinFactor;
		
		//Randomly generate and set scale
		GLfloat scale;
		for (scale = 0.0f; (scale < 0.4f) || (scale > 0.9f); scale = (float)1 / (rand() % 10));
		for (int j = 0; j < 3; j++)
			a->scale[j] = scale;

		//Randomly pick an axis to spin about
		a->orientation[rand() % 3] = 1.0f;

		for (int j = 0; j < 2; j++){
			//Randomly generate a velocity
			GLfloat vel;
			for (vel = 0.0f; (vel < -0.06f) || (vel > 0.06f) || (vel == 0.0f) || (vel == -0.0f); vel = (float)1 / (10 + (25 + rand() % 100)));
			
			//Alternal direction
			if (i < (NUM_ASTEROIDS / 2))
				a->vVector[j] = vel;
			else
				a->vVector[j] = -vel;

			//Randomly generate a starting position
			GLfloat pos;
			for (pos = -10.f; (pos < -6.0f) || (pos > 6.0f) || (pos == -0.00); pos = (float)-1 * (rand() % 2) * (rand() % 7));
			a->positionVector[j] = pos;
		}
		//Set the Z value
		a->positionVector[Z_] = Z_LEVEL;

		addToList(a, list);
	}
}
