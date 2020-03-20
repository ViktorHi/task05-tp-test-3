/*
  #dmitars
  dmitriy.tarasienko@gmail.com
  2020

  Sum_counter.h
  declaration of the functions, related to the counting of the total sums
*/
#include<stdio.h>
#include<stdlib.h>
#include<sqlite3.h>
#ifndef _SUM_COUNTER_

	#define _SUM_COUNTER_

		double find_current_total_sum(sqlite3* db, int driver_id ,char* date_start,char* date_end, 
	char* err_msg);

		int find_total_sums(sqlite3* db,char* date_start,char* date_end);

		static int get_drivers_count(void *data, int argc, char **argv, char **azColName);

		static int get_sum(void *data, int argc, char **argv, char **azColName);

		static int get_drivers_id(void *data, int argc, char **argv, char **azColName);

		int check_code(int rc,char*);

#endif
