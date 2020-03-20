#include "../include/ui.h"
#include <stdio.h>
#include <stdlib.h>

#define PASSWORD_SIZE 1024

int _ui_generic_menu
			(int initial,
		     int first, int last,
		     const char **options)
{
	int result = initial;

	while (result < first || result > last)
	{
		for (int i = first; i <= last; ++i)
			printf("%i. %s\n", i - first + 1, options[i]);
		
		scanf("%i", &result);
		getchar();
		result += first - 1;

		if (result < first || result > last)
			puts("Try again!");		
	}
	
	return result;
}

const char * _choice_options[] = 
{
	"",
	"Find info",
	"Insert info",
	"Update info",
	"Delete info",
	"Exit"	
};

const char * _select_options[] = 
{
	"",
	"Find info on all purchases for the given period",
	"Find the total mileage and total amount of cargo for a car",
	"Find all stats for a driver",
	"Find all info on the most used car",
	"Find all info on the least profitable driver"
};

const char * _table_options[] =
{
	"",
	"Cars",
	"Completed Orders",
	"Drivers",
	"Users"
};

const char * _cars_field_options[] =
{
	"",
	"Brand",
	"Start mileage",
	"Carrying weight"
};

const char * _orders_field_options[] =
{
	"",
	"Mileage",
	"Weight",
	"Cost",
	"Date"
};

const char * _drivers_field_options[] =
{
	"",
	"Last name",
	"Category",
	"Experience",
	"Address",
	"Year of birth"
};

void ask_credentials(int *id, char **password)
{
	puts("Input your ID, please:");
	scanf("%i", id);
	getchar();
	
	*password = malloc(PASSWORD_SIZE);
	puts("Input your password, please:");
	fgets(*password, PASSWORD_SIZE, stdin);
	char *pos = *password;
	while (*pos != '\0')
	{
		if (*pos == '\r' || *pos == '\n')
			*pos = '\0';
		++pos;
	}
}

choice choice_menu()
{
	return _ui_generic_menu(CHOICE_INITIAL,
						    CHOICE_SELECT,
							CHOICE_EXIT,
							_choice_options);
}

select_choice select_choice_menu(user_handle usr)
{
	return _ui_generic_menu(SELECT_INITIAL,
							SELECT_ALL_ORDERS,
							usr->authorized_user.is_admin 
								? SELECT_LEAST_PROFITABLE_DRIVER
								: SELECT_MOST_USED_CAR,
							_select_options);
}

table_choice table_choice_menu()
{
	return _ui_generic_menu(TABLE_INITIAL,
							TABLE_CARS,
							TABLE_USERS,
							_table_options);
}

cars_choice cars_choice_menu()
{
	return _ui_generic_menu(CARS_INITIAL,
							CARS_BRAND,
							CARS_CARRYING,
							_cars_field_options);
}

orders_choice orders_choice_menu()
{
	return _ui_generic_menu(ORDERS_INITIAL,
						 	ORDERS_MILEAGE,
							ORDERS_DATE,
							_orders_field_options);
}

drivers_choice drivers_choice_menu()
{
	return _ui_generic_menu(DRIVERS_INITIAL,
							DRIVERS_LAST_NAME,
							DRIVERS_YEAR_OF_BIRTH,
							_drivers_field_options);
}
