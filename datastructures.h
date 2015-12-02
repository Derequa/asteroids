

#ifndef __DEEZSTRUCTZ__
#define __DEEZSTRUCTZ__


#include "objects.h"
/*
	@file datastructures.h
	@author Derek Batts - dsbatts@ncsu.edu
	This header file defines functions and struct for lists of
	Asteroids, missles, and explosions
*/

// A struct describing a node for a generic list
typedef struct node {
	void* value;
	struct node* next;
} ObjectNode;

// A struct containing basic data needed to manipulate a generic list
typedef struct {
	ObjectNode* head;
	ObjectNode* tail;
	int size;
} ObjectList;

/*
	This function creates a list.
	@return A pointer to the list that was made.
 */
ObjectList* createList();

/*
	This function adds a pointer of any type to the given list.
	@param value The pointer to the thing you want to add to the list.
	@param list A pointer to the list you want to add stuff to.
 */
void addToList(void* value, ObjectList* list);

/*
	This function looks for a given pointer in a list and removes it if present.
	This function will free the node storing the value, BUT WILL NOT FREE THE VALUE.
	WARNING: This looks for values by comparing thier memory addresses not thier actual values.
	@param value A pointer to the item you want to remove.
	@param list A pointer to the list you want to remove it from.
	@return True if the item was found and removed, false otherwise.
 */
bool removeFromList(void* value, ObjectList* list);

/*
	This function clears a list and frees all the nodes AND VALUES
	ASSOCIATED WITH THEM.
	@param list A pointer to the list to clear
 */
void clearList(ObjectList* list);

#endif // !__DEEZSTRUCTZ__
