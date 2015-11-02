#include "GL/glut.h"
#include <iostream>
#include <stdlib.h>

#ifndef __ASTOBJS__
#define __ASTOBJS__

/**
    @file models.h
    @author Derek Batts
    This header file defines constants, models, and verticies for the asteroids program.
 */

#define ROOT2OV2 0.707106f
#define ROOTROOT2OV2 0.594603f
#define PIOV8HI 0.923879f
#define PIOV8LO 0.382683f
#define COLLISION_PHI 26.0f
#define NUM_SPHERE_VERTS 144
#define NUM_SPHERE_NORMS 48
#define NUM_SHIP_VERTS 12
#define NUM_SHIP_NORMS 4
#define NUM_UNIQUE_SPH_PTS 26
#define MAX_PLAYER_V 0.06f
#define PLAYER_INIT_POSX -3.0f
#define PLAYER_INIT_POSY 0.0f
#define PLAYER_SIZE 0.25f
#define PLAYER_SPIN 3.0f
#define PLAYER_A 0.08f
#define PLAYER_FRICTION 0.04f
#define PLAYER_COOLDOWN 500
#define PLAYER_CD_DELTA 25
#define BOUND_X_UPPER 6.0f
#define BOUND_X_LOWER -6.0f
#define BOUND_Y_UPPER 6.0f
#define BOUND_Y_LOWER -6.0f
#define MISSLE_V 0.2f
#define MISSLE_AGE_MAX 2500
#define MISSLE_AGE_DELTA 25
#define MISSLE_SIZE 5.0f
#define EXPLOSION_PT_SIZE 1.75f
#define EXPLOSION_NUM_PTS 8
#define EXPLOSION_V 0.1f
#define EXPLOSION_MAX_AGE 28
#define X_ 0
#define Y_ 1
#define Z_ 2

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

typedef struct {
	GLfloat verticies[NUM_SHIP_VERTS][3];
	int numVerticies;
	GLfloat normals[NUM_SHIP_NORMS][3];
	int numNorms;
	float mat[4];
	unsigned int missleCoolDown;
	GLfloat positionVector[3];
	GLfloat orientation[3];
	GLfloat scale[3];
	GLfloat directionUnitVector[2];
	GLfloat aMag;
	GLfloat vMag;
	GLfloat spin;
	GLfloat spinFactor;
} PlayerShip;

typedef struct {
	GLfloat verticies[NUM_SPHERE_VERTS][3];
	int numVerticies;
	GLfloat normals[NUM_SPHERE_NORMS][3];
	int numNorms;
	int age;
	float mat[4];
	GLfloat positionVector[3];
	GLfloat orientation[3];
	GLfloat scale[3];
	GLfloat vVector[2];
	GLfloat spin;
	GLfloat spinFactor;
} Asteroid;

typedef struct {
	GLfloat positionVector[3];
	GLfloat directionVector[2];
	GLfloat vMag;
	GLfloat mat[4];
	unsigned int age;
} Missle;

typedef struct {
	GLfloat vMag;
	GLfloat positionVector[3];
	GLfloat points[EXPLOSION_NUM_PTS][3];
	GLfloat directions[EXPLOSION_NUM_PTS][2];
	int age;
	float mat[4];

} Explosion;

void updatePlayer(PlayerShip* p);
PlayerShip* initPlayer();
void updateAsteroid(Asteroid* a);
Asteroid* initAsteroid();
void updateRoughSphere(GLfloat(&verts)[NUM_UNIQUE_SPH_PTS][3], GLfloat(&dest)[NUM_SPHERE_VERTS][3]);
void roughenSphere(GLfloat(&verts)[NUM_UNIQUE_SPH_PTS][3], const GLfloat(&srcVerts)[NUM_UNIQUE_SPH_PTS][3]);
void calculateNormal(GLfloat(&v0)[3], GLfloat(&v1)[3], GLfloat(&v3)[3], GLfloat(&normDest)[3]);
Missle* fireShot(PlayerShip* p);
void updateMissle(Missle* m);
void setKeyLeft(bool isDown);
void setKeyRight(bool isDown);
void setKeyX(bool isDown);
Explosion* makeExplosion(GLfloat x, GLfloat y, GLfloat z);
void updateExplosion(Explosion* e);

#endif
