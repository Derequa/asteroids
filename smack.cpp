#include "smack.h"
#include "objects.h"
#include "datastructures.h"
#include <math.h>

/*
	@file smack.cpp
	@author Derek Batts - dsbatts@ncsu.edu
	This file provides implementation for collision detecting and responses to collisions
	for our asteroids game.
 */


static bool checkPointInCircle(float radius, float sx, float sy, float x, float y);
static bool checkLineInCircle(float radius, float sx, float sy, float x1, float y1, float x2, float y2);
static bool checkPointInLine(float x, float y, float x1, float y1, float x2, float y2);
static bool checkPointInTriangle(float ptx, float pty, float v1x, float v1y, float v2x, float v2y, float v3x, float v3y);
static void splitOrRemove(ObjectList* list, Asteroid* asteroid, Missle* shot);


/* Function for detecting collisions */


/*
	This function determines if an Asteroid and a Missle collided.
	@param asteroid The Asteroid to look at.
	@param shot The Missle to look at.
	@return True if a collision occured false otherwise.
*/
bool detectCollideAsteroidShot(Asteroid* asteroid, Missle* shot)
{
	// Check a point in a circle
	return checkPointInCircle(asteroid->scale[0], asteroid->positionVector[0], asteroid->positionVector[1],
		shot->positionVector[0], shot->positionVector[1]);
}

/*
	This function determines if an Asteroid and a player collided.
	@param asteroid The Asteroid to look at.
	@param ship The player to look at.
	@return True if a collision occured false otherwise.
*/
bool detectCollideAsteroidShip(Asteroid* asteroid, PlayerShip* ship)
{
	// Determine the point for the three lines that define the ship
	float xtemp = (ship->scale[X_] * spaceShip[0][X_]);
	float ytemp = (ship->scale[Z_] * spaceShip[0][Z_]);

	float x1 = ship->positionVector[X_] + ytemp;
	float y1 = ship->positionVector[Y_] - xtemp;

	xtemp = (ship->scale[X_] * spaceShip[1][X_]);
	ytemp = (ship->scale[Z_] * spaceShip[1][Z_]);

	float x2 = ship->positionVector[X_] + ytemp;
	float y2 = ship->positionVector[Y_] - xtemp;

	xtemp = (ship->scale[X_] * spaceShip[2][X_]);
	ytemp = (ship->scale[Z_] * spaceShip[2][Z_]);

	float x3 = ship->positionVector[X_] + ytemp;
	float y3 = ship->positionVector[Y_] - xtemp;


	// Check points in circle
	if (checkPointInCircle(asteroid->scale[0], asteroid->positionVector[0], asteroid->positionVector[1],
		ship->positionVector[0], ship->positionVector[1]))
		return true;
	else if (checkPointInCircle(asteroid->scale[0], asteroid->positionVector[0], asteroid->positionVector[1],
		x1, y1))
		return true;
	else if (checkPointInCircle(asteroid->scale[0], asteroid->positionVector[0], asteroid->positionVector[1],
		x2, y2))
		return true;
	else if (checkPointInCircle(asteroid->scale[0], asteroid->positionVector[0], asteroid->positionVector[1],
		x3, y3))
		return true;

	// Check line segments in circle
	if (checkLineInCircle(asteroid->scale[0], asteroid->positionVector[0], asteroid->positionVector[1],
		x1, y1, x2, y2))
		return true;
	else if (checkLineInCircle(asteroid->scale[0], asteroid->positionVector[0], asteroid->positionVector[1],
		x2, y2, x3, y3))
		return true;
	else if (checkLineInCircle(asteroid->scale[0], asteroid->positionVector[0], asteroid->positionVector[1],
		x1, y1, x3, y3))
		return true;

	return false;
}

/*
	This function checks if a given asteroid and alien are colliding.
	@param asteroid A pointer to the asteroid to look at.
	@param alien A pointer to the alien ship to look att.
	@return True if the asteroid and the alien are colliding, false otherwise.
 */
bool detectCollideAsteroidAlien(Asteroid* asteroid, Alien* alien)
{
	// Calculate the alien and the asteroid's total width and height (reduced to make collisions appear more realistic)
	float alien_w = (1.6 * alien->torusOuterRadius) + (3.5 * alien->torusInnerRadius);
	float alien_h = 1.6 * alien->sphereRadius;
	float asteroid_w = 1.9 * asteroid->scale[X_];
	float asteroid_h = 1.9 * asteroid->scale[Y_];
	// Check how close the alien and asteroid are
	if ((abs(asteroid->positionVector[X_] - alien->positionVector[X_]) <= ((alien_w + asteroid_w) / 2)) &&
		(abs(asteroid->positionVector[Y_] - alien->positionVector[Y_]) <= ((alien_h + asteroid_h) / 2)))
		return true;
	else return false;
}

/*
	This function detects if a collision has occured between a given alien and missle.
	@param alien A pointer to the alien to look at.
	@param missle A pointer to the missle to look at.
	@return True if the given alien and missle are colliding.
 */
bool detectCollideAlienShot(Alien* alien, Missle* missle)
{
	// Calculate the alien's width and height (reduced to look slightly more realistic)
	float alien_w = (1.5 * alien->torusOuterRadius) + (3.5 * alien->torusInnerRadius);
	float alien_h = 1.5 * alien->sphereRadius;
	// See how close the missle is to the alien
	if ((abs(missle->positionVector[X_] - alien->positionVector[X_]) <= ((alien_w) / 2)) &&
		(abs(missle->positionVector[Y_] - alien->positionVector[Y_]) <= ((alien_h) / 2)))
		return true;
	else return false;
}

/*
	This function checks if a given alien ship and player ship are colliding.
	@param alien A pointer to the alien to look at.
	@param ship A pointer to the player ship to look at.
	@return True if a collision has occured.
 */
bool detectCollideAlienPlayer(Alien* alien, PlayerShip* ship)
{
	// Calculate the aliens adjusted height and width.
	float alien_w = ((1.7 * alien->torusOuterRadius) + (3.6 * alien->torusInnerRadius)) / 2;
	float alien_h = (1.6 * alien->sphereRadius) / 2;

	// Calculate the co-ordinates for the player's three points
	float xtemp = (ship->scale[X_] * spaceShip[0][X_]);
	float ytemp = (ship->scale[Z_] * spaceShip[0][Z_]);

	float x1 = ship->positionVector[X_] + ytemp;
	float y1 = ship->positionVector[Y_] - xtemp;

	xtemp = (ship->scale[X_] * spaceShip[1][X_]);
	ytemp = (ship->scale[Z_] * spaceShip[1][Z_]);

	float x2 = ship->positionVector[X_] + ytemp;
	float y2 = ship->positionVector[Y_] - xtemp;

	xtemp = (ship->scale[X_] * spaceShip[2][X_]);
	ytemp = (ship->scale[Z_] * spaceShip[2][Z_]);

	float x3 = ship->positionVector[X_] + ytemp;
	float y3 = ship->positionVector[Y_] - xtemp;

	// Check if any of the 3 points collide with the alien
	if ((abs(x1 - alien->positionVector[X_]) <= alien_w) &&
		(abs(y1 - alien->positionVector[Y_]) <= alien_h))
		return true;
	else if ((abs(x2 - alien->positionVector[X_]) <= alien_w) &&
			 (abs(y2 - alien->positionVector[Y_]) <= alien_h))
		return true;
	else if ((abs(x3 - alien->positionVector[X_]) <= alien_w) &&
			 (abs(y3 - alien->positionVector[Y_]) <= alien_h))
		return true;
	else return false;
}

/*
	This function detects if a collision between a given player
	and missle has occured.
	@param ship A pointer to the player ship to look at.
	@param missle A pointer to the missle to look at.
	@return True if a collision has occured.
 */
bool detectCollidePlayerShot(PlayerShip* ship, Missle* missle)
{
	float xtemp = (ship->scale[X_] * spaceShip[0][X_]);
	float ytemp = (ship->scale[Z_] * spaceShip[0][Z_]);

	float x1 = ship->positionVector[X_] + ytemp;
	float y1 = ship->positionVector[Y_] - xtemp;

	xtemp = (ship->scale[X_] * spaceShip[1][X_]);
	ytemp = (ship->scale[Z_] * spaceShip[1][Z_]);

	float x2 = ship->positionVector[X_] + ytemp;
	float y2 = ship->positionVector[Y_] - xtemp;

	xtemp = (ship->scale[X_] * spaceShip[2][X_]);
	ytemp = (ship->scale[Z_] * spaceShip[2][Z_]);

	float x3 = ship->positionVector[X_] + ytemp;
	float y3 = ship->positionVector[Y_] - xtemp;

	return checkPointInTriangle(missle->positionVector[X_], missle->positionVector[Y_], x1, y1, x2, y2, x3, y3);
}



/* Functions for responding to collisions */



/*
	This function is responsible for handling what happens when a collision happens
	between an Asteroid and a Missle.
	@param asteroidList The list of Asteroids.
	@param asteroid The Asteroid that collided.
	@param shots The list of Missles.
	@param shot The Missle that collided.
	@param eList The list of explosions.
*/
void handleCollideAsteroidShot(ObjectList* asteroidList, Asteroid* asteroid, ObjectList* shots, Missle* shot, ObjectList* eList)
{
	// Explosion
	Explosion* newExp = makeExplosion(asteroid->positionVector[X_], asteroid->positionVector[Y_], asteroid->positionVector[Z_]);
	addToList(newExp, eList);
	// Split or remove asteroid
	splitOrRemove(asteroidList, asteroid, shot);
	// Remove shot
	if (removeFromList(shot, shots))
		free(shot);
}

/*
	This function is responsible for handling what happens when a collision happens
	between an Asteroid and a player.
	@param asteroidList The list of Asteroids.
	@param asteroid The Asteroid that collided.
	@param ship The player that collided.
	@param eList The list of explosions.
*/
void handleCollideAsteroidShip(ObjectList* asteroidList, Asteroid* asteroid, PlayerShip* ship, ObjectList* eList)
{
	// Explosion
	Explosion* newExp1 = makeExplosion(ship->positionVector[X_], ship->positionVector[Y_], ship->positionVector[Z_]);
	addToList(newExp1, eList);
	Explosion* newExp2 = makeExplosion(asteroid->positionVector[X_], asteroid->positionVector[Y_], asteroid->positionVector[Z_]);
	addToList(newExp2, eList);
	// Split or remove asteroid
	splitOrRemove(asteroidList, asteroid, NULL);
	// Reset player
	ship->positionVector[X_] = PLAYER_INIT_POSX;
	ship->positionVector[Y_] = PLAYER_INIT_POSY;
	ship->spin = 0.0f;
}

/*
	This function handles reponding to a collision between an asteroid and
	an alien ship.
	@param asteroidList The list of asteroid the asteroid is apart of.
	@param asteroid A pointer to the asteroid that collided.
	@param alienList The list of alien ships the alien is apart of.
	@param alien A pointer to the alien that collided.
	@param eList The list of explosions to add new explosions to.
 */
void handleCollideAsteroidAlien(ObjectList* asteroidList, Asteroid* asteroid, ObjectList* alienList, Alien* alien, ObjectList* eList)
{
	// Explosion
	Explosion* newExp1 = makeExplosion(alien->positionVector[X_], alien->positionVector[Y_], alien->positionVector[Z_]);
	addToList(newExp1, eList);
	Explosion* newExp2 = makeExplosion(asteroid->positionVector[X_], asteroid->positionVector[Y_], asteroid->positionVector[Z_]);
	addToList(newExp2, eList);
	// Split or remove asteroid
	splitOrRemove(asteroidList, asteroid, NULL);
	if (removeFromList(alien, alienList))
		free(alien);
}

/*
	This function responds to a collision between an alien and a missle.
	@param alienList The list of alien ships the alien is apart of.
	@param alien A pointer to the alien that collided.
	@param shots The list of missles the missle belongs to.
	@param missle A pointer to the missle that collided.
	@param eList The list of explosions to add new explosions to.
 */
void handleCollideAlienShot(ObjectList* alienList, Alien* alien, ObjectList* shots, Missle* missle, ObjectList* eList)
{
	// Explosion
	Explosion* newExp1 = makeExplosion(alien->positionVector[X_], alien->positionVector[Y_], alien->positionVector[Z_]);
	addToList(newExp1, eList);
	// Remove alien and missle
	if (removeFromList(missle, shots))
		free(missle);
	if (removeFromList(alien, alienList))
		free(alien);
}

/*
	This function responds to a collision between an alien and a player.
	@param alienList The list of alien ships the alien is apart of.
	@param alien A pointer to the alien that collided.
	@param ship The player that collided.
	@param eList The list of explosions to add new explosions to.
*/
void handleCollideAlienPlayer(ObjectList* alienList, Alien* alien, PlayerShip* ship, ObjectList* eList)
{
	// Explosions
	Explosion* newExp1 = makeExplosion(alien->positionVector[X_], alien->positionVector[Y_], alien->positionVector[Z_]);
	addToList(newExp1, eList);
	Explosion* newExp2 = makeExplosion(ship->positionVector[X_], ship->positionVector[Y_], ship->positionVector[Z_]);
	addToList(newExp2, eList);
	if (removeFromList(alien, alienList))
		free(alien);
	// Reset player
	ship->positionVector[X_] = PLAYER_INIT_POSX;
	ship->positionVector[Y_] = PLAYER_INIT_POSY;
	ship->spin = 0.0f;
}


/*
	This function responds to a collision between an player and a missle.
	@param ship The player that collided.
	@param shots The list of missles the missle belongs to.
	@param missle A pointer to the missle that collided.
	@param eList The list of explosions to add new explosions to.
*/
void handleCollidePlayerShot(PlayerShip* ship, ObjectList* shots, Missle* missle, ObjectList* eList)
{
	// Make an explosion
	Explosion* newExp1 = makeExplosion(ship->positionVector[X_], ship->positionVector[Y_], ship->positionVector[Z_]);
	addToList(newExp1, eList);
	if (removeFromList(missle, shots))
		free(missle);
	// Reset player
	ship->positionVector[X_] = PLAYER_INIT_POSX;
	ship->positionVector[Y_] = PLAYER_INIT_POSY;
	ship->spin = 0.0f;
}


/* Mathy stuff */


/*
	This function checks if a point lies within a circle.
	@param radius The radius of the circle.
	@param sx The x co-ordinate of the circle.
	@param sy The y co-ordinate of the circle.
	@param x The point's x value.
	@param y The point's y value.
	@return True if the point lies within the circle.
*/
static bool checkPointInCircle(float radius, float sx, float sy, float x, float y)
{
	float xSide = sx - x;
	float ySide = sy - y;
	float d = (xSide * xSide) + (ySide * ySide);
	return (d < (radius * radius));
}

/*
	This function checks if a given line intersects a given circle.
	(I'm pretty sure this function is at least mostly worthless)
	@param radius The radius of the circle.
	@param sx The x co-ordinate of the circle.
	@param sy The y co-ordinate of the circle.
	@param x1 The first x co-ordinate of the line.
	@param y1 The first y co-ordinate of the line.
	@param x2 The second x co-ordinate of the line.
	@param y2 The second y co-ordinate of the line.
	@return True if the line crosses into the circle.
*/
static bool checkLineInCircle(float radius, float sx, float sy, float x1, float y1, float x2, float y2)
{
	// Mathy stuff from the spec that I don't quite get
	float dx = x2 - x1;
	float dy = y2 - y1;
	float d = (dx * dx) + (dy * dy);
	float bigD = ((x1 - sx) * (y2 - sy)) - ((x2 - sx) * (y1 - sy));
	float delta = ((radius * radius) * d) - (bigD * bigD);
	if (delta < 0.0f)
		return false;

	float sgnDy = 1.0f;
	if (dy < 0.0f)
		sgnDy = -1.0f;
	float nx1 = ((bigD * dy) + ((sgnDy * dx) * sqrt((radius * radius * d) - (bigD * bigD)))) / d;
	float nx2 = ((bigD * dy) - ((sgnDy * dx) * sqrt((radius * radius * d) - (bigD * bigD)))) / d;
	float ny1 = (-(bigD * dx) + ((abs(dx)) * sqrt((radius * radius * d) - (bigD * bigD)))) / d;
	float ny2 = (-(bigD * dx) + ((abs(dx)) * sqrt((radius * radius * d) - (bigD * bigD)))) / d;

	int count = 0;
	// Check if the points of intersection lie on the given line segment
	if (checkPointInLine(nx1, ny1, x1, y1, x2, y2))
		count++;
	else if (checkPointInLine(nx1, ny2, x1, y1, x2, y2))
		count++;
	else if (checkPointInLine(nx2, ny1, x1, y1, x2, y2))
		count++;
	else if (checkPointInLine(nx2, ny2, x1, y1, x2, y2))
		count++;

	if (count > 1)
		return true;
	else return false;

}

/*
	This function sees if a point lies on a given line.
	@param x The point's x value.
	@param y The point's y value.
	@param x1 The first x co-ordinate of the line.
	@param y1 The first y co-ordinate of the line.
	@param x2 The second x co-ordinate of the line.
	@param y2 The second y co-ordinate of the line.
*/
static bool checkPointInLine(float x, float y, float x1, float y1, float x2, float y2)
{
	return (x1<x<x2 && y1<y<y2) || (x1>x>x2 && y1>y>y2);
}

/*
	This is a helper function for determining if a point lies within a triangle.
*/
static float sign(float p1x, float p1y, float p2x, float p2y, float p3x, float p3y)
{
	return (p1x - p3x) * (p2y - p3y) - (p2x - p3x) * (p1y - p3y);
}

/*
	This function checks if a given point lies in a given triangle.
	Lots of help (copied) from:
	http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-triangle
	@param ptx The x value of the point.
	@param pty The y value of the point.
	@param v1x The x co-ordinate of the first vertex in the triangle.
	@param v1y The y co-ordinate of the first vertex in the triangle.
	@param v2x The x co-ordinate of the second vertex in the triangle.
	@param v2y The y co-ordinate of the second vertex in the triangle.
	@param v3x The x co-ordinate of the third vertex in the triangle.
	@param v3y The y co-ordinate of the third vertex in the triangle.
*/
static bool checkPointInTriangle(float ptx, float pty, float v1x, float v1y, float v2x, float v2y, float v3x, float v3y)
{
	// Initialize flags
	bool b1, b2, b3;

	// Calculate fancy stuff
	b1 = sign(ptx, pty, v1x, v1y, v2x, v2y) < 0.0f;
	b2 = sign(ptx, pty, v2x, v2y, v3x, v3y) < 0.0f;
	b3 = sign(ptx, pty, v3x, v3y, v1x, v1y) < 0.0f;

	// Rub some loginc on that result
	return ((b1 == b2) && (b2 == b3));
}



/* Less mathy stuff */



/*
	This function handles splitting an asteroid or removing it.
	@param list The list of asteroid the asteroid is apart of.
	@param asteroid The asteroid that we are working on.
	@param shot The missle that hit the asteroid.
 */
static void splitOrRemove(ObjectList* list, Asteroid* asteroid, Missle* shot)
{
	// Check how many times the asteroid has been split
	if (asteroid->age == 0){
		if (removeFromList(asteroid, list))
			free(asteroid);
	}
	else {
		// Make the asteroid smaller
		for (int i = 0; i < 3; i++)
			asteroid->scale[i] = asteroid->scale[i] / 1.7f;
		// Decrement the age
		asteroid->age--;

		// Make a clone of the asteroid
		Asteroid* newAsteroid = (Asteroid*)malloc(sizeof(Asteroid));
		memcpy(newAsteroid, asteroid, sizeof(Asteroid));

		// Remember the asteroid's original velocity
		float theta = 0.0f;
		GLfloat oldV[] = { asteroid->vVector[X_], asteroid->vVector[Y_] };
		float vMag = sqrt((oldV[Y_] * oldV[Y_]) + (oldV[X_] * oldV[X_]));

		// If we were hit by a shot, adjust our direction
		if (shot == NULL)
			theta = atan(oldV[Y_] / oldV[X_]) * (180 / 3.1415);
		else 
			theta = atan(shot->directionVector[X_] / shot->directionVector[Y_]) * (180 / 3.1415);

		// Adjust direction for both asteroids so they fly away from each other
		asteroid->vVector[X_] = vMag * cos(theta + COLLISION_PHI);
		asteroid->vVector[Y_] = vMag * sin(theta + COLLISION_PHI);
		newAsteroid->vVector[X_] = vMag * cos(theta - COLLISION_PHI);
		newAsteroid->vVector[Y_] = vMag * sin(theta - COLLISION_PHI);

		// Add the new asteroid to the list
		addToList(newAsteroid, list);
	}
}
