#include "../include/ui.h"
#include "../include/db_routines.h"

const char * DATABASE_FILE = "base/Autopark.db";

void handle_err_msg(char *err_msg)
{
	printf("An error has occurred: %s\n", err_msg);
	sqlite3_free(err_msg);
}

int main()
{
	int id;
	char *password;
	ask_credentials(&id, &password);

	char *err_msg;
	user_handle curr_user;
	int ret_code = login_and_connect_to_db(DATABASE_FILE, id, password, &curr_user, &err_msg);
	if (ret_code != SQLITE_OK)
	{
		handle_err_msg(err_msg);
		return ret_code;
	}

	choice menu_choice = CHOICE_INITIAL;
	while (menu_choice != CHOICE_EXIT)
	{
		system("clear");

		puts("Hello from the Autopark System!\nChoose an option:");
		menu_choice = choice_menu();

		switch (menu_choice)
		{
			case CHOICE_SELECT:
				{
					puts("Choose the info to find:");
					select_choice sel_menu_choice = select_choice_menu(curr_user);
					switch (sel_menu_choice)
					{				
						case SELECT_ALL_ORDERS: 
							{
								int driver_number = 0;
								char first_date[256], second_date[256];
								if (is_admin(curr_user))
								{
									puts("Input the number of the driver you want to find orders for:");
									scanf("%i", &driver_number);
								}
								puts("Input the beginning date:");
								scanf("%s", first_date);
								puts("Input the ending date:");
								scanf("%s", second_date);
								ret_code = select_orders_within_period(curr_user, &err_msg, 
																	   driver_number, 
																	   first_date, second_date);
							}
							break;

						case SELECT_CAR_STATS:
							{
								char car_number[256];
								puts("Input the car's number, please:");
								fgets(car_number, 256, stdin);
								char *pos = car_number;
								while (*pos != '\0')
								{
									if (*pos == '\r' || *pos == '\n')
										*pos = '\0';
									++pos;
								}
								ret_code = select_car_stats(curr_user, &err_msg, car_number);
							}
						   	break;

						case SELECT_DRIVER_STATS:
							{
								int driver_number = 0;
								if (is_admin(curr_user))
								{
									puts("Input the number of the driver you want to find orders for:");
									scanf("%i", &driver_number);
								}
								ret_code = select_driver_stats(curr_user, &err_msg, driver_number);
							}	
							break;

						case SELECT_MOST_USED_CAR:
							ret_code = select_most_used_car(curr_user, &err_msg);
							break;

						case SELECT_LEAST_PROFITABLE_DRIVER: 
							ret_code = select_least_profitable_driver(curr_user, &err_msg);
							break;
					}
				}
				break;

			case CHOICE_INSERT:
				{
					puts("Choose the category to insert data into:");
					table_choice tab_menu_choice = table_choice_menu();
					switch (tab_menu_choice)
					{
						case TABLE_CARS:
							ret_code = insert_car(curr_user, &err_msg);
							break;

						case TABLE_COMPLETED_ORDERS:
						    ret_code = insert_order(curr_user, &err_msg);
							break;

						case TABLE_DRIVERS:
							ret_code = insert_driver(curr_user, &err_msg);
						   	break;

						case TABLE_USERS:
							ret_code = insert_admin(curr_user, &err_msg);
						   	break;
					}
				}
				break;

			case CHOICE_UPDATE:
				{
					puts("Choose the category to update data in:");
					table_choice tab_menu_choice = table_choice_menu();
					switch (tab_menu_choice)
					{
						case TABLE_CARS:
							ret_code = update_car(curr_user, &err_msg);
							break;

						case TABLE_COMPLETED_ORDERS:
						    ret_code = update_order(curr_user, &err_msg);
							break;

						case TABLE_DRIVERS:
							ret_code = update_driver(curr_user, &err_msg);
						   	break;

						case TABLE_USERS:
							ret_code = update_password(curr_user, &err_msg);
						   	break;
					}
				}
				break;

			case CHOICE_DELETE:
				{
					puts("Choose the category to delete data from:");
					table_choice tab_menu_choice = table_choice_menu();
					switch (tab_menu_choice)
					{
						case TABLE_CARS:
							ret_code = delete_car(curr_user, &err_msg);
							break;

						case TABLE_COMPLETED_ORDERS:
						    	ret_code = delete_order(curr_user, &err_msg);
							break;

						case TABLE_DRIVERS:
							ret_code = delete_driver(curr_user, &err_msg);
						   	break;

						case TABLE_USERS:
							puts("Deletion of admins isn't supported!");
							ret_code = SQLITE_OK;
							break;
					}
				}
				break;

			case CHOICE_EXIT:
				ret_code = SQLITE_OK;
				break;
		}
		
		if (ret_code != SQLITE_OK)
			handle_err_msg(err_msg);	
	
		system("echo \"Press any key to continue . . .\"; read a");
	}

	logout_and_close_db(curr_user);
	free(password);
	return 0;
}
