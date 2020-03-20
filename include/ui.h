#ifndef _UI_H_

	#define _UI_H_

	#include <stdio.h>
	#include <sqlite3.h>
	#include "user.h"

	typedef enum choice
	{
		CHOICE_INITIAL,
		CHOICE_SELECT,
		CHOICE_INSERT,
		CHOICE_UPDATE,
		CHOICE_DELETE,
		CHOICE_EXIT,
	}
	choice;

	typedef enum select_choice
	{
		SELECT_INITIAL,
		SELECT_ALL_ORDERS,
		SELECT_CAR_STATS,
		SELECT_DRIVER_STATS,
		SELECT_MOST_USED_CAR,
		SELECT_LEAST_PROFITABLE_DRIVER
	}
	select_choice;

	typedef enum table_choice
	{
		TABLE_INITIAL,
		TABLE_CARS,
		TABLE_COMPLETED_ORDERS,
		TABLE_DRIVERS,
		TABLE_USERS
	}
	table_choice;

	typedef enum cars_choice
	{
		CARS_INITIAL,
		CARS_BRAND,
		CARS_START_MILEAGE,
		CARS_CARRYING
	}
	cars_choice;

	typedef enum orders_choice
	{
		ORDERS_INITIAL,
		ORDERS_MILEAGE,
		ORDERS_WEIGHT,
		ORDERS_COST,
		ORDERS_DATE
	}
	orders_choice;

	typedef enum drivers_choice
	{
		DRIVERS_INITIAL,
		DRIVERS_LAST_NAME,
		DRIVERS_CATEGORY,
		DRIVERS_EXPERIENCE,
		DRIVERS_ADDRESS,
		DRIVERS_YEAR_OF_BIRTH
	}
	drivers_choice;

	void ask_credentials(int *, char **);
	choice choice_menu();
	select_choice select_choice_menu(user_handle);
	table_choice table_choice_menu();
	cars_choice cars_choice_menu();
	orders_choice orders_choice_menu();
	drivers_choice drivers_choice_menu();

#endif // _UI_H_
