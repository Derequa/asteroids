#include "GL/glut.h"
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <time.h>
#include "objects.h"
#include "datastructures.h"

/**
    @file objects.cpp
    @author Derek Batts - dsbatts@ncsu.edu
    This module contains functions for updating objects/data given in objects.h
*/

//Because there is no pi in zmath :(
#define PI 3.14159265
#define SHOOT_RANDOM 0
#define SHOOT_ASTEROID 1
#define SHOOT_PLAYER 2

//A flag for when the left key is pressed
static bool leftKeyPressed = false;
//A flag for when the right key is pressed
static bool rightKeyPressed = false;
//A flag for when the X button is pressed
static bool xKeyPressed = false;
//THe material for all missles
float missleMat[] = { 1.0f, 1.0f, 1.0f, 0.0f };

/*
	This function handles an alien shooting at various objects in the game world.
	@param alien The alien doing the shooting.
	@param asteroids The list of asteroid in the game.
	@param p The player in the game.
	@param alienShots The list of alien shots.
 */
void alienShoot(Alien* alien, ObjectList* asteroids, PlayerShip* p, ObjectList* alienShots)
{
	// Check if we can shoot
	if (alien->missleCoolDown > 0)
		return;
	// Randomly picka number between 1 and 10 and create a flag
	int result = (rand() % 10) + 1;
	int whatDoFlag = -1;
	// Check the size of the alien
	if (alien->isBig){
		// Determine what to shoot at based on the number picked
		if ((result > 5) && (result <= 10))
			whatDoFlag = SHOOT_RANDOM;
		else if ((result == 5) || (result == 4))
			whatDoFlag = SHOOT_PLAYER;
		else whatDoFlag = SHOOT_ASTEROID;
	}
	else{
		if ((result > 5) && (result <= 10))
			whatDoFlag = SHOOT_PLAYER;
		else if (result == 5)
			whatDoFlag = SHOOT_RANDOM;
		else whatDoFlag = SHOOT_ASTEROID;
	}

	// Make a new missle object
	Missle* m = (Missle*)malloc(sizeof(Missle));
	// Set up its fields
	m->age = 0;
	for (int i = 0; i < 3; i++){
		m->positionVector[i] = alien->positionVector[i];
		m->mat[i] = missleMat[i];
	}
	m->mat[3] = missleMat[3];
	m->vMag = MISSLE_V;

	// Check if we are shooting randomly
	if (whatDoFlag == SHOOT_RANDOM){
		// Randomly create a vector
		float x = (float)1 * (rand() % 15);
		float y = (float)1 * (rand() % 15);
		float mag = sqrt((x * x) + (y * y));
		// Make it a unit vector
		x = x / mag;
		y = y / mag;
		// Randomly choose to make its components negative
		if (rand() % 2)
			x = -x;
		if (rand() % 2)
			y = -y;
		// Set the missle's direction vector
		m->directionVector[X_] = x;
		m->directionVector[Y_] = y;
	}
	// Check if we are shooting at an asteroid
	else if (whatDoFlag == SHOOT_ASTEROID){
		// Look for the closest asteroid
		float closestDist = 100;
		Asteroid* closest = NULL;
		for (ObjectNode* node = asteroids->head; node != NULL; node = node->next){
			// Get the asteroid and determine the distance
			Asteroid* a = (Asteroid*)node->value;
			float dx = abs(a->positionVector[X_] - alien->positionVector[X_]);
			float dy = abs(a->positionVector[Y_] - alien->positionVector[Y_]);
			float dist = sqrt((dx * dx) + (dy * dy));
			// Remember it if its the closest one yet
			if (dist < closestDist)
				closest = a;
		}

		// Determine the vector pointing from the alien to the asteroid
		float nx = (alien->positionVector[X_] - closest->positionVector[X_]);
		float ny = (alien->positionVector[Y_] - closest->positionVector[Y_]);
		float mag = sqrt((nx * nx) + (ny * ny));
		// Set the direction of the missle with the unit vector of the vector we calculated
		m->directionVector[X_] = -nx / mag;
		m->directionVector[Y_] = -ny / mag;
	}
	// Check if we are shooting a player
	else if (whatDoFlag == SHOOT_PLAYER){
		// Calculate a vector pointing from the alien to the player
		float nx = alien->positionVector[X_] - p->positionVector[X_];
		float ny = alien->positionVector[Y_] - p->positionVector[Y_];
		float mag = sqrt((nx * nx) + (ny * ny));
		// Set the direction of the missle with the unit vector of the vector we calculated
		m->directionVector[X_] = -nx / mag;
		m->directionVector[Y_] = -ny / mag;
	}

	// Add the shot to the alien's list of shots
	addToList(m, alienShots);
	// Reset its cooldown
	alien->missleCoolDown = ALIEN_COOLDOWN;

}

/*
	This function updates a given alien according to its fields.
	It also check if the alien has crossed a horizontal wall and will signal this on return.
	@param a The alien to update.
	@return True if this alien should still exist, false if it needs to be removed
 */
bool updateAlien(Alien* a)
{
	// Update the position vector by the velocity vector
	for (int i = 0; i < 2; i++)
		a->positionVector[i] += a->vVector[i];
	// Update the spin by the speed
	a->spin += a->spinSpeed;
	// Decrement the cooldown if it can be
	if (a->missleCoolDown != 0)
		a->missleCoolDown -= PLAYER_CD_DELTA;

	// Check if we should change direction in Y
	if ((a->directionTimer <= 0) && !(rand() % 9)){
		// Change y velocity
		GLfloat vel;
		for (vel = 0.0f; (vel < -0.06f) || (vel > 0.06f) || (vel == 0.0f) || (vel == -0.0f); vel = (float)1 / (10 + (25 + rand() % 100)));
		// Check if the new velocity is in the same direction as the current
		if ((vel > 0) && (a->vVector[Y_] > 0))
			// Change it if so
			vel = -vel;
		else if ((vel < 0) && (a->vVector[Y_] < 0))
			vel = -vel;
		// Set the new velocity and reset the timer
		a->vVector[Y_] = vel;
		a->directionTimer = ALIEN_DIR_TIMER;
	}
	// Decrement the direction change timer
	else if(a->directionTimer > 0)
		a->directionTimer--;

	// Wrap the spin
	if (a->spin >= 360)
		a->spin -= 360;
	else if (a->spin < 0)
		a->spin += 360;
	
	// Initialize a flag for whether or not the asteroid is finished
	bool finished = false;

	// Wrap around the edges of the window
	if (a->positionVector[X_] > BOUND_X_UPPER){
		a->positionVector[X_] -= 2 * BOUND_X_UPPER;
		// If we wrap on an x wall this alien is done living
		finished = true;
	}
	else if (a->positionVector[X_] < BOUND_X_LOWER){
		a->positionVector[X_] -= 2 * BOUND_X_LOWER;
		finished = true;
	}
	if (a->positionVector[Y_] > BOUND_Y_UPPER)
		a->positionVector[Y_] -= 2 * BOUND_Y_UPPER;
	else if (a->positionVector[Y_] < BOUND_Y_LOWER)
		a->positionVector[Y_] -= 2 * BOUND_Y_LOWER;

	// Return whether or not we wrapped
	return finished;
}

/*
	This function initializes an Alien ship with size determined by
	the given parameter.
	@param makeBig True if we are making a big alien ship, false if we are making a small one.
	@return A pointer to the alien we made.
 */
Alien* initAlienShip(bool makeBig)
{
	// Allocate space for the new alien
	Alien* a = (Alien*)malloc(sizeof(Alien));
	// Set fields
	a->missleCoolDown = ALIEN_COOLDOWN;
	a->directionTimer = ALIEN_DIR_TIMER;

	//Randomly pick position
	if (rand() % 2){
		a->positionVector[X_] = BOUND_X_LOWER + 0.5f;
		a->positionVector[Y_] = (float)-1 * (rand() % 4);
		//Set X velocity
		if (makeBig)
			a->vVector[X_] = ALIEN_LARGE_V_X;
		else
			a->vVector[X_] = ALIEN_SMALL_V_X;
	}
	else {
		a->positionVector[X_] = BOUND_X_UPPER - 0.5f;
		a->positionVector[Y_] = (float)1 * (rand() % 4);
		//Set X velocity
		if (makeBig)
			a->vVector[X_] = -ALIEN_LARGE_V_X;
		else
			a->vVector[X_] = -ALIEN_SMALL_V_X;
	}
	
	// Set the Z depth to the level we are working on
	a->positionVector[Z_] = -14.0f;

	//Set orientation
	a->orientation[X_] = 0.0f;
	a->orientation[Y_] = 0.0f;
	a->orientation[Z_] = 1.0f;
	a->spin = 0.0f;
	a->spinSpeed = 1.3f;

	//Set material
	a->mat[0] = 0.0f;
	a->mat[1] = 1.0f;
	a->mat[2] = 1.0f;
	a->mat[3] = 0.0f;

	//Randomly set Y velocity
	GLfloat vel;
	for (vel = 0.0f; (vel < -0.06f) || (vel > 0.06f) || (vel == 0.0f) || (vel == -0.0f); vel = (float)1 / (10 + (25 + rand() % 100)));
	if (rand() % 2)
		vel = -vel;
	a->vVector[Y_] = vel;
	
	// Set sphere and torus values based on size
	if (makeBig){
		a->isBig = true;
		a->sphereRadius = ALIEN_LARGE_SPH_R;
		a->sphereSlices = ALIEN_LARGE_SPH_SL;
		a->sphereStacks = ALIEN_LARGE_SPH_ST;
		a->torusInnerRadius = ALIEN_LARGE_TOR_IN_R;
		a->torusOuterRadius = ALIEN_LARGE_TOR_OUT_R;
		a->torusRings = ALIEN_LARGE_TOR_RINGS;
		a->torusSides = ALIEN_LARGE_TOR_SIDE;
	}
	else {
		a->isBig = false;
		a->sphereRadius = ALIEN_SMALL_SPH_R;
		a->sphereSlices = ALIEN_SMALL_SPH_SL;
		a->sphereStacks = ALIEN_SMALL_SPH_ST;
		a->torusInnerRadius = ALIEN_SMALL_TOR_IN_R;
		a->torusOuterRadius = ALIEN_SMALL_TOR_OUT_R;
		a->torusRings = ALIEN_SMALL_TOR_RINGS;
		a->torusSides = ALIEN_SMALL_TOR_SIDE;
	}

	// Return what we made
	return a;
}

/*
	This function creates an explosion object at the given location.
	@param x The x co-ordinate for where to create explosion.
	@param y The y co-ordinate for where to create explosion.
	@param z The z co-ordinate for where to create explosion.
	@return A pointer to the explosion we made.
 */
Explosion* makeExplosion(GLfloat x, GLfloat y, GLfloat z)
{
	// Allocate space for the explosion object
	Explosion* e = (Explosion*)malloc(sizeof(Explosion));
	// Set some fields
	e->vMag = EXPLOSION_V;
	e->age = 0;
	e->positionVector[X_] = x;
	e->positionVector[Y_] = y;
	e->positionVector[Z_] = z;
	// Set the material
	for (int i = 0; i < 4; i++)
		e->mat[i] = missleMat[i];
	// Set each point's origin
	for (int i = 0; i < EXPLOSION_NUM_PTS; i++){
		e->points[i][X_] = x;
		e->points[i][Y_] = y;
		e->points[i][Z_] = z;
	}
	// Set the directions for all the explosion points
	e->directions[0][X_] = 1.0f;
	e->directions[0][Y_] = 0.0f;

	e->directions[1][X_] = 1.0f;
	e->directions[1][Y_] = 1.0f;

	e->directions[2][X_] = 0.0f;
	e->directions[2][Y_] = 1.0f;

	e->directions[3][X_] = -1.0f;
	e->directions[3][Y_] = 1.0f;

	e->directions[4][X_] = -1.0f;
	e->directions[4][Y_] = 0.0f;

	e->directions[5][X_] = -1.0f;
	e->directions[5][Y_] = -1.0f;

	e->directions[6][X_] = 0.0f;
	e->directions[6][Y_] = -1.0f;

	e->directions[7][X_] = 1.0f;
	e->directions[7][Y_] = -1.0f;

	// We done
	return e;
}

/*
	This function updates an explosion.
	@param e The explosion to update.
 */
void updateExplosion(Explosion* e)
{
	// Increment the age
	e->age++;

	// Update each point by its vMag and direction vector.
	for (int i = 0; i < EXPLOSION_NUM_PTS; i++){
		e->points[i][X_] += e->vMag * e->directions[i][X_];
		e->points[i][Y_] += e->vMag * e->directions[i][Y_];
	}
}

/**
	This functions handles generating a missle for
	a given player
	@param p The pointer to the player ship to generate missles for
	@return The missle fired, or NULL if one cannot be fired
*/
Missle* fireShot(PlayerShip* p)
{
	// Check the player's cooldown
	if (p->missleCoolDown > 0)
		return NULL;

	// Allocate space for a new missle
	Missle* ret = (Missle*)malloc(sizeof(Missle));

	// Set its age to zero
	ret->age = 0;
	for (int i = 0; i < 2; i++){
		// Set its position to the player's ship
		ret->positionVector[i] = p->positionVector[i];
		// Set the direction to the player's ship's
		ret->directionVector[i] = p->directionUnitVector[i];
		// Set the material
		ret->mat[i] = missleMat[i];
	}

	// Set the z position
	ret->positionVector[Z_] = p->positionVector[Z_];
	// Set the material
	ret->mat[3] = missleMat[3];
	ret->mat[4] = missleMat[4];
	// Set the missle's velocity to the velocity constant plus the player's
	ret->vMag = MISSLE_V + p->vMag;
	// Initialize the player's cooldown
	p->missleCoolDown = PLAYER_COOLDOWN;
	return ret;
}

/**
	This function handles updating a given missle's parameters
	@param m A pointer to the missle to update
*/
void updateMissle(Missle* m)
{
	// Check the age, only update if it is young enough
	if (m->age >= MISSLE_AGE_MAX)
		return;

	// Increment the age
	m->age += MISSLE_AGE_DELTA;
	// Update the position based on direction and velocity
	m->positionVector[X_] += m->vMag * m->directionVector[X_];
	m->positionVector[Y_] += m->vMag * m->directionVector[Y_];

	// Wrap around the screen
	if (m->positionVector[X_] > BOUND_X_UPPER)
		m->positionVector[X_] -= 2 * BOUND_X_UPPER;
	else if (m->positionVector[X_] < BOUND_X_LOWER)
		m->positionVector[X_] -= 2 * BOUND_X_LOWER;
	if (m->positionVector[Y_] > BOUND_Y_UPPER)
		m->positionVector[Y_] -= 2 * BOUND_Y_UPPER;
	else if (m->positionVector[Y_] < BOUND_Y_LOWER)
		m->positionVector[Y_] -= 2 * BOUND_Y_LOWER;
}

/**
	This function calculates a normal vector for a triangle with 3 given vectors
	@param v0 The first vertex drawn in the triangle
	@param v1 The second vertex drawn in the traiangle
	@param v2 The third vertex drawn in the triangle
	@param normDest The reference to where to store the normal
*/
void calculateNormal(GLfloat(&v0)[3], GLfloat(&v1)[3], GLfloat(&v2)[3], GLfloat(&normDest)[3])
{
	// Calculate normal and store in normDest
	GLfloat d0[] = { v1[X_] - v0[X_], v1[Y_] - v0[Y_], v1[Z_] - v0[Z_] };
	GLfloat d1[] = { v2[X_] - v1[X_], v2[Y_] - v1[Y_], v2[Z_] - v1[Z_] };
	normDest[X_] = (d0[Y_] * d1[Z_]) - (d0[Z_] * d1[Y_]);
	normDest[Y_] = (d0[Z_] * d1[X_]) - (d0[X_] * d1[Z_]);
	normDest[Z_] = (d0[X_] * d1[Y_]) - (d0[Y_] * d1[X_]);
}

/**
	This function updates a player ship's position and stuff
	@param p The player ship to update
*/
void updatePlayer(PlayerShip* p)
{
	// Decrement the cooldown if needed
	if (p->missleCoolDown > 0)
		p->missleCoolDown -= PLAYER_CD_DELTA;

	// If we are not at max velocity and the acceleration key is down, we accelerate
	if ((p->vMag < MAX_PLAYER_V) && (xKeyPressed))
		p->vMag += PLAYER_A;

	// If the left key is down spin counter-clockwise
	if (leftKeyPressed)
		p->spinFactor = PLAYER_SPIN;
	// If the right key is down spin clockwise
	else if (rightKeyPressed)
		p->spinFactor = -PLAYER_SPIN;
	// Otherwise, ensure we do not spin
	else
		p->spinFactor = 0.0f;

	// If there is a spin factor, update the spin angle (and wrap)
	if (p->spinFactor != 0)
		p->spin += p->spinFactor;
	if (p->spin > 360)
		p->spin -= 360;
	else if (p->spin < 0)
		p->spin += 360;

	// Update our direction based on the spin
	p->directionUnitVector[X_] = cos(p->spin * (PI / 180));
	p->directionUnitVector[Y_] = sin(p->spin * (PI / 180));

	// If our velocity is non-zero move along the direction vector
	if (p->vMag > 0.0f){
		p->positionVector[X_] += p->vMag * p->directionUnitVector[X_];
		p->positionVector[Y_] += p->vMag * p->directionUnitVector[Y_];
		// Slow down according to the friction constant
		p->vMag -= PLAYER_FRICTION;
		if (p->vMag < 0)
			p->vMag = 0;
	}

	// Wrap around the screen
	if (p->positionVector[X_] > BOUND_X_UPPER)
		p->positionVector[X_] -= 2 * BOUND_X_UPPER;
	else if (p->positionVector[X_] < BOUND_X_LOWER)
		p->positionVector[X_] -= 2 * BOUND_X_LOWER;
	if (p->positionVector[Y_] > BOUND_Y_UPPER)
		p->positionVector[Y_] -= 2 * BOUND_Y_UPPER;
	else if (p->positionVector[Y_] < BOUND_Y_LOWER)
		p->positionVector[Y_] -= 2 * BOUND_Y_LOWER;
}

/**
	This function initialized a player ship
	@return The player ship created
*/
PlayerShip* initPlayer()
{
	// Allocate space for a player ship
	PlayerShip* ret = (PlayerShip*)malloc(sizeof(PlayerShip));
	// Copy the ship verticies into the new ship object
	memcpy(ret->verticies, spaceShip, NUM_SHIP_VERTS * 3 * sizeof(GLfloat));
	// Set the number of verticies
	ret->numVerticies = NUM_SHIP_VERTS;

	// Calculate the normal for each triangle
	for (int i = 0; i < NUM_SHIP_VERTS; i += 3)
		calculateNormal(ret->verticies[i], ret->verticies[i + 1], ret->verticies[i + 2], ret->normals[i/3]);
	// Set the number of normals
	ret->numNorms = NUM_SHIP_NORMS;

	// Set the ship material
	ret->mat[0] = 0.0f;
	ret->mat[1] = 0.0f;
	ret->mat[2] = 1.0f;
	ret->mat[3] = 0.0f;

	// Initialize the ship's position
	for (int i = 0; i < 3; i++){
		ret->positionVector[i] = 0.0f;
		ret->orientation[i] = 0.0f;
		ret->scale[i] = PLAYER_SIZE;
	}

	// Initialize the ship's direction
	ret->directionUnitVector[X_] = 1.0f;
	ret->directionUnitVector[Y_] = 0.0f;

	// Initialize the magnitudes for velocity, acceleration, spin, and spin factor (velocity)
	ret->vMag = 0.0f;
	ret->aMag = 0.0f;
	ret->spin = 0.0f;
	ret->spinFactor = 0.0f;
	ret->score = 0;
	ret->deathsLeft = PLAYER_DEATHS_INIT;
	// Initialize missle cooldown
	ret->missleCoolDown = 0;

	// Return the ship
	return ret;
}

/**
	This function updates a given asteroid based on its properties
	@param a A pointer to the asteroid to update
*/
void updateAsteroid(Asteroid* a)
{
	// Move the asteroid by it's velocity
	a->positionVector[X_] += a->vVector[X_];
	a->positionVector[Y_] += a->vVector[Y_];

	// Spin the asteroid by its spin factor (velocity)
	a->spin += a->spinFactor;
	// Wrap the angle
	if (a->spin > 360.0f)
		a->spin -= 360.0f;
	else if (a->spin < 0.0f)
		a->spin += 360.0f;

	// Wrap the asteroid around the window
	if (a->positionVector[X_] > BOUND_X_UPPER)
		a->positionVector[X_] -= 2 * BOUND_X_UPPER;
	else if (a->positionVector[X_] < BOUND_X_LOWER)
		a->positionVector[X_] -= 2 * BOUND_X_LOWER;
	if (a->positionVector[Y_] > BOUND_Y_UPPER)
		a->positionVector[Y_] -= 2 * BOUND_Y_UPPER;
	else if (a->positionVector[Y_] < BOUND_Y_LOWER)
		a->positionVector[Y_] -= 2 * BOUND_Y_LOWER;
}


/**
	This function creates and returns an asteroid
	@return The created asteroid
*/
Asteroid* initAsteroid()
{
	// Allocate space for the asteroid
	Asteroid* ret = (Asteroid*)malloc(sizeof(Asteroid));
	// Create a 2D array for all the verticies in the asteroid
	GLfloat vertsCpy[NUM_UNIQUE_SPH_PTS][3];
	// Copy and roughed all the verticies in the unit sphere into vertsCpy
	roughenSphere(vertsCpy, verts);
	// Order all the verticies so we can draw the asteroid with triangles
	updateRoughSphere(vertsCpy, ret->verticies);
	// Set the number of verticies
	ret->numVerticies = NUM_SPHERE_VERTS;

	// Calculate normals for all the triangles
	for (int i = 0; i < NUM_SPHERE_VERTS ; i += 3)
		calculateNormal(ret->verticies[i], ret->verticies[i + 1], ret->verticies[i + 2], ret->normals[i/3]);
	// Set the number of normals
	ret->numNorms = NUM_SPHERE_NORMS;
	
	// Set the material
	ret->mat[0] = 0.9f;
	ret->mat[1] = 0.0f;
	ret->mat[2] = 0.9f;
	ret->mat[3] = 0.0f;

	// Initialize orientation, position, and velocity
	for (int i = 0; i < 3; i++){
		ret->orientation[i] = 0.0f;
		ret->positionVector[i] = 0.0f;
		ret->scale[i] = 1.0f;
		if (i < 2)
			ret->vVector[i] = 0.0f;
	}

	// Initialize spin, spin factor, and age
	ret->spin = 0.0f;
	ret->spinFactor = 0.0f;
	ret->age = 2;

	// Return the asteroid
	return ret;
}

/**
	This function set the global for left key pressed
	@param The boolean value to set it to
*/
void setKeyLeft(bool isDown)
{
	leftKeyPressed = isDown;
}

/**
	This function set the global for right key pressed
	@param The boolean value to set it to
*/
void setKeyRight(bool isDown)
{
	rightKeyPressed = isDown;
}

/**
	This function set the global for X key pressed
	@param The boolean value to set it to
*/
void setKeyX(bool isDown)
{
	xKeyPressed = isDown;
}

/**
	This function randomly disturbs verticies on the sphere
*/
void roughenSphere(GLfloat(&verts)[NUM_UNIQUE_SPH_PTS][3], const GLfloat(&srcVerts)[NUM_UNIQUE_SPH_PTS][3])
{
	// For through all the unique verticie
	for (int i = 0; i < NUM_UNIQUE_SPH_PTS; i++){
		for (int j = 0; j < 3; j++){
			// For each vertex coordinate, generate a random delta and move the coordinate that much
			float d = (float)(rand() % 10) / (float) 24.0;
			verts[i][j] = srcVerts[i][j] + d;
		}
	}
}

/**
	This function updates the sphere with the disturbed verticies
*/
void updateRoughSphere(GLfloat(&verts)[NUM_UNIQUE_SPH_PTS][3], GLfloat(&dest)[NUM_SPHERE_VERTS][3])
{
	// All the unique verticies are in the 2D verts array
	// We build a sphere using though verticies
	// If they have been disturbed then the sphere will look disturbed
	GLfloat roughSphereTemplate[NUM_SPHERE_VERTS][3] = {
		// Triangles for top quater
		{ verts[0][0], verts[0][1], verts[0][2] },
		{ verts[1][0], verts[1][1], verts[1][2] },
		{ verts[2][0], verts[2][1], verts[2][2] },

		{ verts[0][0], verts[0][1], verts[0][2] },
		{ verts[2][0], verts[2][1], verts[2][2] },
		{ verts[3][0], verts[3][1], verts[3][2] },

		{ verts[0][0], verts[0][1], verts[0][2] },
		{ verts[3][0], verts[3][1], verts[3][2] },
		{ verts[4][0], verts[4][1], verts[4][2] },

		{ verts[0][0], verts[0][1], verts[0][2] },
		{ verts[4][0], verts[4][1], verts[4][2] },
		{ verts[5][0], verts[5][1], verts[5][2] },

		{ verts[0][0], verts[0][1], verts[0][2] },
		{ verts[5][0], verts[5][1], verts[5][2] },
		{ verts[6][0], verts[6][1], verts[6][2] },

		{ verts[0][0], verts[0][1], verts[0][2] },
		{ verts[6][0], verts[6][1], verts[6][2] },
		{ verts[7][0], verts[7][1], verts[7][2] },

		{ verts[0][0], verts[0][1], verts[0][2] },
		{ verts[7][0], verts[7][1], verts[7][2] },
		{ verts[8][0], verts[8][1], verts[8][2] },

		{ verts[0][0], verts[0][1], verts[0][2] },
		{ verts[8][0], verts[8][1], verts[8][2] },
		{ verts[1][0], verts[1][1], verts[1][2] },

		// Triangles for upper middle quater
		{ verts[1][0], verts[1][1], verts[1][2] },
		{ verts[9][0], verts[9][1], verts[9][2] },
		{ verts[2][0], verts[2][1], verts[2][2] },

		{ verts[2][0], verts[2][1], verts[2][2] },
		{ verts[9][0], verts[9][1], verts[9][2] },
		{ verts[10][0], verts[10][1], verts[10][2] },

		{ verts[2][0], verts[2][1], verts[2][2] },
		{ verts[10][0], verts[10][1], verts[10][2] },
		{ verts[3][0], verts[3][1], verts[3][2] },

		{ verts[3][0], verts[3][1], verts[3][2] },
		{ verts[10][0], verts[10][1], verts[10][2] },
		{ verts[11][0], verts[11][1], verts[11][2] },

		{ verts[3][0], verts[3][1], verts[3][2] },
		{ verts[11][0], verts[11][1], verts[11][2] },
		{ verts[4][0], verts[4][1], verts[4][2] },

		{ verts[4][0], verts[4][1], verts[4][2] },
		{ verts[11][0], verts[11][1], verts[11][2] },
		{ verts[12][0], verts[12][1], verts[12][2] },

		{ verts[4][0], verts[4][1], verts[4][2] },
		{ verts[12][0], verts[12][1], verts[12][2] },
		{ verts[5][0], verts[5][1], verts[5][2] },

		{ verts[5][0], verts[5][1], verts[5][2] },
		{ verts[12][0], verts[12][1], verts[12][2] },
		{ verts[13][0], verts[13][1], verts[13][2] },

		{ verts[5][0], verts[5][1], verts[5][2] },
		{ verts[13][0], verts[13][1], verts[13][2] },
		{ verts[6][0], verts[6][1], verts[6][2] },

		{ verts[6][0], verts[6][1], verts[6][2] },
		{ verts[13][0], verts[13][1], verts[13][2] },
		{ verts[14][0], verts[14][1], verts[14][2] },

		{ verts[6][0], verts[6][1], verts[6][2] },
		{ verts[14][0], verts[14][1], verts[14][2] },
		{ verts[7][0], verts[7][1], verts[7][2] },

		{ verts[7][0], verts[7][1], verts[7][2] },
		{ verts[14][0], verts[14][1], verts[14][2] },
		{ verts[15][0], verts[15][1], verts[15][2] },

		{ verts[7][0], verts[7][1], verts[7][2] },
		{ verts[15][0], verts[15][1], verts[15][2] },
		{ verts[8][0], verts[8][1], verts[8][2] },

		{ verts[8][0], verts[8][1], verts[8][2] },
		{ verts[15][0], verts[15][1], verts[15][2] },
		{ verts[16][0], verts[16][1], verts[16][2] },

		{ verts[8][0], verts[8][1], verts[8][2] },
		{ verts[16][0], verts[16][1], verts[16][2] },
		{ verts[1][0], verts[1][1], verts[1][2] },

		{ verts[1][0], verts[1][1], verts[1][2] },
		{ verts[16][0], verts[16][1], verts[16][2] },
		{ verts[9][0], verts[9][1], verts[9][2] },

		// Triangles for lower middle quater
		{ verts[9][0], verts[9][1], verts[9][2] },
		{ verts[16][0], verts[16][1], verts[16][2] },
		{ verts[17][0], verts[17][1], verts[17][2] },

		{ verts[17][0], verts[17][1], verts[17][2] },
		{ verts[16][0], verts[16][1], verts[16][2] },
		{ verts[18][0], verts[18][1], verts[18][2] },

		{ verts[16][0], verts[16][1], verts[16][2] },
		{ verts[15][0], verts[15][1], verts[15][2] },
		{ verts[18][0], verts[18][1], verts[18][2] },

		{ verts[18][0], verts[18][1], verts[18][2] },
		{ verts[15][0], verts[15][1], verts[15][2] },
		{ verts[19][0], verts[19][1], verts[19][2] },

		{ verts[15][0], verts[15][1], verts[15][2] },
		{ verts[14][0], verts[14][1], verts[14][2] },
		{ verts[19][0], verts[19][1], verts[19][2] },

		{ verts[19][0], verts[19][1], verts[19][2] },
		{ verts[14][0], verts[14][1], verts[14][2] },
		{ verts[20][0], verts[20][1], verts[20][2] },

		{ verts[14][0], verts[14][1], verts[14][2] },
		{ verts[13][0], verts[13][1], verts[13][2] },
		{ verts[20][0], verts[20][1], verts[20][2] },

		{ verts[20][0], verts[20][1], verts[20][2] },
		{ verts[13][0], verts[13][1], verts[13][2] },
		{ verts[21][0], verts[21][1], verts[21][2] },

		{ verts[13][0], verts[13][1], verts[13][2] },
		{ verts[12][0], verts[12][1], verts[12][2] },
		{ verts[21][0], verts[21][1], verts[21][2] },

		{ verts[21][0], verts[21][1], verts[21][2] },
		{ verts[12][0], verts[12][1], verts[12][2] },
		{ verts[22][0], verts[22][1], verts[22][2] },

		{ verts[12][0], verts[12][1], verts[12][2] },
		{ verts[11][0], verts[11][1], verts[11][2] },
		{ verts[22][0], verts[22][1], verts[22][2] },

		{ verts[22][0], verts[22][1], verts[22][2] },
		{ verts[11][0], verts[11][1], verts[11][2] },
		{ verts[23][0], verts[23][1], verts[23][2] },

		{ verts[11][0], verts[11][1], verts[11][2] },
		{ verts[10][0], verts[10][1], verts[10][2] },
		{ verts[23][0], verts[23][1], verts[23][2] },

		{ verts[23][0], verts[23][1], verts[23][2] },
		{ verts[10][0], verts[10][1], verts[10][2] },
		{ verts[24][0], verts[24][1], verts[24][2] },

		{ verts[10][0], verts[10][1], verts[10][2] },
		{ verts[9][0], verts[9][1], verts[9][2] },
		{ verts[24][0], verts[24][1], verts[24][2] },

		{ verts[24][0], verts[24][1], verts[24][2] },
		{ verts[9][0], verts[9][1], verts[9][2] },
		{ verts[17][0], verts[17][1], verts[17][2] },

		// Triangles for lower quarter
		{ verts[17][0], verts[17][1], verts[17][2] },
		{ verts[18][0], verts[18][1], verts[18][2] },
		{ verts[25][0], verts[25][1], verts[25][2] },

		{ verts[18][0], verts[18][1], verts[18][2] },
		{ verts[19][0], verts[19][1], verts[19][2] },
		{ verts[25][0], verts[25][1], verts[25][2] },

		{ verts[19][0], verts[19][1], verts[19][2] },
		{ verts[20][0], verts[20][1], verts[20][2] },
		{ verts[25][0], verts[25][1], verts[25][2] },

		{ verts[20][0], verts[20][1], verts[20][2] },
		{ verts[21][0], verts[21][1], verts[21][2] },
		{ verts[25][0], verts[25][1], verts[25][2] },

		{ verts[21][0], verts[21][1], verts[21][2] },
		{ verts[22][0], verts[22][1], verts[22][2] },
		{ verts[25][0], verts[25][1], verts[25][2] },

		{ verts[22][0], verts[22][1], verts[22][2] },
		{ verts[23][0], verts[23][1], verts[23][2] },
		{ verts[25][0], verts[25][1], verts[25][2] },

		{ verts[23][0], verts[23][1], verts[23][2] },
		{ verts[24][0], verts[24][1], verts[24][2] },
		{ verts[25][0], verts[25][1], verts[25][2] },

		{ verts[24][0], verts[24][1], verts[24][2] },
		{ verts[17][0], verts[17][1], verts[17][2] },
		{ verts[25][0], verts[25][1], verts[25][2] }
	};
	// Copy the local array to the global one for the sphere
	memcpy(dest, roughSphereTemplate, sizeof(roughSphereTemplate));
}
