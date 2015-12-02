#include "objects.h"
#include "datastructures.h"

/*
	@file datastructures.cpp
	@author Derek Batts - dsbatts@ncsu.edu
	This file contains functions that implement a simple dynamic list object.
 */

/*
	This function creates a list.
	@return A pointer to the list that was made.
*/
ObjectList* createList()
{
	// Allocate a new list
	ObjectList* newList = (ObjectList*)malloc(sizeof(ObjectList));
	// Initialize it and return it
	newList->head = NULL;
	newList->tail = NULL;
	newList->size = 0;
	return newList;
}

/*
	This function adds a pointer of any type to the given list.
	@param value The pointer to the thing you want to add to the list.
	@param list A pointer to the list you want to add stuff to.
*/
void addToList(void* value, ObjectList* list)
{
	// Allocate a new node and initialize it
	ObjectNode* newNode = (ObjectNode*)malloc(sizeof(ObjectNode));
	newNode->next = NULL;
	newNode->value = value;

	// Check if the list is empty
	if (list->size == 0)
		list->head = newNode;
	// Check if it only has one thing in it
	else if (list->size == 1){
		list->tail = newNode;
		list->head->next = list->tail;
	}
	// Otherwise, add it to the end of the list
	else {
		list->tail->next = newNode;
		list->tail = list->tail->next;
	}
	// Increment the size of the list
	list->size++;
}

/*
	This function looks for a given pointer in a list and removes it if present.
	This function will free the node storing the value, BUT WILL NOT FREE THE VALUE.
	WARNING: This looks for values by comparing thier memory addresses not thier actual values.
	@param value A pointer to the item you want to remove.
	@param list A pointer to the list you want to remove it from.
	@return True if the item was found and removed, false otherwise.
*/
bool removeFromList(void* value, ObjectList* list)
{
	// Make sure the list is not empty
	if (list->size <= 0)
		return false;
	// Check if what we are looking for is at the head
	if (list->head->value == value){
		// Remember the node
		ObjectNode* temp = list->head;
		// Move the head to the next node
		list->head = list->head->next;
		// Free the old head
		free(temp);
		// Decrement the size
		list->size--;
		// Check if the list is empty
		if (list->size == 0){
			list->head == NULL;
			list->tail = NULL;
		}
		return true;
	}

	// Remember the previous node (the head at first)
	ObjectNode* prev = list->head;

	// Loop through the nodes looking for the value
	for (ObjectNode* n = prev->next; n != NULL; n = n->next){
		// Check for the value
		if (n->value == value){
			// Point the previous node's next field to the node infront of this one
			prev->next = n->next;
			// Free this node
			free(n);
			// Check if we were the tail, and update if so
			if (prev->next == NULL)
				list->tail = prev;
			// Decrement the size and finish
			list->size--;
			return true;
		}
		// Move prev with n
		prev = n;
	}
	return false;
}

/*
	This function clears a list and frees all the nodes AND VALUES
	ASSOCIATED WITH THEM.
	@param list A pointer to the list to clear
*/
void clearList(ObjectList* list)
{
	// Check for and empty list
	if (list->size <= 0) return;
	// Loop through all the nodes in the list
	for (ObjectNode* node = list->head; node != NULL;){
		// Free the value at a given node
		free(node->value);
		// Move the node and free the one we were loking at
		ObjectNode* temp = node;
		node = node->next;
		free(temp);
	}
	// Reset the list fields
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
}
