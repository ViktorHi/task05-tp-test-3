/*
  #dmitars
  dmitriy.tarasienko@gmail.com
  2020

  Sum_counter.c
  implementation of the functions, related to the counting of the total sums
*/
#include "../include//parameter.h"
#include "../include//parameter_2.h"
#include "../include/sum_counter.h"

double find_current_total_sum(sqlite3* db, int driver_id ,char* date_start,char* date_end, 
	char* err_msg){
	char* sql="";
	asprintf(&sql,"select cost from Autopark_completed_orders where driver_number = %i\\
		AND date>=%s AND date<=%s",driver_id,date_start,date_end);
	double sum = 0;

	Parameter* parameter;
	parameter->sum = &sum;
	parameter->start_date = date_start;
	parameter->end_date = date_end;

	int rc = sqlite3_exec(db,sql,get_sum,parameter,&err_msg);
	if(!check_code(rc,err_msg))
		return 0;
	return sum;
}

int find_total_sums(sqlite3* db,char* date_start,char* date_end){

	char* err_msg;
	double* sums;
	int count;

	char* sql_count = "select count(id) from Autopark_drivers";
	sqlite3_exec(db,sql_count,get_drivers_count,&count,&err_msg);
	sums = malloc(count*sizeof(double));

	int* id_of_drivers = malloc(count*sizeof(int));
	int index = 0;

	Parameter_2* parameter_2;
	parameter_2->index = &index;
	parameter_2->mass = &id_of_drivers;

	char* sql_get_id = "select id from Autopark_drivers";
	int rc = sqlite3_exec(db,sql_get_id,get_drivers_id,(void*)parameter_2,&err_msg);
	if(!check_code(rc,err_msg))
	{
		free(sums);
		free(id_of_drivers);
		return 0;
	}

	char* sql = "";

	char* main_sql;
	char* name_of_table;
	asprintf(&name_of_table, "%s_from_%s_to_%s","sums",date_start,date_end);
	asprintf(&main_sql,"BEGIN TRANSACTON;\n\\
					CREATE TABLE %s (integer driver_id PRIMERY_KEY,real sum):",name_of_table);

	Parameter* parameter;
	for(int i = 0;i<count;i++)
	{

		parameter->sum = sums+i;
		asprintf(&sql,"select cost from Autopark_completed_orders where driver_number = %i \\
		AND date>=%s AND date<=%s",id_of_drivers[i],date_start,date_end);
		rc = sqlite3_exec(db,sql,get_sum,parameter,&err_msg);

		if(!check_code(rc,err_msg))
		{
			free(sums);
			free(id_of_drivers);
			return 0;
		}

		asprintf(&main_sql,"%s\ninsert into %s values(%i,%lf)",main_sql,name_of_table,
			id_of_drivers[i],sums[i]);
	}

	asprintf(&main_sql,"%s\nCOMMIT;",main_sql);
	rc = sqlite3_exec(db,main_sql,NULL,NULL,&err_msg);
	free(sums);
	free(id_of_drivers);
	if(!check_code(rc,err_msg))
		return 0;

	return 1;
}

static int get_drivers_count(void *data, int argc, char **argv, char **azColName){
	int count = atoi(argv[0]);
	data = (void*)(&count);
	return 0;
}

static int get_sum(void *data, int argc, char **argv, char **azColName){
	Parameter* parameter = (Parameter*)data;
	double* sum = parameter->sum;
	char* endptr;
	(*sum)+=strtod(argv[0],&endptr);	
	return 0;	
}

static int get_drivers_id(void *data, int argc, char **argv, char **azColName){
	Parameter_2* parameter_2 = (Parameter_2*)data;
	*((*parameter_2->mass) + (*(parameter_2->index))) = atoi(argv[0]);
	(*(parameter_2->index))++;
	return 0;
}


int check_code(int rc,char* err_msg)
{
	if(!rc)
	{
		printf("%s\n",err_msg);
			return 0;
	}
	return 1;
}
