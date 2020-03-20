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

int test_choice_menu(){
	void* data;
	int argc =1;
	char *argv="_select_callback()";
	char *azColName="working: ";
	_select_callback(data, argc, &argv, &azColName);
	return S_OK;
}

int run_tests() {
	if (E_FAIL==test_check_code()){
		printf("failed test_check_code()\n");
		return E_FAIL;
	}

	if (E_FAIL==test_choice_menu()){
		printf("failed test_choice_menu()\n");
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