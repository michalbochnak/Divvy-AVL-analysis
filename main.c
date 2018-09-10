/*main.cpp*/

//
// Divvy Bike Ride Route Analysis, using AVL trees.
//
// <<YOUR NAME>>
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
    
  }
  else if (value.Type == TRIPTYPE)
  {

  }
  else if (value.Type == BIKETYPE)
  {

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


///////////////////////////////////////////////////////////////////////
//
// main:
//
int main()
{
  printf("** Welcome to Divvy Route Analysis **\n");

  //
  // get filenames from the user/stdin:
  //
  char  cmd[64];
  char *StationsFileName = getFileName();
  char *TripsFileName = getFileName();

  //
  // As an example, create some trees and insert some
  // dummy (key, value) pairs:
  //
  AVL *stations = AVLCreate();
  AVL *trips = AVLCreate();
  AVL *bikes = AVLCreate();

  //
  // Insert a new station:
  //
  AVLValue stationValue;

  stationValue.Type = STATIONTYPE;  // union holds a station:
  stationValue.Station.StationID = 123;
  stationValue.Station.Data = 456;

  //AVLInsert(stations, stationValue.Station.StationID, stationValue);

  //
  // Insert a new trip:
  //
  AVLValue tripValue;

  tripValue.Type = TRIPTYPE;  // union holds a trip:
  tripValue.Trip.TripID = 789001;
  tripValue.Trip.Data = 11;

  //AVLInsert(trips, tripValue.Trip.TripID, tripValue);

  //
  // Insert a new bike:
  //
  AVLValue bikeValue;

  bikeValue.Type = BIKETYPE;  // union holds a bike:
  bikeValue.Bike.BikeID = 4318;
  bikeValue.Bike.Data = 981;

  //AVLInsert(bikes, bikeValue.Bike.BikeID, bikeValue);

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

  AVLFree(stations, freeAVLNodeData);
  AVLFree(trips, freeAVLNodeData);
  AVLFree(bikes, freeAVLNodeData);

  printf("** Done **\n");

  return 0;
}