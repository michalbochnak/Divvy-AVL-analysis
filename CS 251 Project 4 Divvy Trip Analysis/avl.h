/*avl.h*/

//
// AVL Tree ADT header file.
//
// << Michal Bochnak >>
// U. of Illinois, Chicago
// CS251, Spring 2017
//

// make sure this header file is #include exactly once:
#pragma once

#define TRUE 1
#define FALSE 0

//
// AVL type declarations:
//
typedef int  AVLKey;

// coordinates
typedef struct Coords {
	double latitude;
	double longtitude;
} Coords;

// duration in minutes and seconds
typedef struct Duration {
	int minutes;
	int seconds;
} Duration;

// station type
typedef struct STATION
{
	char	*Name;
	Coords	Coordinates;
	int		Capacity;
	int		TripCount;

} STATION;

// trip type
typedef struct TRIP
{
	int		BikeID;
	int		FromID;
	int		ToID;
	Duration TripDuration;

} TRIP;

// bike type
typedef struct BIKE
{
	int  TripCount;

} BIKE;

// union types
enum UNIONTYPE
{
	STATIONTYPE,
	TRIPTYPE,
	BIKETYPE
};

// AVLValue struct
typedef struct AVLValue
{
	enum UNIONTYPE Type;  // Station, Trip, or Bike:
	union
	{
		STATION  Station;   // union => only ONE of these is stored:
		TRIP     Trip;
		BIKE     Bike;
	};
} AVLValue;

// AVLNode struct
typedef struct AVLNode
{
	AVLKey    Key;
	AVLValue  Value;
	struct AVLNode  *Left;
	struct AVLNode  *Right;
	int       Height;
} AVLNode;

// AVL Struct / tree handle
typedef struct AVL
{
	AVLNode *Root;
	int      Count;
} AVL;

// station info
typedef struct StationInfo 
{
	int stationID;
	double distance;
} StationInfo;

// keeps info about closest stations
typedef struct ClosestStations 
{
	StationInfo *stations;
	int count;
	int size;
} ClosestStations;

// keeps info about id's
typedef struct IDList 
{
	int *arr;
	int count;
	int size;
} IDList;




//
// main.c
// function prototypes 
//
void DisplayStationInfo(AVLNode *station);
void DisplayTripInfo(AVLNode *trip);
void DisplayBikeInfo(AVLNode *bike);
void DisplayClosestStations(ClosestStations *closestStations);
void DisplayRouteStats(int tripCount, int sourceID, int destID, int totalTrips);
void InitializeClosestStations(ClosestStations *closestStations);
double distBetween2Points(double lat1, double long1, double lat2, double long2);
void GrowClosestStations(ClosestStations *closestStations);
void GrowIDList(IDList *list);
void SelectionSort(ClosestStations *closestStations);
int SearchArray(IDList *sources, int id);
IDList *InitializeIDList();
Duration ConvertDuration(int seconds);



//
// AVL API: 
// function prototypes
//
AVL *AVLCreate();
AVLNode *AVLSearch(AVL *tree, AVLKey key);
ClosestStations *AVLFindClosestStations(AVLNode *stations, Coords userLocation, double distance, ClosestStations *closestStations);
int AVLCompareKeys(AVLKey key1, AVLKey key2);
int AVLInsert(AVL *tree, AVLKey key, AVLValue value);
int AVLCount(AVL *tree);
int AVLHeight(AVL *tree);
void AVLBuildStationsTree(AVL *tree, char *StationsFileName);
void AVLBuildTripsTree(AVL *trips, AVL *bikes, char *TripsFileName);
void AVLUpdateStationsTree(AVL *stations, AVLNode *trips);
void AVLCountTrips(IDList *sources, IDList *destinations, AVLNode *trips, int *count);
void AVLBuildSubSet(IDList *list, Coords coords, AVLNode *stations, double distance);
void AVLFree(AVL *tree, void(*fp)(AVLKey key, AVLValue value));
