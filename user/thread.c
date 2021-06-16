#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/thread.h"
#include "kernel/param.h"
int thread(void(*fcn)(void*), void *arg)
{
	int ret;
	if((ret = thread_create()) == 0){
		printf("child thread\n");
		fcn(arg);
		printf("end function\n");
		t_exit();
	}
	return ret;
}
	
