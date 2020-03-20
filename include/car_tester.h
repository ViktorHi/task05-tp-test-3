/*
  #dmitars
  dmitriy.tarasienko@gmail.com
  2020

  Sum_tester.h
  declaration of the functions, related to the checking of car's carrying
*/
#include<stdio.h>
#include<stdlib.h>
#include<sqlite3.h>

#ifndef _CAR_TESTER_

	#define _CAR_TESTER_

	int check_car(sqlite3* db,char* car_number, double weight);

	static int get_weight(void *data, int argc, char **argv, char **azColName);

#endif
