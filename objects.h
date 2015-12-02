#include "GL/glut.h"
#include <iostream>
#include <stdlib.h>


#ifndef __ASTOBJS__
#define __ASTOBJS__

#include "datastructures.h"

/**
    @file models.h
    @author Derek Batts - dsbatts@ncsu.edu
    This header file defines constants, models, objects, and verticies for the asteroids game.
 */


// Constants needed for determining the vertices in a sphere

// sqrt( 2 ) / 2
#define ROOT2OV2 0.707106f
// sqrt( sqrt( 2 ) ) / 2
#define ROOTROOT2OV2 0.594603f
// cos( pi / 8 )
#define PIOV8HI 0.923879f
// -cos( 5 * pi / 8 )
#define PIOV8LO 0.382683f
// A thingy for deteriming how asteroids split
#define COLLISION_PHI 26.0f


// Constants for vertices and stuff

// The number of vertices in a sphere (drawn with triangles)
#define NUM_SPHERE_VERTS 144
// The number of normals in a sphere
#define NUM_SPHERE_NORMS 48
// The number of vertices in our spaceship model (drawn with triangles)
#define NUM_SHIP_VERTS 12
// The number of normals on the spaceship
#define NUM_SHIP_NORMS 4
// The number of unique points in a sphere
#define NUM_UNIQUE_SPH_PTS 26


// Constants for the player

// The max velocity a player can move with
#define MAX_PLAYER_V 0.06f
// The starting position for the player
#define PLAYER_INIT_POSX -3.0f
#define PLAYER_INIT_POSY 0.0f
// The scale of the player
#define PLAYER_SIZE 0.25f
// The starting angle the player points at
#define PLAYER_SPIN 3.0f
// The acceleration a player has when input is given
#define PLAYER_A 0.08f
// The force slowing down the player
#define PLAYER_FRICTION 0.04f
// The cooldown for the player's gun
#define PLAYER_COOLDOWN 500
// The amount to decrement the player's cooldown by on each frame
#define PLAYER_CD_DELTA 25
// The initial number of lives / deaths a player starts with
#define PLAYER_DEATHS_INIT 5


// Window parameters
#define BOUND_X_UPPER 6.0f
#define BOUND_X_LOWER -6.0f
#define BOUND_Y_UPPER 6.0f
#define BOUND_Y_LOWER -6.0f


// Constants for missles

// The speed at which missles travel
#define MISSLE_V 0.2f
// The amount of time a missle can live
#define MISSLE_AGE_MAX 2500
// The step size a missle ages by
#define MISSLE_AGE_DELTA 25
// How big the missle is as a point
#define MISSLE_SIZE 5.0f


// Constants for explosions

// The size of the points in the explosion
#define EXPLOSION_PT_SIZE 1.75f
// The number of points in the explosion
#define EXPLOSION_NUM_PTS 8
// How fast the points in the explosion travel
#define EXPLOSION_V 0.1f
// How long the explosion lasts
#define EXPLOSION_MAX_AGE 28


// Constants for the aliens

// The speed of the bigger alien
#define ALIEN_LARGE_V_X 0.022f
// The speed of the smaller alien
#define ALIEN_SMALL_V_X 0.031f
// The cooldown for the alien's shots
#define ALIEN_COOLDOWN 1800
// How long to wait before trying to change direction
#define ALIEN_DIR_TIMER 100
// Sphere and torus parameters fo the small alien
#define ALIEN_SMALL_SPH_R 0.2
#define ALIEN_SMALL_SPH_SL 6
#define ALIEN_SMALL_SPH_ST 6
#define ALIEN_SMALL_TOR_OUT_R 0.2
#define ALIEN_SMALL_TOR_IN_R 0.15
#define ALIEN_SMALL_TOR_SIDE 4
#define ALIEN_SMALL_TOR_RINGS 8
// Sphere and torus parameters for the larger alien
#define ALIEN_LARGE_SPH_R 0.4
#define ALIEN_LARGE_SPH_SL 6
#define ALIEN_LARGE_SPH_ST 6
#define ALIEN_LARGE_TOR_OUT_R 0.4
#define ALIEN_LARGE_TOR_IN_R 0.3
#define ALIEN_LARGE_TOR_SIDE 4
#define ALIEN_LARGE_TOR_RINGS 8


// Constants for x, y, and z since they're everywhere
#define X_ 0
#define Y_ 1
#define Z_ 2

// All the vertices needed to draw a sphere
const GLfloat verts[26][3] = {
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, ROOT2OV2, ROOT2OV2 },
		{ ROOTROOT2OV2, ROOTROOT2OV2, ROOTROOT2OV2 },
		{ ROOT2OV2, ROOT2OV2, 0.0f },
		{ ROOTROOT2OV2, ROOTROOT2OV2, -ROOTROOT2OV2 },
		{ 0.0f, ROOT2OV2, -ROOT2OV2 },
		{ -ROOTROOT2OV2, ROOTROOT2OV2, -ROOTROOT2OV2 },
		{ -ROOT2OV2, ROOT2OV2, 0.0f },
		{ -ROOTROOT2OV2, ROOTROOT2OV2, ROOTROOT2OV2 },
		{ PIOV8LO, 0.0f, PIOV8HI },
		{ PIOV8HI, 0.0f, PIOV8LO },
		{ PIOV8HI, 0.0f, -PIOV8LO },
		{ PIOV8LO, 0.0f, -PIOV8HI },
		{ -PIOV8LO, 0.0f, -PIOV8HI },
		{ -PIOV8HI, 0.0f, -PIOV8LO },
		{ -PIOV8HI, 0.0f, PIOV8LO },
		{ -PIOV8LO, 0.0f, PIOV8HI },
		{ 0.0f, -ROOT2OV2, ROOT2OV2 },
		{ -ROOTROOT2OV2, -ROOTROOT2OV2, ROOTROOT2OV2 },
		{ -ROOT2OV2, -ROOT2OV2, 0.0f },
		{ -ROOTROOT2OV2, -ROOTROOT2OV2, -ROOTROOT2OV2 },
		{ 0.0f, -ROOT2OV2, -ROOT2OV2 },
		{ ROOTROOT2OV2, -ROOTROOT2OV2, -ROOTROOT2OV2 },
		{ ROOT2OV2, -ROOT2OV2, 0.0f },
		{ ROOTROOT2OV2, -ROOTROOT2OV2, ROOTROOT2OV2 },
		{ 0.0f, -1.0f, 0.0f }
};

// All the vertices needed to draw our player's spaceship
const GLfloat spaceShip[12][3] = {
		//Base
		{ 0.0f, 0.0f, 1.35f },
		{ -1.0f, 0.0f, -0.65f },
		{ 1.0f, 0.0f , -0.65f },

		//Sides
		{ 1.0f, 0.0f, -0.65f },
		{ 0.0f, 0.40f, 0.0f },
		{ 0.0f, 0.0f, 1.35f },

		{ 0.0f, 0.0f, 1.35f },
		{ 0.0f, 0.40f, 0.0f },
		{ -1.0f, 0.0f, -0.65f },

		{ -1.0f, 0.0f, -0.65f },
		{ 0.0f, 0.40f, 0.0f },
		{ 1.0f, 0.0f, -0.65f }
};

// A struct to model an alien ship
typedef struct {
	// The position in 3D space
	GLfloat positionVector[3];
	// The axes deterimining rotational orientation in 3D
	GLfloat orientation[3];
	// The velocity vector in 2D
	GLfloat vVector[2];
	// The material to draw with
	float mat[4];
	// What angle the ship is rotated by
	float spin;
	// How much the ship is spinning by
	float spinSpeed;
	// Whether or not this is a large alien
	bool isBig;
	// The cooldown for the alien's missles
	unsigned int missleCoolDown;
	// A timer for when we can try to change direction
	unsigned int directionTimer;
	// Sphere parameters for drawing
	float sphereRadius;
	int sphereSlices;
	int sphereStacks;
	// Torus parameters for drawing
	float torusInnerRadius;
	float torusOuterRadius;
	int torusSides;
	int torusRings;
} Alien;

// A struct for modeling a player's ship
typedef struct {
	// The vertices used to draw the ship
	GLfloat verticies[NUM_SHIP_VERTS][3];
	// This player's score
	int score;
	// How many lives / deaths the player has
	int deathsLeft;
	// The number of vertices (also a constant)
	int numVerticies;
	// All the normals for this player's model
	GLfloat normals[NUM_SHIP_NORMS][3];
	// The number of normals
	int numNorms;
	// The material to draw with
	float mat[4];
	// The cooldown for the player's missles
	unsigned int missleCoolDown;
	// Where the player is in 3D space
	GLfloat positionVector[3];
	// The axis to rotate by in 3D
	GLfloat orientation[3];
	// The size of the model
	GLfloat scale[3];
	// The direction the player is headed in 2D
	GLfloat directionUnitVector[2];
	// The magnitude of the player's acceleration
	GLfloat aMag;
	// The magnitude of the player's velocity
	GLfloat vMag;
	// The angle the player is pointed
	GLfloat spin;
	// How much to turn the player by on an update
	GLfloat spinFactor;
} PlayerShip;

// A struct modeling an asteroid
typedef struct {
	// All the vertices used to draw the asteroid
	GLfloat verticies[NUM_SPHERE_VERTS][3];
	// The number of vertices (also a constant)
	int numVerticies;
	// All the normals for this asteroid
	GLfloat normals[NUM_SPHERE_NORMS][3];
	// The number of normals for this asteroid (also a constant)
	int numNorms;
	// How many times has this asteroid been split
	int age;
	// The material to draw this asteroid with
	float mat[4];
	// Where this asteroid is in 3D space
	GLfloat positionVector[3];
	// What axis to rotate this asteroid by
	GLfloat orientation[3];
	// The size of the asteroid
	GLfloat scale[3];
	// The velocity vector the asteroid is moving in
	GLfloat vVector[2];
	// The angle the asteroid is rotated at
	GLfloat spin;
	// How much to increment / decrement the spin by
	GLfloat spinFactor;
} Asteroid;

// A struct modeling a missle
typedef struct {
	// Where the missle in in 3D space
	GLfloat positionVector[3];
	// What direction the missle is headed in
	GLfloat directionVector[2];
	// The magnitude of the missle's velocity
	GLfloat vMag;
	// The material of the missle (no longer used)
	GLfloat mat[4];
	// How long this missle has been traveling
	unsigned int age;
} Missle;

// A struct modeling an explosion
typedef struct {
	// The magnitude of the velocity for each bit of the explosion
	GLfloat vMag;
	// The position of the explosion in 3D space
	GLfloat positionVector[3];
	// All the points in the explosion as 3D points
	GLfloat points[EXPLOSION_NUM_PTS][3];
	// The unit vectors for each point
	GLfloat directions[EXPLOSION_NUM_PTS][2];
	// How long the explosion has been around
	int age;
	// The material to draw the explosion with (no longer needed)
	float mat[4];

} Explosion;

/**
	This function updates a player ship's position and stuff
	@param p The player ship to update
*/
void updatePlayer(PlayerShip* p);

/**
	This function initialized a player ship
	@return The player ship created
*/
PlayerShip* initPlayer();

/**
	This function updates a given asteroid based on its properties
	@param a A pointer to the asteroid to update
*/
void updateAsteroid(Asteroid* a);

/**
	This function creates and returns an asteroid
	@return The created asteroid
*/
Asteroid* initAsteroid();

/**
	This function updates the sphere with the disturbed verticies
*/
void updateRoughSphere(GLfloat(&verts)[NUM_UNIQUE_SPH_PTS][3], GLfloat(&dest)[NUM_SPHERE_VERTS][3]);

/**
	This function randomly disturbs verticies on the sphere
*/
void roughenSphere(GLfloat(&verts)[NUM_UNIQUE_SPH_PTS][3], const GLfloat(&srcVerts)[NUM_UNIQUE_SPH_PTS][3]);

/**
	This function calculates a normal vector for a triangle with 3 given vectors
	@param v0 The first vertex drawn in the triangle
	@param v1 The second vertex drawn in the traiangle
	@param v2 The third vertex drawn in the triangle
	@param normDest The reference to where to store the normal
*/
void calculateNormal(GLfloat(&v0)[3], GLfloat(&v1)[3], GLfloat(&v3)[3], GLfloat(&normDest)[3]);

/**
	This functions handles generating a missle for
	a given player
	@param p The pointer to the player ship to generate missles for
	@return The missle fired, or NULL if one cannot be fired
*/
Missle* fireShot(PlayerShip* p);

/**
	This function handles updating a given missle's parameters
	@param m A pointer to the missle to update
*/
void updateMissle(Missle* m);

/**
	This function set the global for left key pressed
	@param The boolean value to set it to
*/
void setKeyLeft(bool isDown);

/**
	This function set the global for right key pressed
	@param The boolean value to set it to
*/
void setKeyRight(bool isDown);

/**
	This function set the global for X key pressed
	@param The boolean value to set it to
*/
void setKeyX(bool isDown);

/*
	This function creates an explosion object at the given location.
	@param x The x co-ordinate for where to create explosion.
	@param y The y co-ordinate for where to create explosion.
	@param z The z co-ordinate for where to create explosion.
	@return A pointer to the explosion we made.
*/
Explosion* makeExplosion(GLfloat x, GLfloat y, GLfloat z);

/*
	This function updates an explosion.
	@param e The explosion to update.
*/
void updateExplosion(Explosion* e);

/*
	This function initializes an Alien ship with size determined by
	the given parameter.
	@param makeBig True if we are making a big alien ship, false if we are making a small one.
	@return A pointer to the alien we made.
*/
Alien* initAlienShip(bool makeBig);

/*
	This function updates a given alien according to its fields.
	It also check if the alien has crossed a horizontal wall and will signal this on return.
	@param a The alien to update.
	@return True if this alien should still exist, false if it needs to be removed
*/
bool updateAlien(Alien* a);

/*
	This function handles an alien shooting at various objects in the game world.
	@param alien The alien doing the shooting.
	@param asteroids The list of asteroid in the game.
	@param p The player in the game.
	@param alienShots The list of alien shots.
*/
void alienShoot(Alien* alien, ObjectList* asteroids, PlayerShip* p, ObjectList* alienShots);

#endif
