#ifndef __SMACK__
#define __SMACK__

#include "objects.h"
#include "datastructures.h"

/*
	@file smack.h
	@author Derek Batts - dsbatts@ncsu.edu
	This header file defines methods for determnining and handling collisions in
	our Asteroids game.
*/


/*
	This function determines if an Asteroid and a Missle collided.
	@param asteroid The Asteroid to look at.
	@param shot The Missle to look at.
	@return True if a collision occured false otherwise.
*/
bool detectCollideAsteroidShot(Asteroid* asteroid, Missle* shot);

/*
	This function determines if an Asteroid and a player collided.
	@param asteroid The Asteroid to look at.
	@param ship The player to look at.
	@return True if a collision occured false otherwise.
*/
bool detectCollideAsteroidShip(Asteroid* asteroid, PlayerShip* ship);

/*
	This function checks if a given asteroid and alien are colliding.
	@param asteroid A pointer to the asteroid to look at.
	@param alien A pointer to the alien ship to look att.
	@return True if the asteroid and the alien are colliding, false otherwise.
*/
bool detectCollideAsteroidAlien(Asteroid* asteroid, Alien* alien);

/*
	This function detects if a collision has occured between a given alien and missle.
	@param alien A pointer to the alien to look at.
	@param missle A pointer to the missle to look at.
	@return True if the given alien and missle are colliding.
*/
bool detectCollideAlienShot(Alien* alien, Missle* missle);

/*
	This function checks if a given alien ship and player ship are colliding.
	@param alien A pointer to the alien to look at.
	@param ship A pointer to the player ship to look at.
	@return True if a collision has occured.
*/
bool detectCollideAlienPlayer(Alien* alien, PlayerShip* ship);

/*
	This function detects if a collision between a given player
	and missle has occured.
	@param ship A pointer to the player ship to look at.
	@param missle A pointer to the missle to look at.
	@return True if a collision has occured.
*/
bool detectCollidePlayerShot(PlayerShip* ship, Missle* missle);

/*
	This function is responsible for handling what happens when a collision happens
	between an Asteroid and a Missle.
	@param asteroidList The list of Asteroids.
	@param asteroid The Asteroid that collided.
	@param shots The list of Missles.
	@param shot The Missle that collided.
	@param eList The list of explosions.
*/
void handleCollideAsteroidShot(ObjectList* asteroidList, Asteroid* asteroid, ObjectList* shots, Missle* shot, ObjectList* eList);

/*
	This function is responsible for handling what happens when a collision happens
	between an Asteroid and a player.
	@param asteroidList The list of Asteroids.
	@param asteroid The Asteroid that collided.
	@param ship The player that collided.
	@param eList The list of explosions.
*/
void handleCollideAsteroidShip(ObjectList* asteroidList, Asteroid* asteroid, PlayerShip* ship, ObjectList* eList);

/*
	This function handles reponding to a collision between an asteroid and
	an alien ship.
	@param asteroidList The list of asteroid the asteroid is apart of.
	@param asteroid A pointer to the asteroid that collided.
	@param alienList The list of alien ships the alien is apart of.
	@param alien A pointer to the alien that collided.
	@param eList The list of explosions to add new explosions to.
*/
void handleCollideAsteroidAlien(ObjectList* asteroidList, Asteroid* asteroid, ObjectList* alienList, Alien* alien, ObjectList* eList);

/*
	This function responds to a collision between an alien and a missle.
	@param alienList The list of alien ships the alien is apart of.
	@param alien A pointer to the alien that collided.
	@param shots The list of missles the missle belongs to.
	@param missle A pointer to the missle that collided.
	@param eList The list of explosions to add new explosions to.
*/
void handleCollideAlienShot(ObjectList* alienList, Alien* alien, ObjectList* shots, Missle* missle, ObjectList* eList);

/*
	This function responds to a collision between an alien and a player.
	@param alienList The list of alien ships the alien is apart of.
	@param alien A pointer to the alien that collided.
	@param ship The player that collided.
	@param eList The list of explosions to add new explosions to.
*/
void handleCollideAlienPlayer(ObjectList* alienList, Alien* alien, PlayerShip* ship, ObjectList* eList);

/*
	This function responds to a collision between an player and a missle.
	@param ship The player that collided.
	@param shots The list of missles the missle belongs to.
	@param missle A pointer to the missle that collided.
	@param eList The list of explosions to add new explosions to.
*/
void handleCollidePlayerShot(PlayerShip* ship, ObjectList* shots, Missle* missle, ObjectList* eList);

#endif
