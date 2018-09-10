/*avl.c*/

//
// AVL Tree ADT implementation file.
//
// << Michal Bochnak >>
// U. of Illinois, Chicago
// CS251, Spring 2017
//

// ignore stdlib warnings if working in Visual Studio:
#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "avl.h"


//
// AVLCreate:
//
// Dynamically creates and returns an empty AVL tree.
//
AVL *AVLCreate()
{
	AVL *tree;

	tree = (AVL *)malloc(sizeof(AVL));
	tree->Root = NULL;
	tree->Count = 0;

	return tree;
}


//
// AVLFree:
//
// Frees the memory associated with the tree: the handle and the nodes.
// The provided function pointer is called to free the memory that
// might have been allocated as part of the key or value.
//
void _AVLFree(AVLNode *node, void(*fp)(AVLKey key, AVLValue value)) {

	// base case
	if (node == NULL)
		return;
	else {
		// visit left
		_AVLFree(node->Left, fp);

		// visit right
		_AVLFree(node->Right, fp);

		// delete node
		fp(node->Key, node->Value);
		free(node);
	}

	return;
}


void AVLFree(AVL *tree, void(*fp)(AVLKey key, AVLValue value))
{
	// delete the nodes
	_AVLFree(tree->Root, fp);

	// delete the handle
	free(tree);
}



//
// AVLCompareKeys: 
//
// Compares key1 and key2, returning
//   value < 0 if key1 <  key2
//   0         if key1 == key2
//   value > 0 if key1 >  key2
//
int AVLCompareKeys(AVLKey key1, AVLKey key2)
{
	if (key1 < key2)
		return -1;
	else if (key1 == key2)
		return 0;
	else
		return 1;
}


//
// AVLCount:
//
// Returns # of nodes in the tree.
//
int AVLCount(AVL *tree)
{
	return tree->Count;
}


//
// AVLHeight: 
//
// Returns the overall height of the AVL tree.
//
int AVLHeight(AVL *tree)
{
	if (tree->Root == NULL)
		return -1;
	else
		return tree->Root->Height;
}


//
// Rotate right the sub-tree rooted at node k2, return pointer
// to root of newly-rotated sub-tree --- i.e. return pointer
// to node k1 that was rotated up to top of sub-tree.  Heights
// of the rotated sub-tree are also updated by this function.
//
int _height(AVLNode *cur)
{
	if (cur == NULL)
		return -1;
	else
		return cur->Height;
}

int _max2(int x, int y)
{
	return (x > y) ? x : y;
}

AVLNode *RightRotate(AVLNode *k2)
{
	AVLNode *k1 = k2->Left;

	AVLNode *X = k1->Left;
	AVLNode *Y = k1->Right;
	AVLNode *Z = k2->Right;

	//
	// rotate k1 up, and k2 down to the right:
	//
	k1->Right = k2;
	k2->Left = Y;

	//
	// recompute heights of nodes that moved:  k2, then k1
	//
	k2->Height = 1 + _max2(_height(k2->Left), _height(k2->Right));
	k1->Height = 1 + _max2(_height(k1->Left), _height(k1->Right));

	return k1;  // k1 is the new root of rotated sub-tree:
}

//
// Rotate left the sub-tree rooted at node k1, return pointer
// to root of newly-rotated sub-tree --- i.e. return pointer
// to node k2 that was rotated up to top of sub-tree.  Heights
// of the rotated sub-tree are also updated by this function.
//
AVLNode *LeftRotate(AVLNode *k1)
{
	AVLNode *k2 = k1->Right;

	AVLNode *X = k1->Left;
	AVLNode *Y = k2->Left;
	AVLNode *Z = k2->Right;

	//
	// rotate k2 up, and k1 down to the left:
	//
	k2->Left = k1;
	k1->Right = Y;

	//
	// recompute heights of nodes that moved:  k1, then k2
	//
	k1->Height = 1 + _max2(_height(k1->Left), _height(k1->Right));
	k2->Height = 1 + _max2(_height(k2->Left), _height(k2->Right));

	return k2;  // k2 is the new root of rotated sub-tree:
}


//
// AVL Insert:
//
// Inserts the given (key, value) into the AVL tree, rebalancing
// the tree as necessary.  Returns true (non-zero) if successful,
// false (0) if not --- insert fails if the key is already in the
// tree (no changes are made to the tree in this case).
//
#define _TRUE  1
#define _FALSE 0

int AVLInsert(AVL *tree, AVLKey key, AVLValue value)
{
	AVLNode *prev = NULL;
	AVLNode *cur = tree->Root;

	AVLNode *stack[64];
	int      top = -1;

	//
	// first we search the tree to see if it already contains key:
	//
	while (cur != NULL)
	{
		top++;
		stack[top] = cur;

		if (AVLCompareKeys(key, cur->Key) == 0)  // already in tree, failed:
			return _FALSE;
		else if (AVLCompareKeys(key, cur->Key) < 0)  // smaller, go left:
		{
			prev = cur;
			cur = cur->Left;
		}
		else  // larger, go right:
		{
			prev = cur;
			cur = cur->Right;
		}
	}

	// 
	// If we get here, tree does not contain key, so insert new node
	// where we fell out of tree:
	//
	AVLNode *newNode = (AVLNode *)malloc(sizeof(AVLNode));
	newNode->Key = key;
	newNode->Value = value;
	newNode->Left = NULL;
	newNode->Right = NULL;
	newNode->Height = 0;

	//
	// link T where we fell out of tree -- after prev:
	//
	if (prev == NULL)  // tree is empty, insert @ root:
	{
		tree->Root = newNode;
	}
	else if (AVLCompareKeys(key, prev->Key) < 0)  // smaller, insert to left:
	{
		prev->Left = newNode;
	}
	else  // larger, insert to right:
	{
		prev->Right = newNode;
	}

	tree->Count++;

	//
	// Now walk back up the tree, updating heights and looking for
	// where the AVL balancing criteria may be broken.  If we reach
	// a node where the height doesn't change, then we're done -- the
	// tree is still balanced.  If we reach a node where the AVL 
	// condition is broken, we fix locally and we're done.  One or two
	// local rotations is enough to re-balance the tree.
	//
	AVLNode *N;
	int      rebalance = _FALSE;  // false by default, e.g. if tree is empty:

	while (top >= 0)  // stack != empty::
	{
		N = stack[top];  // N = pop();
		top--;

		int hl = _height(N->Left);
		int hr = _height(N->Right);
		int newH = 1 + _max2(hl, hr);

		if (newH == N->Height)  // heights the same, still an AVL tree::
		{
			rebalance = _FALSE;
			break;
		}
		else if (abs(hl - hr) > 1)  // AVL condition broken, we have to fix::
		{
			rebalance = _TRUE;
			break;
		}
		else  // update height and continue walking up tree::
		{
			N->Height = newH;
		}
	}

	//
	// Okay, does the tree need to be rebalanced?
	//
	if (rebalance)
	{
		cur = N;

		//
		// if we get here, then the AVL condition is broken at "cur".  So we
		// have to decide which of the 4 cases it is and then rotate to fix.
		//

		// we need cur's parent, so pop the stack one more time
		if (top < 0)     // stack is empty, ==> N is root
			prev = NULL;   // flag this with prev == NULL
		else  // stack not empty, so obtain ptr to cur's parent:
			prev = stack[top];

		//
		// which of the 4 cases?
		//
		if (AVLCompareKeys(newNode->Key, cur->Key) < 0)  // case 1 or 2:
		{
			// case 1 or case 2?  either way, we know cur->left exists:
			AVLNode *L = cur->Left;

			if (AVLCompareKeys(newNode->Key, L->Key) > 0)
			{
				// case 2: left rotate @L followed by a right rotate @cur:
				cur->Left = LeftRotate(L);
			}

			// case 1 or 2:  right rotate @cur
			if (prev == NULL)
				tree->Root = RightRotate(cur);
			else if (prev->Left == cur)
				prev->Left = RightRotate(cur);
			else
				prev->Right = RightRotate(cur);
		}
		else
		{
			//
			// case 3 or case 4?  either way, we know cur->right exists:
			//
			AVLNode *R = cur->Right;

			if (AVLCompareKeys(newNode->Key, R->Key) < 0)
			{
				// case 3: right rotate @R followed by a left rotate @cur:
				cur->Right = RightRotate(R);
			}

			// case 3 or case 4:  left rotate @cur:
			if (prev == NULL)
				tree->Root = LeftRotate(cur);
			else if (prev->Left == cur)
				prev->Left = LeftRotate(cur);
			else
				prev->Right = LeftRotate(cur);
		}
	}

	//
	// done:
	//
//	free(newNode);
	return _TRUE;  // success:
}

//
// AVLSearch: searches the binary search tree for a node containing the
// same key.  If a match is found, a pointer to the node is returned, 
// otherwise NULL is returned.
//
AVLNode *AVLSearch(AVL *tree, AVLKey key)
{	
	// cur as tree->Root
	AVLNode *cur = tree->Root;

	if (cur == NULL) {
		return NULL;		// not found
	}
	else {
		while (cur != NULL) {
			if (key == cur->Key) {		// found, return cur
				return cur;		
			}
			else if (key < cur->Key) {	// go left
				cur = cur->Left;
			}
			else {						// go right
				cur = cur->Right;
			}
		}
	}

	return NULL;			// not found
}



//
// Builds the tree with stations, return pointer to the handle
// 
void AVLBuildStationsTree(AVL *tree, char *StationsFileName) {

	// open file
	FILE *pStationsFile = fopen(StationsFileName, "r");
	char line[512];
	char *token;
	AVLNode *temp = (AVLNode*)malloc(sizeof(AVLNode));
	int status = -1;		// insertion status

	

	fgets(line, 512, pStationsFile);	// skip the header line
	fgets(line, 512, pStationsFile);	// read in first line with data

	while (!feof(pStationsFile)) {
		temp->Value.Type = STATIONTYPE;		// specify the type
		line[strcspn(line, "\r\n")] = '\0';	// avoid errors on different platforms
		token = strtok(line, ",");			// parse id
		// convert to int and store into key
		temp->Key = atoi(token);
		token = strtok(NULL, ",");			//	grab next token
		temp->Value.Station.Name = (char*)malloc((sizeof(char) * (strlen(token) + 1)));
		strcpy(temp->Value.Station.Name, token);
		token = strtok(NULL, ",");			//	grab next token
		temp->Value.Station.Coordinates.latitude = atof(token);
		token = strtok(NULL, ",");			//	grab next token
		temp->Value.Station.Coordinates.longtitude = atof(token);
		token = strtok(NULL, ",");			//	grab next token
		temp->Value.Station.Capacity = atoi(token);

		// initialize tripCount to 0
		temp->Value.Station.TripCount = 0;

		// insert
		AVLInsert(tree, temp->Key, temp->Value);

		if (status == 0)
			printf("Insertion failed.\n");

		fgets(line, 512, pStationsFile);	// read in next line
	}
	free(temp);
		
	fclose(pStationsFile);		// close the file
}




//
// Builds the tree with trips, return pointer to the handle
// 
void AVLBuildTripsTree(AVL *trips, AVL *bikes, char *TripsFileName) {

	// open file
	FILE *pTripsFile = fopen(TripsFileName, "r");
	char line[512];
	char *token;
	AVLNode *tempTrip = (AVLNode*)malloc(sizeof(AVLNode));
	//AVLNode *tempBike = (AVLNode*)malloc(sizeof(AVLNode));

	int status = -1;		// insertion status



	fgets(line, 512, pTripsFile);	// skip the header line
	fgets(line, 512, pTripsFile);	// read in first line with data

	while (!feof(pTripsFile)) {
		tempTrip->Value.Type = TRIPTYPE;		// specify the type
		line[strcspn(line, "\r\n")] = '\0';	// avoid errors on different platforms
		token = strtok(line, ",");			// parse id
		// convert to int and store into key
		tempTrip->Key = atoi(token);
		token = strtok(NULL, ",");			//	skip start date and time
		token = strtok(NULL, ",");			//	skip stop date and time
		token = strtok(NULL, ",");			//	grab BikeID
		// convert to int and store into BikeID
		tempTrip->Value.Trip.BikeID = atoi(token);
		// convert to int and store into FromID

		token = strtok(NULL, ",");			//	grab TripDuration
		int seconds = atoi(token);			// convert to int
		// convert to min and sec by calling ConvertDuration function
		tempTrip->Value.Trip.TripDuration = ConvertDuration(seconds);
		token = strtok(NULL, ",");			//	grab FromID
		// convert to int and store into FromID
		tempTrip->Value.Trip.FromID = atoi(token);
		token = strtok(NULL, ",");			//	skip station name
		token = strtok(NULL, ",");			//	grab ToID
		// convert to int and store into ToID
		tempTrip->Value.Trip.ToID = atoi(token);


		// insert
		AVLInsert(trips, tempTrip->Key, tempTrip->Value);

		// insert into bikes tree is needed
		AVLNode *result = AVLSearch(bikes, tempTrip->Value.Trip.BikeID);
		if (result == NULL) {
			// fill out the info about the bike
			AVLNode *tempInsert = (AVLNode*)malloc(sizeof(AVLNode));
			tempInsert->Value.Type = BIKETYPE;		// specify the type
			tempInsert->Key = tempTrip->Value.Trip.BikeID;
			tempInsert->Value.Bike.TripCount = 1;
			// insert
			AVLInsert(bikes, tempInsert->Key, tempInsert->Value);
			// free the memory
			free(tempInsert);
		}
		else							// already inserted, increment count
			result->Value.Bike.TripCount++;

		if (status == 0)				// failed
			printf("Insertion failed.\n");

		fgets(line, 512, pTripsFile);	// read in next line
	}
	free(tempTrip);
	// fclose(TripsFileName);
}


// 
// Updates the stations tree counts,
// Performs pre order traversal of trips tree, 
// searches for id in stations and update if necessary
//
void AVLUpdateStationsTree(AVL *stations, AVLNode *trips) {

	AVLNode *cur = trips;

	// base case
	if (cur == NULL)
		return;
	

	// search for FromID
	AVLNode *result = AVLSearch(stations, cur->Value.Trip.FromID);
	if (result != NULL)
		result->Value.Station.TripCount++;

	// search for ToID
	result = AVLSearch(stations, cur->Value.Trip.ToID);
	if (result != NULL)
		result->Value.Station.TripCount++;

	// recursively visit Right Sub tree
	AVLUpdateStationsTree(stations, trips->Right);

	// recursively visit Left Sub tree
	AVLUpdateStationsTree(stations, trips->Left);
}


// 
// search the tree for stations in the distance range specified by user
// returns pointer to the array when they are stored
//
ClosestStations *AVLFindClosestStations(AVLNode *stations, Coords userLocation,
								double distance, ClosestStations *closestStations) {
	// base case 
	if (stations == NULL)
		return closestStations;

	// compute the distance
	double actualDistance = distBetween2Points(stations->Value.Station.Coordinates.latitude,
		stations->Value.Station.Coordinates.longtitude,
		userLocation.latitude, userLocation.longtitude);

	// check if station in range, insert into array of yes
	if (actualDistance <= distance) {

		// update count
		closestStations->count++;
		// grow size if needed
		if (closestStations->count > closestStations->size)
			GrowClosestStations(closestStations);

		// add to array
		closestStations->stations[closestStations->count-1].distance = actualDistance;
		closestStations->stations[closestStations->count-1].stationID = stations->Key;
	}

	// traverse left sub tree
	AVLFindClosestStations(stations->Left, userLocation,
		distance, closestStations);

	// traverse left subtree
	AVLFindClosestStations(stations->Right, userLocation,
		distance, closestStations);
	
	// return array
	return closestStations;
}


//
// builds the array with stations in the given range from given station
//
void AVLBuildSubSet(IDList *list, Coords coords, AVLNode *stations, double distance) {

	// base case
	if (stations == NULL)
		return;

	// compute the distance
	double actualDistance = distBetween2Points(stations->Value.Station.Coordinates.latitude,
							stations->Value.Station.Coordinates.longtitude,
							coords.latitude, coords.longtitude);

	// add to arr
	if (actualDistance <= distance) {
		list->count++;					// increment count
		if (list->count > list->size)	// size too small, increase size
			GrowIDList(list);

		// insert into array
		list->arr[list->count - 1] = stations->Key;
	}
	
	// visit left subtree
	AVLBuildSubSet(list, coords, stations->Left, distance);

	// visit right sub tree
	AVLBuildSubSet(list, coords, stations->Right, distance);
}


//
// Traverse the tree and counts number of trips from source set to destination set
//
void AVLCountTrips(IDList *sources, IDList *destinations, AVLNode *trips, int *count) {

	// result indicator, exist in set or not
	int resultSource = 0;
	int resultDest = 0;
	
	// NULL, return
	if (trips == NULL)
		return;

	// check sources array
	resultSource = SearchArray(*&sources, trips->Value.Trip.FromID);

	if (resultSource) {		// source matches, search destinations array
		resultDest = SearchArray(*&destinations, trips->Value.Trip.ToID);
		if (resultDest)
			*count = *count + 1;		// destination also matches, increment counter
	}

	// visit left subtree
	AVLCountTrips(sources, destinations, trips->Left, *&count);

	// visit right subtree
	AVLCountTrips(sources, destinations, trips->Right, *&count);
}