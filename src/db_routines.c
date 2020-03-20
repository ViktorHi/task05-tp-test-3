#include "../include/db_routines.h"
#include "../include/ui.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef int (*callback) (void *, int, char **, char **);
typedef struct _login_info
{
	char *password;
	bool is_admin;
}
_login_info;

#define BUFFER_SIZE 1024

int _login_callback
	(void *data, 
	 int num_of_columns, 
	 char **columns, 
	 char **headers)
{
	_login_info *info = (_login_info *)data;
	for (int i = 0; i < num_of_columns; ++i)
	{
		if (!strcmp(headers[i], "password"))
		{
			info->password = malloc(strlen(columns[i]) + 1);
			strcpy(info->password, columns[i]);
		}	
		else 
			info->is_admin = strtol(columns[i], NULL, 10);
	}
	return 0;
}

int _select_callback
	(void *ignored,
	 int num_of_columns,
	 char **columns,
	 char **headers)
{
	for (int i = 0; i < num_of_columns; ++i)
		printf("%s = %s\n", headers[i], columns[i]);
	return 0;
}

void _copy_error_msg
	(char **err_msg, 
	 const char *msg)
{
	if (err_msg != NULL)
	{
		*err_msg = sqlite3_malloc(strlen(msg) + 1);
		strcpy(*err_msg, msg);
	}
}

bool _is_date_correct
	(const char *date)
{
	int day, month, year;
	sscanf(date, "%i-%i-%i", &year, &month, &day);
	
	bool result = (day >= 1 && day <= 31) &&
				  (month >= 1 && month <= 12) &&
				  (year >= 1601);
	result = result && !(day == 31 && (month == 2 || month == 4 ||
						   			   month == 6 || month == 9 ||
									   month == 11));
	result = result && !(day == 30 && month == 2);
	return result;
}

void _fgets_without_newline
	(char *buffer,
	 int number,
	 FILE *file)
{
	fgets(buffer, number, file);
	char *pos = buffer;
	while (*pos != '\0')
	{
		if (*pos == '\r' || *pos == '\n')
			*pos = '\0';
		++pos;
	}
}

int login_and_connect_to_db
	(const char *db_name,
	 int id,
	 const char *password,
	 user_handle *handle,
	 char **err_msg)
{
	*handle = malloc(sizeof(*handle));
	
	int ret_code = sqlite3_open(db_name, &((*handle)->db));
	if (ret_code != SQLITE_OK)
	{
		_copy_error_msg(err_msg, sqlite3_errmsg((*handle)->db));
		return ret_code;
	}

	char query[BUFFER_SIZE] = { '\0' };
	sprintf(query, 
			"SELECT password, is_admin FROM Autopark_users \
			 WHERE id = %i;", id);
	_login_info info;
	info.password = NULL;
	ret_code = sqlite3_exec((*handle)->db, query, _login_callback, 
						    &info, err_msg);
	if (ret_code != SQLITE_OK)
		return ret_code;
	
	if (info.password == NULL)
	{
		_copy_error_msg(err_msg, "No such login!");
		return SQLITE_ERROR;
	}

	if (strcmp(password, info.password))
	{
		_copy_error_msg(err_msg, "Passwords do not match!");
		return SQLITE_ERROR;
	}
	free(info.password);

	(*handle)->authorized_user.user_id = id;
	(*handle)->authorized_user.is_admin = info.is_admin;

	return SQLITE_OK;
}

void logout_and_close_db(user_handle handle)
{
	sqlite3_close(handle->db);
	free(handle);
}

int select_orders_within_period
	(user_handle handle,
	 char **err_msg,
	 int driver_id,
	 const char *start_date,
	 const char *end_date)
{
	if (!_is_date_correct(start_date))
	{
		_copy_error_msg(err_msg, "The start date is incorrect!");
		return SQLITE_ERROR;
	}
	if (!_is_date_correct(end_date))
	{
		_copy_error_msg(err_msg, "The end date is incorrect!");
		return SQLITE_ERROR;
	}

	if (!handle->authorized_user.is_admin)
		driver_id = handle->authorized_user.user_id;
	char query[BUFFER_SIZE] = { '\0' };
	sprintf(query, 
			"SELECT id AS \"ID\", \
					car_number AS \"Car number\", \
		    		mileage AS \"Mileage\", \
				   	weight AS \"Weight\", \
					cost AS \"Cost\", \
					date AS \"Date\" \
			 FROM Autopark_completed_orders \
			 WHERE driver_number = %i \
				   AND date BETWEEN '%s' AND '%s';",
			 driver_id, start_date, end_date);

	return sqlite3_exec(handle->db, query, _select_callback, NULL, err_msg);
}

bool is_admin
	(user_handle handle)
{
	if (handle == NULL)
		return false;
	return handle->authorized_user.is_admin;
}

int select_car_stats
	(user_handle handle,
	 char **err_msg,
	 const char *car_number)
{	
	char query[BUFFER_SIZE] = { '\0' };
	int len = sprintf(query,
			"SELECT sum(mileage) AS \"Total mileage\", \
			 		sum(weight) AS \"Total weight\" \
			 FROM Autopark_completed_orders \
			 WHERE car_number = '%s'", 
			 car_number);
	if (!handle->authorized_user.is_admin)
		len += sprintf(query + len, " AND driver_number = %i", 
					     handle->authorized_user.user_id);
	sprintf(query + len, ";");

	return sqlite3_exec(handle->db, query, _select_callback, NULL, err_msg);
}

int select_driver_stats
	(user_handle handle,
	 char **err_msg,
	 int driver_id)
{	
	if (!handle->authorized_user.is_admin)
		driver_id = handle->authorized_user.user_id;
	char query[BUFFER_SIZE] = { '\0' };
	sprintf(query,
			"SELECT count(id) AS \"Number of orders\", \
					sum(cost) AS \"Total profit\", \
			 		sum(weight) AS \"Total weight\" \
			 FROM Autopark_completed_orders \
			 WHERE driver_number = %i;", 
			 driver_id);

	return sqlite3_exec(handle->db, query, _select_callback, NULL, err_msg);
}

int select_least_profitable_driver
	(user_handle handle,
	 char **err_msg)
{	
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't perform operation: user isn't an admin!");
		return SQLITE_ERROR;
	}
	char query[2 * BUFFER_SIZE] = 
			 "WITH min_profit AS (SELECT min(Total_Profit) AS result \
			 	 				  FROM (SELECT sum(cost) AS Total_Profit \
									    FROM Autopark_completed_orders \
										GROUP BY driver_number)) \
			 SELECT Autopark_drivers.number AS \"Driver number\", \
			 		last_name AS \"Last name\", \
					category AS \"Category\", \
					experience AS \"Experience\", \
					address AS \"Address\", \
					year_of_birth AS \"Year of birth\", \
					sum(cost) AS \"Total profit\" \
			 FROM min_profit, Autopark_drivers INNER JOIN Autopark_completed_orders \
			 ON Autopark_drivers.number = Autopark_completed_orders.driver_number \
			 GROUP BY Autopark_drivers.number \
			 HAVING sum(cost) = min_profit.result;";

	return sqlite3_exec(handle->db, query, _select_callback, NULL, err_msg);
}

int select_most_used_car
	(user_handle handle,
	 char **err_msg)
{	
	char query[3 * BUFFER_SIZE] = 
			 "WITH max_mileage AS (SELECT max(Total_Mileage) AS result \
			 	 				   FROM (SELECT sum(mileage) AS Total_Mileage \
									     FROM Autopark_completed_orders \
									     GROUP BY car_number";
	int len = strlen(query);
	if (!handle->authorized_user.is_admin)
		len += sprintf(query + len, " HAVING driver_number = %i", 
				handle->authorized_user.user_id);
	len += sprintf(query + len, ")) \
			 SELECT Autopark_cars.car_number AS \"Car number\", \
			 		brand AS \"Brand\", \
					start_mileage AS \"Start mileage\", \
					carrying AS \"Carrying\", \
					sum(mileage) AS \"Total mileage\" \
			 FROM max_mileage, Autopark_cars INNER JOIN Autopark_completed_orders \
			 ON Autopark_cars.car_number = Autopark_completed_orders.car_number \
			 GROUP BY Autopark_cars.car_number \
			 HAVING sum(mileage) = max_mileage.result");
	if (!handle->authorized_user.is_admin)
		len += sprintf(query + len, " AND driver_number = %i", 
				handle->authorized_user.user_id);
	sprintf(query + len, ";");

	puts(query);

	return sqlite3_exec(handle->db, query, _select_callback, NULL, err_msg);
}

int insert_order
	(user_handle handle,
	 char **err_msg)
{
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't insert: not an admin!");
		return SQLITE_ERROR;
	}

	char query[4 * BUFFER_SIZE] = 
			"INSERT INTO Autopark_completed_orders \
			 (driver_number, car_number, mileage, \
			  weight, cost, date) VALUES (";
	int len = strlen(query);

	puts("Type the driver's number, please:");
	int driver_number;
	scanf("%i", &driver_number);
	len += sprintf(query + len, "%i, ", driver_number);
	
	getchar();
	puts("Type the car's number, please:");
	char car_number[BUFFER_SIZE];
	_fgets_without_newline(car_number, BUFFER_SIZE, stdin);
	len += sprintf(query + len, "'%s', ", car_number);
	
	puts("Type the mileage, please:");
	double mileage;
	scanf("%lf", &mileage);
	len += sprintf(query + len, "%lf, ", mileage);
	
	puts("Type the weight, please:");
	double weight;
	scanf("%lf", &weight);
	len += sprintf(query + len, "%lf, ", weight);
	
	puts("Type the cost, please:");
	double cost;
	scanf("%lf", &cost);
	len += sprintf(query + len, "%lf, ", cost);
	
	getchar();
	puts("Type the date in the format YYYY-MM-DD, please:");
	char date[BUFFER_SIZE];
	_fgets_without_newline(date, BUFFER_SIZE, stdin);
	len += sprintf(query + len, "'%s');", date);
	if (!_is_date_correct(date))
	{
		_copy_error_msg(err_msg, "You typed an incorrect date!");
		return SQLITE_ERROR;
	}

	return sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
}

int insert_car
	(user_handle handle,
	 char **err_msg)
{
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't insert: not an admin!");
		return SQLITE_ERROR;
	}

	char query[4 * BUFFER_SIZE] = 
			"INSERT INTO Autopark_cars \
			 (car_number, brand, start_mileage, \
			  carrying) VALUES (";
	int len = strlen(query);

	puts("Type the car's number, please:");
	char car_number[BUFFER_SIZE];
	_fgets_without_newline(car_number, BUFFER_SIZE, stdin);
	len += sprintf(query + len, "'%s', ", car_number);
	
	puts("Type the brand, please:");
	char brand[BUFFER_SIZE];
	_fgets_without_newline(brand, BUFFER_SIZE, stdin);
	len += sprintf(query + len, "'%s', ", brand);
	
	puts("Type the start mileage, please:");
	double mileage;
	scanf("%lf", &mileage);
	len += sprintf(query + len, "%lf, ", mileage);
	
	puts("Type the carrying weight, please:");
	double weight;
	scanf("%lf", &weight);
	len += sprintf(query + len, "%lf);", weight);

	return sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
}

int insert_driver
	(user_handle handle,
	 char **err_msg)
{
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't insert: not an admin!");
		return SQLITE_ERROR;
	}
	
	int ret_code = sqlite3_exec(handle->db, "BEGIN; ", NULL, NULL, err_msg);
	if (ret_code != SQLITE_OK)
		return ret_code;

	char query[16 * BUFFER_SIZE] = 
			"INSERT INTO Autopark_drivers \
			 (number, last_name, category, \
			  experience, address, year_of_birth) VALUES (";
	int len = strlen(query);

	puts("Type the driver's number, please:");
	int driver_number;
	scanf("%i", &driver_number);
	len += sprintf(query + len, "%i, ", driver_number);
	
	getchar();
	puts("Type the driver's last name, please:");
	char last_name[BUFFER_SIZE];
	_fgets_without_newline(last_name, BUFFER_SIZE, stdin);
	len += sprintf(query + len, "'%s', ", last_name);
	
	puts("Type the driver's category, please:");
	char category[BUFFER_SIZE];
	_fgets_without_newline(category, BUFFER_SIZE, stdin);
	len += sprintf(query + len, "'%s', ", category);
	
	puts("Type the years of experience, please:");
	int experience;
	scanf("%i", &experience);
	len += sprintf(query + len, "%i, ", experience);
	
	getchar();
	puts("Type the driver's address, please:");
	char address[BUFFER_SIZE];
	_fgets_without_newline(address, BUFFER_SIZE, stdin);
	len += sprintf(query + len, "'%s', ", address);
	
	puts("Type the year of birth, please:");
	int year_of_birth;
	scanf("%i", &year_of_birth);
	len += sprintf(query + len, "%i);", year_of_birth);
	
	getchar();
	puts("Type the driver's password, please:");
	char password[BUFFER_SIZE];
	_fgets_without_newline(password, BUFFER_SIZE, stdin);

	ret_code = sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
	if (ret_code != SQLITE_OK)
	{
		sqlite3_exec(handle->db, "ROLLBACK;", NULL, NULL, NULL);
		return ret_code;
	}

	query[0] = '\0';
	sprintf(query, 
			"INSERT INTO Autopark_users (id, password, is_admin) \
			 VALUES (%i, '%s', 0);",
			 driver_number, password);

	ret_code = sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
	if (ret_code != SQLITE_OK)
	{
		sqlite3_exec(handle->db, "ROLLBACK;", NULL, NULL, NULL);
		return ret_code;
	}

	return sqlite3_exec(handle->db, "COMMIT;", NULL, NULL, err_msg);
}

int insert_admin
	(user_handle handle,
	 char **err_msg)
{
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't insert: not an admin!");
		return SQLITE_ERROR;
	}

	char query[2 * BUFFER_SIZE] = 
			"INSERT INTO Autopark_users (id, password, is_admin) \
			 VALUES (";
	int len = strlen(query);

	puts("Type the admin's ID, please:");
	int id;
	scanf("%i", &id);
	len += sprintf(query + len, "%i, ", id);
	
	getchar();
	puts("Type the driver's password, please:");
	char password[BUFFER_SIZE];
	_fgets_without_newline(password, BUFFER_SIZE, stdin);
	len += sprintf(query + len, "'%s', 1);", password);

	return sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
}

int update_order
	(user_handle handle,
	 char **err_msg)
{
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't update: not an admin!");
		return SQLITE_ERROR;
	}

	int order_id;
	puts("Input the order ID to update:");
	scanf("%i", &order_id);

	char query[2 * BUFFER_SIZE] = 
		"UPDATE Autopark_completed_orders SET ";
	int len = strlen(query);

	puts("Choose the field to update:");
	orders_choice chosen_field = orders_choice_menu();
	switch (chosen_field)
	{
		case ORDERS_MILEAGE:
			{
				double mileage;
				puts("Input the mileage, please:");
				scanf("%lf", &mileage);
				len += sprintf(query + len, "mileage = %lf", mileage);
			}
			break;

		case ORDERS_WEIGHT:
			{
				double weight;
				puts("Input the weight, please:");
				scanf("%lf", &weight);
				len += sprintf(query + len, "weight = %lf", weight);
			}
			break;

		case ORDERS_COST:
			{
				double cost;
				puts("Input the cost, please:");
				scanf("%lf", &cost);
				len += sprintf(query + len, "cost = %lf", cost);
			}
			break;

		case ORDERS_DATE:
			{
				getchar();
				char date[BUFFER_SIZE];
				puts("Input the order date, please:");
				_fgets_without_newline(date, BUFFER_SIZE, stdin);
				len += sprintf(query + len, "date = '%s'", date);
				if (!_is_date_correct(date))
				{
					_copy_error_msg(err_msg, "You typed incorrect date!");
					return SQLITE_ERROR;
				}
			}
			break;	
	}
	len += sprintf(query + len, " WHERE id = %i;", order_id);

	return sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
}

int update_car
	(user_handle handle,
	 char **err_msg)
{
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't update: not an admin!");
		return SQLITE_ERROR;
	}

	char car_number[BUFFER_SIZE];
	puts("Input the car number to update:");
	_fgets_without_newline(car_number, BUFFER_SIZE, stdin);

	char query[3 * BUFFER_SIZE] = 
		"UPDATE Autopark_cars SET ";
	int len = strlen(query);	

	puts("Choose the field to update:");
	cars_choice chosen_field = cars_choice_menu();
	switch (chosen_field)
	{
		case CARS_BRAND:
			{
				char brand[BUFFER_SIZE];
				puts("Input the brand, please:");
				_fgets_without_newline(brand, BUFFER_SIZE, stdin);
				len += sprintf(query + len, "brand = '%s'", brand);
			}
			break;

		case CARS_START_MILEAGE:
			{
				double start_mileage;
				puts("Input the start mileage, please:");
				scanf("%lf", &start_mileage);
				len += sprintf(query + len, "start_mileage = %lf", start_mileage);
			}
			break;

		case CARS_CARRYING:
			{
				double carrying;
				puts("Input the carrying capacity, please:");
				scanf("%lf", &carrying);
				len += sprintf(query + len, "carrying = %lf", carrying);
			}
			break;
	}
	len += sprintf(query + len, " WHERE car_number = '%s';", car_number);

	return sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
}

int update_driver
	(user_handle handle,
	 char **err_msg)
{
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't update: not an admin!");
		return SQLITE_ERROR;
	}

	int driver_id;
	puts("Input the driver number to update:");
	scanf("%i", &driver_id);

	char query[2 * BUFFER_SIZE] = 
		"UPDATE Autopark_drivers SET ";
	int len = strlen(query);

	puts("Choose the field to update:");
	drivers_choice chosen_field = drivers_choice_menu();
	switch (chosen_field)
	{
		case DRIVERS_LAST_NAME:
			{
				getchar();
				char last_name[BUFFER_SIZE];
				puts("Input the driver's last name, please:");
				_fgets_without_newline(last_name, BUFFER_SIZE, stdin);
				len += sprintf(query + len, "last_name = '%s'", last_name);
			}
			break;

		case DRIVERS_CATEGORY:
			{
				getchar();
				char category[BUFFER_SIZE];
				puts("Input the driver's category, please:");
				_fgets_without_newline(category, BUFFER_SIZE, stdin);
				len += sprintf(query + len, "category = '%s'", category);
			}
			break;

		case DRIVERS_EXPERIENCE:
			{
				int experience;
				puts("Input the driver's experience, please:");
				scanf("%i", &experience);
				len += sprintf(query + len, "experience = %i", experience);
			}
			break;

		case DRIVERS_ADDRESS:
			{
				getchar();
				char address[BUFFER_SIZE];
				puts("Input the driver's address, please:");
				_fgets_without_newline(address, BUFFER_SIZE, stdin);
				len += sprintf(query + len, "address = '%s'", address);
			}
			break;	
		
		case DRIVERS_YEAR_OF_BIRTH:
			{
				int year_of_birth;
				puts("Input the driver's year of birth, please:");
				scanf("%i", &year_of_birth);
				len += sprintf(query + len, "year_of_birth = %i", year_of_birth);
			}
			break;	
	}
	len += sprintf(query + len, " WHERE number = %i;", driver_id);

	return sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
}

int update_password
	(user_handle handle,
	 char **err_msg)
{
	char query[2 * BUFFER_SIZE] =
			"UPDATE Autopark_users SET password = ";
	int len = strlen(query);
	char new_password[BUFFER_SIZE];
	puts("Input new password, please:");
	_fgets_without_newline(new_password, BUFFER_SIZE, stdin);
	sprintf(query + len, "'%s' WHERE id = %i;",
			new_password, handle->authorized_user.user_id);

	return sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
}

int delete_order
	(user_handle handle,
	 char **err_msg)
{
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't delete: not an admin!");
		return SQLITE_ERROR;
	}

	char query[BUFFER_SIZE] = "DELETE FROM Autopark_completed_orders \
							   WHERE id = ";
	int len = strlen(query);
	int id;
	puts("Input the ID of an order you want to delete:");
	scanf("%i", &id);
	len += sprintf(query + len, "%i;", id);

	return sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
}

int delete_car
	(user_handle handle,
	 char **err_msg)
{
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't delete: not an admin!");
		return SQLITE_ERROR;
	}

	char query[2 * BUFFER_SIZE] = "DELETE FROM Autopark_cars \
							   WHERE car_number = ";
	int len = strlen(query);
	char car_number[BUFFER_SIZE];
	puts("Input the number of a car you want to delete:");
	_fgets_without_newline(car_number, BUFFER_SIZE, stdin);
	len += sprintf(query + len, "'%s';", car_number);

	return sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
}

int delete_driver
	(user_handle handle,
	 char **err_msg)
{
	if (!handle->authorized_user.is_admin)
	{
		_copy_error_msg(err_msg, "Can't delete: not an admin!");
		return SQLITE_ERROR;
	}

	int ret_code = sqlite3_exec(handle->db, "BEGIN;", NULL, NULL, err_msg);
	if (ret_code != SQLITE_OK)
		return ret_code;

	char query[BUFFER_SIZE] = "DELETE FROM Autopark_drivers \
							   WHERE number = ";
	int len = strlen(query);
	int id;
	puts("Input the number of a driver you want to delete:");
	scanf("%i", &id);
	len += sprintf(query + len, "%i;", id);

	ret_code = sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
	if (ret_code != SQLITE_OK)
	{
		sqlite3_exec(handle->db, "ROLLBACK;", NULL, NULL, NULL);
		return ret_code;
	}
	
	query[0] = '\0';
	sprintf(query, "DELETE FROM Autopark_users WHERE id = %i;", id);

	ret_code = sqlite3_exec(handle->db, query, NULL, NULL, err_msg);
	if (ret_code != SQLITE_OK)
	{
		sqlite3_exec(handle->db, "ROLLBACK;", NULL, NULL, NULL);
		return ret_code;
	}

	return sqlite3_exec(handle->db, "COMMIT;", NULL, NULL, err_msg);
}
