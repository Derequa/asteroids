#ifndef __DEEZSTRUCTZ__
#define __DEEZSTRUCTZ__

#include "objects.h"

/*
	@file datastructures.h
	@author Derek Batts
	This header file defines functions and struct for lists of
	Asteroids, missles, and explosions
*/
typedef struct missleNode {
	Missle* value;
	struct missleNode* next;
} MissleNode;

typedef struct {
	MissleNode* head;
	MissleNode* tail;
	int size;
} MissleList;

typedef struct asteroidNode {
	Asteroid* value;
	struct asteroidNode* next;
} AsteroidNode;

typedef struct {
	AsteroidNode* head;
	AsteroidNode* tail;
	int size;
} AsteroidList;

typedef struct explosionNode {
	Explosion* value;
	struct explosionNode* next;
} ExplosionNode;

typedef struct {
	ExplosionNode* head;
	ExplosionNode* tail;
	int size;
} ExplosionList;

MissleList* createListMissle();
void addToMissleList(Missle* missle, MissleList* list);
bool removeMissle(Missle* missle, MissleList* list);
void clearMissles(MissleList* list);
AsteroidList* createList();
void addToList(Asteroid* asteroid, AsteroidList* list);
bool remove(Asteroid* asteroid, AsteroidList* list);
ExplosionList* createExplosions();
void addExplosion(Explosion* e, ExplosionList* list);
bool removeExplosion(Explosion* e, ExplosionList* list);

#endif // !__DEEZSTRUCTZ__
