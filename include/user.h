#ifndef _USER_H_

	#define _USER_H_
	
	#include <stdbool.h>
	#include <sqlite3.h>
	
	typedef struct user 
	{
		int user_id;
		bool is_admin;
	}
	user;

	typedef struct _user
	{
		user authorized_user;
		sqlite3 *db;
	}
	*user_handle;

#endif // _USER_H_
