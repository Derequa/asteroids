#include "objects.h"
#include "datastructures.h"


MissleList* createListMissle()
{
	MissleList* newList = (MissleList*)malloc(sizeof(MissleList));
	newList->head = NULL;
	newList->tail = NULL;
	newList->size = 0;
	return newList;
}

void addToMissleList(Missle* missle, MissleList* list)
{
	MissleNode* newNode = (MissleNode*)malloc(sizeof(MissleNode));
	newNode->next = NULL;
	newNode->value = missle;

	if (list->size == 0)
		list->head = newNode;
	else if (list->size == 1){
		list->tail = newNode;
		list->head->next = list->tail;
	}
	else {
		list->tail->next = newNode;
		list->tail = list->tail->next;
	}

	list->size++;
}

bool removeMissle(Missle* missle, MissleList* list)
{
	if (list->size <= 0)
		return false;
	if (list->head->value == missle){
		MissleNode* temp = list->head;
		list->head = list->head->next;
		free(temp);
		list->size--;
		if (list->size == 0){
			list->head == NULL;
			list->tail = NULL;
		}
		return true;
	}

	MissleNode* prev = list->head;

	for (MissleNode* n = prev->next; n != NULL; n = n->next){
		if (n->value == missle){
			prev->next = n->next;
			free(n);
			if (prev->next == NULL)
				list->tail = prev;
			list->size--;
			return true;
		}
		prev = n;
	}
	return false;
}

void clearMissles(MissleList* list)
{
	if (list->size <= 0) return;
	for (MissleNode* mNode = list->head; mNode != NULL;){
		free(mNode->value);
		MissleNode* temp = mNode;
		mNode = mNode->next;
		free(temp);
	}
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
}

AsteroidList* createList()
{
	AsteroidList* newList = (AsteroidList*)malloc(sizeof(AsteroidList));
	newList->head = NULL;
	newList->tail = NULL;
	newList->size = 0;
	return newList;
}

void addToList(Asteroid* asteroid, AsteroidList* list)
{
	AsteroidNode* newNode = (AsteroidNode*)malloc(sizeof(AsteroidNode));
	newNode->next = NULL;
	newNode->value = asteroid;

	if (list->size == 0)
		list->head = newNode;
	else if (list->size == 1){
		list->tail = newNode;
		list->head->next = list->tail;
	}
	else {
		list->tail->next = newNode;
		list->tail = list->tail->next;
	}

	list->size++;
}

bool remove(Asteroid* asteroid, AsteroidList* list)
{
	if (list->size <= 0)
		return false;
	if (list->head->value == asteroid){
		AsteroidNode* temp = list->head;
		list->head = list->head->next;
		free(temp);
		list->size--;
		if (list->size == 0){
			list->head == NULL;
			list->tail = NULL;
		}
		return true;
	}
	AsteroidNode* prev = list->head;

	for (AsteroidNode* n = prev->next; n != NULL; n = n->next){
		if (n->value == asteroid){
			prev->next = n->next;
			free(n);
			if (prev->next == NULL)
				list->tail = prev;
			list->size--;
			return true;
		}
		prev = n;
	}
	return false;
}

ExplosionList* createExplosions()
{
	ExplosionList* newList = (ExplosionList*)malloc(sizeof(ExplosionList));
	newList->head = NULL;
	newList->tail = NULL;
	newList->size = 0;
	return newList;
}

void addExplosion(Explosion* e, ExplosionList* list)
{
	ExplosionNode* newNode = (ExplosionNode*)malloc(sizeof(ExplosionNode));
	newNode->next = NULL;
	newNode->value = e;

	if (list->size == 0)
		list->head = newNode;
	else if (list->size == 1){
		list->tail = newNode;
		list->head->next = list->tail;
	}
	else {
		list->tail->next = newNode;
		list->tail = list->tail->next;
	}

	list->size++;
}

bool removeExplosion(Explosion* e, ExplosionList* list)
{
	if (list->size <= 0)
		return false;
	if (list->head->value == e){
		ExplosionNode* temp = list->head;
		list->head = list->head->next;
		free(temp);
		list->size--;
		if (list->size == 0){
			list->head == NULL;
			list->tail = NULL;
		}
		return true;
	}
	ExplosionNode* prev = list->head;

	for (ExplosionNode* n = prev->next; n != NULL; n = n->next){
		if (n->value == e){
			prev->next = n->next;
			free(n);
			if (prev->next == NULL)
				list->tail = prev;
			list->size--;
			return true;
		}
		prev = n;
	}
	return false;
}