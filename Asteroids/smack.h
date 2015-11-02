#ifndef __SMACK__
#define __SMACK__

#include "objects.h"
#include "datastructures.h"

/*
	@file smack.h
	@author Derek Batts
	This header file defines methods for determnining and handling collisions in
	our Asteroids game
*/

bool detectCollideAsteroidShot(Asteroid* asteroid, Missle* shot);
bool detectCollideAsteroidShip(Asteroid* asteroid, PlayerShip* ship);
void handleCollideAsteroidShot(AsteroidList* asteroidList, Asteroid* asteroid, MissleList* shots, Missle* shot, ExplosionList* eList);
void handleCollideAsteroidShip(AsteroidList* asteroidList, Asteroid* asteroid, PlayerShip* ship, ExplosionList* eList);

#endif