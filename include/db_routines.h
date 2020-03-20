#ifndef _DB_ROUTINES_H_

	#define _DB_ROUTINES_H_
	
	#include "user.h"

	int login_and_connect_to_db(const char *, int, const char *, user_handle *, char **);
	void logout_and_close_db(user_handle);
	bool is_admin(user_handle);

	int select_orders_within_period(user_handle, char **, int, const char *, const char *);
	int select_car_stats(user_handle, char **, const char *);
	int select_driver_stats(user_handle, char **, int);
	int select_least_profitable_driver(user_handle, char **);
	int select_most_used_car(user_handle, char **);

	int insert_order(user_handle, char **);
	int insert_car(user_handle, char **);
	int insert_driver(user_handle, char **);
	int insert_admin(user_handle, char **);

	int update_order(user_handle, char **);
	int update_car(user_handle, char **);
	int update_driver(user_handle, char **);
	int update_password(user_handle, char **);

	int delete_order(user_handle, char **);
	int delete_car(user_handle, char **);
	int delete_driver(user_handle, char **);

#endif // _DB_ROUTINES_H_
