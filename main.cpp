#include "GL/glut.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "objects.h"
#include "datastructures.h"
#include "smack.h"

/*
    @file assignment1.cpp
    @author Derek Batts - dsbatts@ncsu.edu
    This program computes, renders, and transforms models for the asteroids game.
    This was completed with heavy use of the tuturials and code from:
    http://www.videotutorialsrock.com/index.php
    As well as the demo program available on the course webpage.
 */

// The max number of asteroids that cand spawn at the begining of a screen / level
#define MAX_NUM_ASTEROIDS 6
// The depth we draw everything at
#define Z_LEVEL -14.0f
// The time between aliens spawning
#define ALIEN_SPAWN_TIME 700
// The minimum lifetime score needed for a small alien ship to spawn
#define ALIEN_SMALL_SPAWN_REQ 5000
// The amount of score needed to get a new life
#define NEW_LIFE_REQ 7000
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
void initAsteroidList(ObjectList* asteroids);
void restartGame();
void drawText(float x, float y, float z, char* string);

// The number of asteroid to spawn on a new screen
int num_asteroids = 1;
// The timer to count until a new alien spawns
int alienTimer = ALIEN_SPAWN_TIME;
bool firstSpawned = false;
int lifetimeScore = 0;
Asteroid* referenceRoid;
ObjectList* asteroids;
PlayerShip* p;
ObjectList* playerShots;
ObjectList* explosions;
ObjectList* aliens;
ObjectList* alienShots;

/**
    This is the main function. Its starts things and stuff.
    @param argc The number of arguments given
    @param argv The argument vector (where dey is)
    @return This function never returns directly, all control is handed over to glutMainLoop
*/
int main(int argc, char** argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	// Set the window size
	glutInitWindowSize(800, 800);

	// Create a window
	glutCreateWindow("Derek Batts - CSC 461 - Assignment 4");
	initLighting();
	// Initialize rendering
	initRendering();

	// Set handler functions for drawing, keypress, and window resize
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutKeyboardUpFunc(handleKeyRelease);
	glutSpecialFunc(handleSpecialPress);
	glutSpecialUpFunc(handleSpecialRelease);
	glutReshapeFunc(handleResize);

	// Make the menu
	glutCreateMenu(handle_menu);
	glutAddMenuEntry("Quit", 0);
	glutAddMenuEntry("Restart Game", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Set the update function (main game loop)
	glutTimerFunc(25, update, 0);

	// Enable backface stuff
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	p = initPlayer();
	// Create lists of stuff
	asteroids = createList();
	explosions = createList();
	initAsteroidList(asteroids);
	playerShots = createList();
	aliens = createList();
	alienShots = createList();

	// Setup player ship
	p->positionVector[X_] = -3.0f;
	p->positionVector[Y_] = 0.0f;
	p->positionVector[Z_] = -14.0f;
	p->orientation[Y_] = 1.0f;

	// Start the glut main loop. glutMainLoop does not return :(
	glutMainLoop();

	// For you my dear compiler
	return EXIT_SUCCESS;
}

/*
	Initialize the 3D rendering.
*/
void initRendering()
{
	// Dark unkown magic to make 3D drawing work
	glEnable(GL_DEPTH_TEST);
}

/*
	This function is called to update object in the game world and acts as our
	main game loop.
	@param value I have no idea.
 */
void update(int value)
{
	// Check if we can spawn a new alien
	if ((alienTimer <= 0) && (asteroids->size < 10) && (aliens->size <= 4)){
		// Get ready to spawn a new alien
		Alien* newAlien = NULL;

		// Spawn a larger alien if this is the first alien of the screen / level
		if (!firstSpawned){
			newAlien = initAlienShip(true);
			firstSpawned = true;
		}
		// Check if the lifetime score will let us spawn a small alien
		else if (lifetimeScore >= ALIEN_SMALL_SPAWN_REQ)
			newAlien = initAlienShip(rand() % 2);
		// Otherwise spawn a big alien
		else newAlien = initAlienShip(true);

		// Add the alien to the lists
		addToList(newAlien, aliens);
		alienTimer = ALIEN_SPAWN_TIME;
	}
	// Decrement the timer for alien spawn
	else if (alienTimer > 0)
		alienTimer--;

	// Update all the aliens
	for (ObjectNode* node = aliens->head; node != NULL;){
		// Check if the alien as run its course as we update it
		if (updateAlien((Alien*)node->value)){
			// Remove the alien
			Alien* a = (Alien*)node->value;
			ObjectNode* temp = node;
			node = node->next;
			if (removeFromList(temp->value, aliens))
				free(a);
		}
		// Otherwise try to make the alien shoot a missle
		else {
			alienShoot((Alien*)node->value, asteroids, p, alienShots);
			node = node->next;
		}
	}

	// Update each asteroid
	for (ObjectNode* a = asteroids->head; a != NULL; a = a->next)
		updateAsteroid((Asteroid*) a->value);

	// Update explosions
	for (ObjectNode* node = explosions->head; node != NULL;){
		Explosion* e = (Explosion*) node->value;
		// Check age
		if (e->age >= EXPLOSION_MAX_AGE){
			node = node->next;
			// Remove if it has reached its limit
			if (removeFromList(e, explosions))
				free(e);
		}
		else {
			updateExplosion(e);
			node = node->next;
		}
	}

	// Update the player ship
	updatePlayer(p);

	// Loop through all the current player missles
	for (ObjectNode* node = playerShots->head; node != NULL;){
		// Update each missle
		Missle* m = (Missle*)node->value;
		updateMissle(m);
		// Increment the remove counter if we find an old missle
		if (m->age >= MISSLE_AGE_MAX){
			Missle* temp = m;
			node = node->next;
			removeFromList(temp, playerShots);
		}
		else node = node->next;
	}

	// Loop through all the current alien missles and update them
	for (ObjectNode* node = alienShots->head; node != NULL;){
		Missle* m = (Missle*)node->value;
		updateMissle(m);
		if (m->age >= MISSLE_AGE_MAX){
			Missle* temp = m;
			node = node->next;
			removeFromList(temp, alienShots);
		}
		else node = node->next;
	}

	// Loop through all the alien missle again
	for (ObjectNode* node = alienShots->head; node != NULL;){
		// Check if a missle collides with the player
		if (detectCollidePlayerShot(p, (Missle*)node->value)){
			// Decrement the deaths left counter
			p->deathsLeft--;
			Missle* m = (Missle*)node->value;
			node = node->next;
			// Handle the collision
			handleCollidePlayerShot(p, alienShots, m, explosions);
			break;
		}
		else node = node->next;
	}

	// Loop through each asteroid in our list
	for (ObjectNode* node = asteroids->head; node != NULL;){
		bool changed = false;
		Asteroid* a = (Asteroid*) node->value;
		// Check if it collides with the player
		if (detectCollideAsteroidShip(a, p)){
			node = node->next;
			changed = true;
			handleCollideAsteroidShip(asteroids, a, p, explosions);
			p->deathsLeft--;
		}
		else if (!changed){
			// Check for collisions between aliens and asteroids
			for (ObjectNode* alienNode = aliens->head; (alienNode != NULL) && !changed; alienNode = alienNode->next){
				if (detectCollideAsteroidAlien(a, (Alien*)alienNode->value)){
					changed = true;
					node = node->next;
					Alien* alien = (Alien*) alienNode->value;
					handleCollideAsteroidAlien(asteroids, a, aliens, alien, explosions);
					break;
				}
			}
			// Loop through each missle in our list of player missles
			for (ObjectNode* missleNode = playerShots->head; (missleNode != NULL) && (node != NULL) && !changed;){
				a = (Asteroid*)node->value;
				Missle* m = (Missle*)missleNode->value;
				// Check if the missle collides with the asteroid
				if (detectCollideAsteroidShot(a, m)){
					Missle* temp = m;
					missleNode = missleNode->next;
					// Calculate score for the player
					int score = 0;
					if (a->age == 2)
						score = 20;
					else if (a->age == 1)
						score = 50;
					else if (a->age == 0)
						score = 100;
					p->score += score;
					lifetimeScore += score;
					node = node->next;
					changed = true;
					handleCollideAsteroidShot(asteroids, a, playerShots, temp, explosions);

					break;
				}
				else missleNode = missleNode->next;
			}
			// Check all alien shots
			for (ObjectNode* missleNode = alienShots->head; (missleNode != NULL) && (node != NULL) && !changed;){
				a = (Asteroid*)node->value;
				Missle* m = (Missle*)missleNode->value;
				// Check if the missle collides with the asteroid
				if (detectCollideAsteroidShot(a, m)){
					Missle* temp = m;
					missleNode = missleNode->next;
					node = node->next;
					changed = true;
					handleCollideAsteroidShot(asteroids, a, alienShots, temp, explosions);

					break;
				}
				else missleNode = missleNode->next;
			}
		}
		// Check if a collision happened
		if (!changed && (node != NULL))
			node = node->next;
	}

	// Loop through all the aliens
	for (ObjectNode* alienNode = aliens->head; alienNode != NULL;){
		bool changed = false;
		Alien* a = (Alien*)alienNode->value;
		// Check if the player and alien collide
		if (detectCollideAlienPlayer(a, p)){
			changed = true;
			alienNode = alienNode->next;
			// Handle the collision and update the player's deaths left
			handleCollideAlienPlayer(aliens, a, p, explosions);
			p->deathsLeft--;
		}
		if (!changed){
			// Check if player shots hit the aliens
			for (ObjectNode* missleNode = playerShots->head; (missleNode != NULL) && !changed && (alienNode != NULL);){
				if (detectCollideAlienShot(a, (Missle*)missleNode->value)){
					changed = true;
					Missle* m = (Missle*)missleNode->value;
					missleNode = missleNode->next;
					alienNode = alienNode->next;

					// Calculate score
					int score = 0;
					if (a->isBig)
						score = 200;
					else score = 1000;
					p->score += score;
					lifetimeScore += score;
					
					// Handle the collision
					handleCollideAlienShot(aliens, a, playerShots, m, explosions);

					break;
				}
				else missleNode = missleNode->next;
			}
		}
		// Check if anything happened
		if (!changed && (alienNode != NULL))
			alienNode = alienNode->next;
	}

	// See if the player has scored enough for a new life
	if (p->score >= NEW_LIFE_REQ){
		p->score -= NEW_LIFE_REQ;
		p->deathsLeft++;
	}

	// Check for no asteroids
	if (asteroids->size == 0){
		// Reset game
		if(num_asteroids < MAX_NUM_ASTEROIDS)
			num_asteroids++;
		initAsteroidList(asteroids);
		p->positionVector[X_] = PLAYER_INIT_POSX;
		p->positionVector[Y_] = PLAYER_INIT_POSY;
		p->spin = 0.0f;
		clearList(playerShots);
		clearList(alienShots);
		clearList(aliens);
		alienTimer = ALIEN_SPAWN_TIME;
		firstSpawned = false;
	}
	// See if the player is out of lives
	if (p->deathsLeft <= -1)
		restartGame();

	// Redraw and wait again
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

/*
	Draw the 3D scene
*/
void drawScene()
{
	glEnable(GL_LIGHTING);
	// Clear information from the last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Switch to the drawing perspective
	glMatrixMode(GL_MODELVIEW);
	// Reset the perspective
	glLoadIdentity();

	
	// Loop through all the alien ships
	for (ObjectNode* node = aliens->head; node != NULL; node = node->next){
		// Save the matrix state
		glPushMatrix();
			// Get the next alien
			Alien* a = (Alien*)node->value;
			// Move and rotate to it
			glTranslatef(a->positionVector[X_], a->positionVector[Y_], a->positionVector[Z_]);
			glRotatef(90, 1.0f, 0.0f, 0.0f);
			glRotatef(a->spin, a->orientation[X_], a->orientation[Y_], a->orientation[Z_]);
			// Set its material
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, a->mat);
			// Draw the alien ship with a torus and a sphere, making a saturn-y looking thing
			glutSolidSphere(a->sphereRadius, a->sphereSlices, a->sphereStacks);
			glutSolidTorus(a->torusInnerRadius, a->torusOuterRadius, a->torusSides, a->torusRings);
		// Reload matrix state
		glPopMatrix();
	}
	

	// Loop through all the asteroids
	for (ObjectNode* node = asteroids->head; node != NULL; node = node->next){
		// Save matrix state for each asteroid
		glPushMatrix();
		Asteroid* a = (Asteroid*) node->value;
			// Move to the asteroids position
			glTranslatef(a->positionVector[X_], a->positionVector[Y_], a->positionVector[Z_]);
			// Rotate to the asteroids orientation
			glRotatef(a->spin, a->orientation[X_], a->orientation[Y_], a->orientation[Z_]);
			// Scale to the asteroid's size
			glScalef(a->scale[X_], a->scale[Y_], a->scale[Z_]);
			// Use the asteroids material
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, a->mat);
			// Draw all the triangles in the asteroid
			glBegin(GL_TRIANGLES);
			for (int j = 0; (j + 2) < a->numVerticies; j += 3){
				// Draw each triangle and its normal
				for (int k = 0; k < 3; k++){
					glNormal3fv(a->normals[j / 3]);
					glVertex3fv(a->verticies[j + k]);
				}
			}
			glEnd();
		glPopMatrix();
	}

	// Save the matrix before we draw the player ship
	glPushMatrix();
		// Move to the players position
		glTranslatef(p->positionVector[X_], p->positionVector[Y_], p->positionVector[Z_]);
		// Rotate the ship back on the X axis (make it look flat)
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		// Rotate on the Y axis (point it to the right)
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		// Rotate the ship to its orientation
		glRotatef(p->spin, p->orientation[X_], p->orientation[Y_], p->orientation[Z_]);
		// Scale the ship to be smaller
		glScalef(p->scale[X_], p->scale[Y_], p->scale[Z_]);
		// Draw all the triangles in the player ship
		glBegin(GL_TRIANGLES);
		// Set the material
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p->mat);
		for (int i = 0; (i + 2) < p->numVerticies;  i += 3){
			// Draw each triangle and its normal
			for (int j = 0; j < 3; j++){
				glNormal3fv(p->normals[i / 3]);
				glVertex3fv(p->verticies[i + j]);
			}
		}
		glEnd();
	glPopMatrix();

	// Disable lighting for drawing points and text
	glDisable(GL_LIGHTING);
	// Setup for drawing missles
	glPointSize(MISSLE_SIZE);

	// Draw all the players missles
	for (ObjectNode* node = playerShots->head; node != NULL; node = node->next){
		// Save the matrix
		glPushMatrix();
			// Get the current missle
			Missle* m = (Missle*) node->value;
			// Set its color
			glColor3f(0, 1, 1);
			// Draw the point and its normal
			glBegin(GL_POINTS);
			glVertex3f(m->positionVector[X_], m->positionVector[Y_], m->positionVector[Z_]);
			glEnd();
		glPopMatrix();
	}

	// Draw all the alien missles
	for (ObjectNode* node = alienShots->head; node != NULL; node = node->next){
		// Save the matrix
		glPushMatrix();
			// Get the current missle
			Missle* m = (Missle*)node->value;
			// Set its color
			glColor3f(1, 0, 0);
			// Draw the point and its normal
			glBegin(GL_POINTS);
			glVertex3f(m->positionVector[X_], m->positionVector[Y_], m->positionVector[Z_]);
			glEnd();
		glPopMatrix();
	}

	// Setup for drawing explody bits
	glPointSize(EXPLOSION_PT_SIZE);

	// Draw all the explosions
	for (ObjectNode* e = explosions->head; e != NULL; e = e->next){
		// Save the matrix
		glPushMatrix();
			// Get the explosion
			Explosion* exp =  (Explosion*) e->value;
			// Set color and move to the explosion
			glColor3f(1, 1, 1);
			glTranslatef(exp->positionVector[X_], exp->positionVector[Y_], exp->positionVector[Z_]);
			// Draw all the points
			glBegin(GL_POINTS);
			for (int i = 0; i < EXPLOSION_NUM_PTS; i++)
				glVertex3f(exp->points[i][X_], exp->points[i][Y_], exp->points[i][Z_]);
			glEnd();

		glPopMatrix();
	}

	// Draw text
	glPushMatrix();
		glColor3f(1, 1, 1);
		// Make buffers for drawing text
		char numberBuffer[32];
		char text[512] = {};
		// Add the score label
		strcat(text, "SCORE: ");
		// Convert the player's score to text and add it onto the string
		itoa(p->score, numberBuffer, 10);
		strcat(text, numberBuffer);

		// Add the deaths left label
		strcat(text, "    DEATHS LEFT: ");
		// Be good and clear the buffer
		for (int i = 0; i < 32; i++)
			numberBuffer[i] = '\0';
		// Convert the player's deaths left to text and add it onto the string
		itoa(p->deathsLeft, numberBuffer, 10);
		strcat(text, numberBuffer);

		// Draw the string
		drawText(-5.0, 5.0, Z_LEVEL, text);
	glPopMatrix();

	glFlush();
	//Send the 3D scene to the window
	glutSwapBuffers();
}

/*
	This is a simple function for drawing text to the window.
	@param x The x co-ordinate for where we should start drawing.
	@param y The y co-ordinate for where we should start drawing.
	@param z The z depth co-ordinate for where we should draw.
	@param string The string to draw.
 */
void drawText(float x, float y, float z, char* string)
{
	glRasterPos3f(x, y, z);
	for (char* p = string; *p != '\0'; p++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
}

/*
	This function handles interaction with a right click menu.
	@param ID Menu entry ID.
 */
void handle_menu(int ID)
{
	switch (ID) {
	case 0:
		// Leave if the Quit entry is clicked
		exit(0);
	case 1:
		// Restart the game
		restartGame();
		break;
	}
}

/*
	Called when the window is resized.
	@param w The new screen width.
	@param h The new screen height.
 */
void handleResize(int w, int h)
{
	// Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, w, h);
	// Switch to setting camera perspective
	glMatrixMode(GL_PROJECTION);
	// Set the camera perspective
	glLoadIdentity();
	// Pass in camera angle, width-to-height ratio, the near z clipping coordinate, and the far z clipping coordinate
	gluPerspective(45.0, (double) w /(double) h, 1.0, 200.0);

}

/*
	Called when a key is pressed.
	@param key The key that was pressed.
	@param x The mouse's x coordinate.
	@param y The mouse's y coordinate.
*/
void handleKeypress(unsigned char key, int x, int y)
{
	Missle* newM = NULL;
	switch (key)
	{
		// Exit on escape press
	case 27:
		exit(EXIT_SUCCESS);
		// Set the X flag in the objects module
	case 'x':
		setKeyX(true);
		break;
	case 'X':
		setKeyX(true);
		break;
		// Try to fire a shot and add it to the queue
	case'z':
		newM = fireShot(p);
		if (newM != NULL)
			addToList(newM, playerShots);
		break;
	case'Z':
		newM = fireShot(p);
		if (newM != NULL)
			addToList(newM, playerShots);
		break;
	}
}

/*
	Called when a key is released.
	@param key The key that was pressed.
	@param x The mouse's x coordinate.
	@param y The mouse's y coordinate.
*/
void handleKeyRelease(unsigned char key, int x, int y)
{
	switch (key)
	{
		// Unset the X flag in the objects module
	case 'x':
		setKeyX(false);
		break;
	case 'X':
		setKeyX(false);
		break;
	}
}

/*
	Called when a special key is pressed.
	@param key The special that was pressed.
	@param x The mouse's x coordinate.
	@param y The mouse's y coordinate.
*/
void handleSpecialPress(int key, int x, int y)
{
	switch (key)
	{
		// Set the flag for a right keypress in the objects module
	case GLUT_KEY_LEFT:
		setKeyLeft(true);
		break;
		// Set the flag for a left keypress in the objects module
	case GLUT_KEY_RIGHT:
		setKeyRight(true);
		break;
	}
}

/*
	Called whenever a special key is released.
	@param key The key that was released.
	@param x The mouse's x coordinate.
	@param y The mouse's y coordinate.
*/
void handleSpecialRelease(int key, int x, int y)
{
	switch (key)
	{
		// Unset the flag for a left keypress in the objects module
	case GLUT_KEY_LEFT:
		setKeyLeft(false);
		break;
		// Unset the flag for a right keypress in the objects module
	case GLUT_KEY_RIGHT:
		setKeyRight(false);
		break;
	}
}

/*
	This initializes lighting magic.
	Its pretty much all stuff from the demo program.
*/
void initLighting()
{
	// Ambient material property
	float  amb[] = { 0, 0, 0, 1 };
	// Ambient light property
	float  lt_amb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	// Ambient light property
	float  lt_dif[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	// Light position
	float  lt_pos[] = { 0, .39392, .91914, 0 };
	// Specular light property
	float  lt_spc[] = { 0, 0, 0, 1 };

	// Set default ambient light in scene
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

	// Set Light 0 position, ambient, diffuse, specular intensities
	glLightfv(GL_LIGHT0, GL_POSITION, lt_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lt_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lt_dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lt_spc);

	// Enable Light 0 and GL lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	// Flat shading
	glShadeModel(GL_FLAT);
	// Normalize normals
	glEnable(GL_NORMALIZE);

	// Setup background colour
	glClearDepth(1.0);
	glClearColor(0, 0, 0, 0);
}

/*
	This function initializes a given array of asteroids randomly according to the specified rules.
	@param a The reference to an array of Asteroid pointers.
*/
void initAsteroidList(ObjectList* list)
{
	// Loop through the whole array
	for (int i = 0; i < num_asteroids; i++){
		// Create an asteroid
		Asteroid* a = initAsteroid();

		// Randomly set its spin
		a->spin = (float) (rand() % 90);
		// Randomly set the spin factor
		GLfloat spinFactor;
		for (spinFactor = 100.0f; (spinFactor < -2.0) || (spinFactor > 2.0) || (spinFactor == -0.0f) || (spinFactor == 0.0f); spinFactor = (float)-1 * (rand() % 2) * ((float)3 / (rand() % 15)));
			a->spinFactor = spinFactor;
		
		// Randomly generate and set scale
		GLfloat scale;
		for (scale = 0.0f; (scale < 0.4f) || (scale > 0.9f); scale = (float)1 / (rand() % 10));
		for (int j = 0; j < 3; j++)
			a->scale[j] = scale;

		// Randomly pick an axis to spin about
		a->orientation[rand() % 3] = 1.0f;

		for (int j = 0; j < 2; j++){
			// Randomly generate a velocity
			GLfloat vel;
			for (vel = 0.0f; (vel < -0.06f) || (vel > 0.06f) || (vel == 0.0f) || (vel == -0.0f); vel = (float)1 / (10 + (25 + rand() % 100)));
			
			// Alternal direction
			if (i < (num_asteroids / 2))
				a->vVector[j] = vel;
			else
				a->vVector[j] = -vel;

			// Randomly generate a starting position
			GLfloat pos;
			for (pos = -10.f; (pos < -6.0f) || (pos > 6.0f) || (pos == -0.00); pos = (float)-1 * (rand() % 2) * (rand() % 7));
			a->positionVector[j] = pos;
		}
		// Set the Z value
		a->positionVector[Z_] = Z_LEVEL;

		addToList(a, list);
	}
}

/*
	This function restarts the game as if it had just been launched.
 */
void restartGame()
{
	// Clear all the lists
	clearList(asteroids);
	clearList(playerShots);
	clearList(explosions);
	clearList(aliens);
	clearList(alienShots);

	// Reset the asteroid count and remake the list of asteroids
	num_asteroids = 1;
	initAsteroidList(asteroids);
	
	// Reset the player
	p->positionVector[X_] = PLAYER_INIT_POSX;
	p->positionVector[Y_] = PLAYER_INIT_POSY;
	p->spin = 0.0f;
	p->score = 0;
	lifetimeScore = 0;
	p->deathsLeft = PLAYER_DEATHS_INIT;

	// Reset stuff for spawning alienss
	alienTimer = ALIEN_SPAWN_TIME;
	firstSpawned = false;
}
