#include "kernel/types.h"
#include "kernel/stat.h"

#include "user/user.h"
#include "user/thread.h"

void thread_test(void *a){
	*((int *)a) = *((int *)a)+1;
	printf("pid:%d in thread  0s : a = %d\n", getpid(), *((int *)a));
	sleep(5);
	
	*((int *)a) = *((int *)a)+1;
	
	printf("pid:%d in thread 10s : a = %d\n", getpid(), *((int *)a));
	return;
}

int main(void){
	int ret1, ret2;
	int *a = malloc(sizeof(int));
	*a = 0;

	ret1 = thread(thread_test, (void *)a);
	sleep(2);
	ret2 = thread(thread_test, (void *)a);
	join(&ret1);
	join(&ret2);
	printf("join end\n");
	exit(0);
}
