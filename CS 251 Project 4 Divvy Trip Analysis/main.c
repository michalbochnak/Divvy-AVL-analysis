/*main.cpp*/

//
// Divvy Bike Ride Route Analysis, using AVL trees.
//
// << Michal Bochnak >>
// U. of Illinois, Chicago
// CS251, Spring 2017
// Project #04
//

// ignore stdlib warnings if working in Visual Studio:
#define _CRT_SECURE_NO_WARNINGS 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "avl.h"


// ----------------------------------------------------------------------------
// Functions Declarations
// ----------------------------------------------------------------------------
double distBetween2Points(double lat1, double long1, double lat2, double long2);
void freeAVLNodeData(AVLKey key, AVLValue value);
char *getFileName(); 
void skipRestOfInput(FILE *stream);




///////////////////////////////////////////////////////////////////////
//
// main:
//
int main()
{
	int id = 0;				// user input 
	double distance = 0;	// user distance

	printf("** Welcome to Divvy Route Analysis **\n");

	//
	// get filenames from the user/stdin:
	//
	char  cmd[64];
	char *StationsFileName = getFileName();
	char *TripsFileName = getFileName();

	//
	// Create trees
	AVL *stations = AVLCreate();
	AVL *trips = AVLCreate();
	AVL *bikes = AVLCreate();


	//
	// Build trees
	//
	AVLBuildStationsTree(stations, StationsFileName);
	AVLBuildTripsTree(trips, bikes, TripsFileName);
	AVLUpdateStationsTree(stations, trips->Root);
	
	
	//
	// now interact with user:
	//
	printf("** Ready **\n");

	scanf("%s", cmd);

	while (strcmp(cmd, "exit") != 0)
	{
		if (strcmp(cmd, "stats") == 0)
		{
			//
			// Output some stats about our data structures:
			//
			printf("** Trees:\n");

			printf("   Stations: count = %d, height = %d\n",
				AVLCount(stations), AVLHeight(stations));
			printf("   Trips:    count = %d, height = %d\n",
				AVLCount(trips), AVLHeight(trips));
			printf("   Bikes:    count = %d, height = %d\n",
				AVLCount(bikes), AVLHeight(bikes));
		}
		else if (strcmp(cmd, "station") == 0) 
		{	
			scanf("%d", &id);
			// display info about station
			DisplayStationInfo(AVLSearch(stations, id));
		}
		else if (strcmp(cmd, "trip") == 0)
		{
			// display info about trip
			scanf("%d", &id);
			DisplayTripInfo(AVLSearch(trips, id));
		}
		else if (strcmp(cmd, "bike") == 0)
		{
			// display info about bike
			scanf("%d", &id);
			DisplayBikeInfo(AVLSearch(bikes, id));
		}
		else if (strcmp(cmd, "find") == 0)
		{
			// array to hold set of locations
			ClosestStations *closestStations = (ClosestStations*)malloc(sizeof(ClosestStations));
			Coords userLocation;	// user coordinates
			// initialize the array info
			InitializeClosestStations(closestStations);
			scanf("%lf %lf %lf", &userLocation.latitude, &userLocation.longtitude, &distance);
			// traverse the tree and insert stations into array
			closestStations = AVLFindClosestStations(stations->Root, userLocation, distance, closestStations);
			// sort the array by distance, secondary by id
			SelectionSort(closestStations);
			// display closest stations
			DisplayClosestStations(closestStations);

			// free the memory
			free(closestStations->stations);
			free(closestStations);
		}
		else if (strcmp(cmd, "route") == 0)
		{	
			// declare needed variables
			int tripCount = 0;						// number of trips
			AVLNode *sourceNode;					// source station
			AVLNode *destNode;						// destunation station 
			Coords sourceCoords, destCoords;		// coordinates of the stations
			IDList *sources;						// set of source stations
			IDList *destinations;					// set of destination stations
			AVLNode *trip;							// trip node based on id

			// initialize arrays
			sources = InitializeIDList();
			destinations = InitializeIDList();
			
			scanf("%d %lf", &id, &distance); 
			// grab the info about given trip
			trip = AVLSearch(trips, id);

			if (trip == NULL) {		// not found
				printf("**not found\n");
				scanf("%s", cmd);
				continue;
			}

			// store info from trip node into source and destination ID's
			int sourceID = trip->Value.Trip.FromID;
			int destID = trip->Value.Trip.ToID;

			// find the nodes
			sourceNode = AVLSearch(stations, sourceID);
			destNode = AVLSearch(stations, destID);

			// assign coords
			sourceCoords = sourceNode->Value.Station.Coordinates;
			destCoords = destNode->Value.Station.Coordinates;

			// build sources and destination Subsets
			AVLBuildSubSet(sources, sourceCoords, stations->Root, distance);
			AVLBuildSubSet(destinations, destCoords, stations->Root, distance);			// count trips
			AVLCountTrips(sources, destinations, trips->Root, &tripCount);
			DisplayRouteStats(tripCount, sourceID, destID, trips->Count);

			// free the memory
			free(sources->arr);
			free(sources);
			free(destinations->arr);
			free(destinations);
		}
		else
		{
			printf("**unknown cmd, try again...\n");
		}

		scanf("%s", cmd);
	}

	//
	// done, free memory and return:
	//
	printf("** Freeing memory **\n");

	// free the memory used for tree
	AVLFree(stations, freeAVLNodeData);
	AVLFree(trips, freeAVLNodeData);
	AVLFree(bikes, freeAVLNodeData);
	
	// free the memory used for filenames
	free(StationsFileName);
	free(TripsFileName);

	printf("** Done **\n");

	return 0;
} // end of main


// ----------------------------------------------------------------------------
// Functions Definitions
// ----------------------------------------------------------------------------


//
// distBetween2Points: 
//
// Returns the distance in miles between 2 points (lat1, long1) and (lat2, long2).
// 
double distBetween2Points(double lat1, double long1, double lat2, double long2)
{
	//
	// Reference: http://www8.nau.edu/cvm/latlon_formula.html
	//
	double PI = 3.14159265;
	double earth_rad = 3963.1;  // statue miles:

	double lat1_rad = lat1 * PI / 180.0;
	double long1_rad = long1 * PI / 180.0;
	double lat2_rad = lat2 * PI / 180.0;
	double long2_rad = long2 * PI / 180.0;

	double dist = earth_rad * acos(
		(cos(lat1_rad)*cos(long1_rad)*cos(lat2_rad)*cos(long2_rad))
		+
		(cos(lat1_rad)*sin(long1_rad)*cos(lat2_rad)*sin(long2_rad))
		+
		(sin(lat1_rad)*sin(lat2_rad))
	);

	return dist;
}


//
// freeAVLNodeData
//
// Works with AVLFree() to free the data inside (key, value) pairs.
//
void freeAVLNodeData(AVLKey key, AVLValue value)
{
	//
	// what we free depends on what type of value we have:
	//
	if (value.Type == STATIONTYPE)
	{
		free(value.Station.Name);
	}
	else if (value.Type == TRIPTYPE)
	{
		return;
	}
	else if (value.Type == BIKETYPE)
	{
		return;
	}
	else
	{
		printf("**ERROR: unexpected value type in freeAVLNodeData!\n\n");
		exit(-1);
	}
}


//
// getFileName: 
//
// Inputs a filename from the keyboard, make sure the file can be
// opened, and returns the filename if so.  If the file cannot be 
// opened, an error message is output and the program is exited.
//
char *getFileName()
{
	char filename[512];
	int  fnsize = sizeof(filename) / sizeof(filename[0]);

	// input filename from the keyboard:
	fgets(filename, fnsize, stdin);
	filename[strcspn(filename, "\r\n")] = '\0';  // strip EOL char(s):

												 // make sure filename exists and can be opened:
	FILE *infile = fopen(filename, "r");
	if (infile == NULL)
	{
		printf("**Error: unable to open '%s'\n\n", filename);
		exit(-1);
	}

	fclose(infile);

	// duplicate and return filename:
	char *s = (char *)malloc((strlen(filename) + 1) * sizeof(char));
	strcpy(s, filename);

	return s;
}


//
// skipRestOfInput:
//
// Inputs and discards the remainder of the current line for the 
// given input stream, including the EOL character(s).
//
void skipRestOfInput(FILE *stream)
{
	char restOfLine[256];
	int rolLength = sizeof(restOfLine) / sizeof(restOfLine[0]);

	fgets(restOfLine, rolLength, stream);
}

//
// Initialize the Closest Stations array
//
void InitializeClosestStations(ClosestStations *closestStations) {

	// 5 spaces inittially
	closestStations->stations = (StationInfo*)malloc(sizeof(StationInfo) * 5);
	closestStations->size = 5;
	closestStations->count = 0;
}


//
// grows the stize of the array
//
void GrowClosestStations(ClosestStations *closestStations) {
	int i;
	int count = closestStations->count;

	// malloc space for the temp
	StationInfo *temp = (StationInfo*)malloc(sizeof(StationInfo) * (closestStations->size * 2));

	// copy the elements over
	for (i = 0; i < count - 1; i++) {
		temp[i] = closestStations->stations[i];
	}

	// free the memory and update info
	free(closestStations->stations);
	closestStations->size *= 2;				// update the size
	closestStations->stations = temp;
}


//
// selection sort 
//
void SelectionSort(ClosestStations *closestStations) {
	int i = 0;					// outer loop counter
	int j = 0;					// inner loop counter
	int minIndex = 0;			// stores index of minimum value during selection sort
	StationInfo temp;

	for (i = 0; i < closestStations->count; i++) {
		minIndex = i;			// assign current i for minIndex
		for (j = i + 1; j < closestStations->count; j++) {
			if (closestStations->stations[j].distance < closestStations->stations[minIndex].distance) {
				minIndex = j;	// new minIndex found, assign to minIndex
			}
		}

		// swap the elements in the array
		temp = closestStations->stations[i];	// store stations[i] into temp

		// store minIndex station into i-th position
		closestStations->stations[i] = closestStations->stations[minIndex];
		// copy temp into minIndex station
		closestStations->stations[minIndex] = temp;

		// sort by id if distance is the same
		if (i > 0) {
			if (closestStations->stations[i - 1].distance
				== closestStations->stations[i].distance)
				if (closestStations->stations[i - 1].stationID
				> closestStations->stations[i].stationID) {
					// swap
					temp = closestStations->stations[i - 1];
					closestStations->stations[i - 1] = closestStations->stations[i];
					closestStations->stations[i] = temp;
				}
		}
	}
}


//
// initialize the IDList data structure
//
IDList *InitializeIDList() {

	// malloc the memory for the list
	IDList *list = (IDList*)malloc(sizeof(IDList));
	list->arr = (int*)malloc(sizeof(int) * 5);
	list->count = 0;
	list->size = 5;

	return list;		// return pointer to the new list
}


//
// Grow IDList
//
void GrowIDList(IDList *list) {
	int i;
	int count = list->count;

	// malloc the memory
	int *temp = (int*)malloc(sizeof(int) * (list->size * 2));

	// copy the elements over
	for (i = 0; i < count - 1; i++) {
		temp[i] = list->arr[i];
	}

	// free the memory and update info
	free(list->arr);
	list->size *= 2;	
	list->arr = temp;
}


//
// searches if given integer is in the array 
//
int SearchArray(IDList *sources, int id) {
	
	int i = 0;	// loop counter
	
	for (; i < sources->count; i++) {
		if (sources->arr[i] == id)
			return TRUE;	// found
	}

	return FALSE;			// not found
}


//
// Displays the info about station
//
void DisplayStationInfo(AVLNode *station) {

	// empty
	if (station == NULL) {
		printf("**not found\n");
		return;
	}

	// displays stats
	printf("**Station %d:\n", station->Key);
	printf("  Name: \'%s\'\n", station->Value.Station.Name);
	printf("%-13s (%lf,%lf)\n", "  Location:", station->Value.Station.Coordinates.latitude,
		station->Value.Station.Coordinates.longtitude);
	printf("%-13s %d\n", "  Capacity:", station->Value.Station.Capacity);
	printf("  Trip count: %d\n", station->Value.Station.TripCount);
}


//
// Displays the info about trip
//
void DisplayTripInfo(AVLNode *trip) {

	// empty
	if (trip == NULL) {
		printf("**not found\n");
		return;
	}

	// displays stats
	printf("**Trip %d:\n", trip->Key);
	printf("%-7s %d\n", "  Bike:", trip->Value.Trip.BikeID);
	printf("%-7s %d\n", "  From:", trip->Value.Trip.FromID);
	printf("%-7s %d\n", "  To:", trip->Value.Trip.ToID);
	printf("  Duration: %d min, %d secs\n", trip->Value.Trip.TripDuration.minutes,
		trip->Value.Trip.TripDuration.seconds);
}


//
// Displays the info about bike
//
void DisplayBikeInfo(AVLNode *bike) {

	// empty
	if (bike == NULL) {
		printf("**not found\n");
		return;
	}

	// displays stats
	printf("**Bike %d:\n", bike->Key);
	printf("  Trip count: %d\n", bike->Value.Bike.TripCount);
}


//
// Displays closest stations
//
void DisplayClosestStations(ClosestStations *closestStations) {
	int i = 0;
	 
	// displays stats about closest the stations
	for (; i < closestStations->count; i++) {
		printf("Station %d: distance %lf miles\n",
			closestStations->stations[i].stationID,
			closestStations->stations[i].distance);
	}
}


//
// Displys info about trips
//
void DisplayRouteStats(int tripCount, int sourceID, int destID, int totalTrips) {

	// display info
	printf("** Route: from station #%d to station #%d\n", sourceID, destID);
	printf("** Trip count: %d\n", tripCount);
	printf("** Percentage: %lf%%\n", ((double)tripCount / totalTrips) * 100);
}


//
// Convert trip duration from seconds to minutes and seconds
//
Duration ConvertDuration(int seconds) {

	Duration duration;

	duration.minutes = seconds / 60;		// get the minutes
	duration.seconds = seconds % 60;		// get the seconds

	return duration;		// return new duration
}