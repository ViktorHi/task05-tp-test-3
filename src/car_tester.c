/*
  #dmitars
  dmitriy.tarasienko@gmail.com
  2020

  Sum_tester.c
  implementation of the functions, related to the checking of car's carrying
*/
#include "../include/car_tester.h"

int check_car(sqlite3* db,char* car_number, double weight){
	char* sql;
	asprintf(&sql,"select weight from Autopark_cars where number = %s",car_number);

	char* err_msg;
	double* max_weight;
	int rc = sqlite3_exec(db,sql,get_weight,max_weight,&err_msg);

	if(!rc){
		printf("can't find this car");
		return 0;
	}

	return weight<=(*max_weight);
}


static int get_weight(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   char* endptr;
   double value = strtod(argv[0],&endptr);
   data = (void*)(&data);
   return 0;
}
