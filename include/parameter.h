/*
  #dmitars
  dmitriy.tarasienko@gmail.com
  2020

  Parameter.h
  declaration of the parameter, passed to function, that receives total sums
*/
#ifndef _PARAMETER_

#define _PARAMETER_

typedef struct Parameter{
	double* sum;
	char* start_date;
	char* end_date;
}Parameter;

#endif
