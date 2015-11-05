#include "smack.h"
#include "objects.h"
#include "datastructures.h"
#include <math.h>


static bool checkPointInCircle(float radius, float sx, float sy, float x, float y);
static bool checkLineInCircle(float radius, float sx, float sy, float x1, float y1, float x2, float y2);
static bool checkPointInLine(float x, float y, float x1, float y1, float x2, float y2);
static void splitOrRemove(AsteroidList* list, Asteroid* asteroid, Missle* shot);

/*
	This function determines if an Asteroid and a Missle collided.
	@param asteroid The Asteroid to look at.
	@param shot The Missle to look at.
	@return True if a collision occured false otherwise
*/
bool detectCollideAsteroidShot(Asteroid* asteroid, Missle* shot)
{
	return checkPointInCircle(asteroid->scale[0], asteroid->positionVector[0], asteroid->positionVector[1],
		shot->positionVector[0], shot->positionVector[1]);
}

/*
	This function determines if an Asteroid and a player collided.
	@param asteroid The Asteroid to look at.
	@param ship The player to look at.
	@return True if a collision occured false otherwise
*/
bool detectCollideAsteroidShip(Asteroid* asteroid, PlayerShip* ship)
{	
	//Determine the point for the three lines that define the ship
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
	

	//Check points in circle
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

	//Check line segments in circle
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
	This function is responsible for handling what happens when a collision happens
	between an Asteroid and a Missle.
	@param asteroidList The list of Asteroids.
	@param asteroid The Asteroid that collided.
	@param shots The list of Missles.
	@param shot The Missle that collided.
	@param eList The list of explosions.
	@sideeffects The given lists will almost certainly be modified and the given objects should be removed from those lists.
*/
void handleCollideAsteroidShot(AsteroidList* asteroidList, Asteroid* asteroid, MissleList* shots, Missle* shot, ExplosionList* eList)
{
	//Explosion
	Explosion* newExp = makeExplosion(asteroid->positionVector[X_], asteroid->positionVector[Y_], asteroid->positionVector[Z_]);
	addExplosion(newExp, eList);
	//Split or remove asteroid
	splitOrRemove(asteroidList, asteroid, shot);
	//Remove shot
	if (removeMissle(shot, shots))
		free(shot);
}

/*
	This function is responsible for handling what happens when a collision happens
	between an Asteroid and a player.
	@param asteroidList The list of Asteroids.
	@param asteroid The Asteroid that collided.
	@param ship The player that collided.
	@param eList The list of explosions.
	@sideeffects The given lists will almost certainly be modified and the given objects should be removed from those lists.
*/
void handleCollideAsteroidShip(AsteroidList* asteroidList, Asteroid* asteroid, PlayerShip* ship, ExplosionList* eList)
{
	//Explosion
	Explosion* newExp1 = makeExplosion(ship->positionVector[X_], ship->positionVector[Y_], ship->positionVector[Z_]);
	addExplosion(newExp1, eList);
	Explosion* newExp2 = makeExplosion(asteroid->positionVector[X_], asteroid->positionVector[Y_], asteroid->positionVector[Z_]);
	addExplosion(newExp2, eList);
	//Split or remove asteroid
	splitOrRemove(asteroidList, asteroid, NULL);
	//Reset player
	ship->positionVector[X_] = PLAYER_INIT_POSX;
	ship->positionVector[Y_] = PLAYER_INIT_POSY;
	ship->spin = 0.0f;
}

//Check if a point is in a circle
static bool checkPointInCircle(float radius, float sx, float sy, float x, float y)
{
	float xSide = sx - x;
	float ySide = sy - y;
	float d = (xSide * xSide) + (ySide * ySide);
	return (d < (radius * radius));
}

//Check for a line in a circe
static bool checkLineInCircle(float radius, float sx, float sy, float x1, float y1, float x2, float y2)
{
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

//Check that a point lies on a given line
static bool checkPointInLine(float x, float y, float x1, float y1, float x2, float y2)
{
	return (x1<x<x2 && y1<y<y2) || (x1>x>x2 && y1>y>y2);
}

//Split an asteroid or destroy it
static void splitOrRemove(AsteroidList* list, Asteroid* asteroid, Missle* shot)
{
	//Check how many times the asteroid has been split
	if (asteroid->age == 0){
		if (remove(asteroid, list))
			free(asteroid);
	}
	else {
		//Make the asteroid smaller
		for (int i = 0; i < 3; i++)
			asteroid->scale[i] = asteroid->scale[i] / 1.7f;
		//Decrement the age
		asteroid->age--;

		//Make a clone of the asteroid
		Asteroid* newAsteroid = (Asteroid*)malloc(sizeof(Asteroid));
		memcpy(newAsteroid, asteroid, sizeof(Asteroid));

		//Remember the asteroid's original velocity
		float theta = 0.0f;
		GLfloat oldV[] = { asteroid->vVector[X_], asteroid->vVector[Y_] };
		float vMag = sqrt((oldV[Y_] * oldV[Y_]) + (oldV[X_] * oldV[X_]));

		//If we were hit by a shot, adjust our direction
		if (shot == NULL)
			theta = atan(oldV[Y_] / oldV[X_]) * (180 / 3.1415);
		else 
			theta = atan(shot->directionVector[X_] / shot->directionVector[Y_]) * (180 / 3.1415);

		//Adjust direction for both asteroids so they fly away from each other
		asteroid->vVector[X_] = vMag * cos(theta + COLLISION_PHI);
		asteroid->vVector[Y_] = vMag * sin(theta + COLLISION_PHI);
		newAsteroid->vVector[X_] = vMag * cos(theta - COLLISION_PHI);
		newAsteroid->vVector[Y_] = vMag * sin(theta - COLLISION_PHI);

		//Add the new asteroid to the list
		addToList(newAsteroid, list);
	}

}
