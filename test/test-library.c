#include "../include/sum_counter.h"
#include "../include/db_routines.h"
#define S_OK (0)
#define E_FAIL (-1)


int test_check_code(){

	char* err_mess=NULL;
	if(check_code(1,err_mess)==1){
		return S_OK;
	}
	return E_FAIL;
}

int test_select_callback(){
	void* data;
	int argc =1;
	char *argv="_select_callback()";
	char *azColName="working: ";
	if(_select_callback(data, argc, &argv, &azColName)==0)
		return S_OK;
	return E_FAIL;
}

int test_is_date_correct(){
	char* is_correct="12-12-2012";
	if(_is_date_correct(is_correct)==0)
		return S_OK;
	return E_FAIL;
}

int test_is_date2_correct(){
	char* is_correct="12-1-20120";
	int i;
	if((i=_is_date_correct(is_correct))==0)
		return S_OK;
	return E_FAIL;
}

int test_is_admin(){
	user_handle handle=NULL;
	if(is_admin(handle)==0)
		return S_OK;
	return E_FAIL;
}

int run_tests() {
	if (E_FAIL==test_check_code()){
		printf("failed test_check_code()\n");
		return E_FAIL;
	}

	if (E_FAIL==test_select_callback()){
		printf("failed test_select_callback()\n");
		return E_FAIL;
	}

	if (E_FAIL==test_is_date_correct()){
		printf("failed test_is_date_correct()\n");
		return E_FAIL;
	}

	if (E_FAIL==test_is_admin()){
		printf("failed test_is_admin()\n");
		return E_FAIL;
	}
	if (E_FAIL==test_is_date2_correct()){
		printf("failed test_is_date2_correct()\n");
		return E_FAIL;
	}
	return S_OK;
}

int main()
{
	int result;
	
	printf("Running Example tests .. \n");
	result = run_tests();

	if (result == S_OK) 
		printf ("tests passed.\n");
	else
		printf ("tests failed.\n");

    return result;  /* remember the value 0 is considered passing in a travis-ci sense */

}