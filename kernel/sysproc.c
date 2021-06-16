#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "rand.h"


uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
uint64
sys_return_777(void)
{
	  return 777;
}
uint64
sys_return_tickets(void)
{
	return myproc()->tickets;
}
uint64
sys_return_called_times(void)
{
	return myproc()->called_times;
}
uint64
sys_return_random(void)
{
	return random_at_most(10);
}
uint64
sys_return_winner(void)
{
	return myproc()->winner;
}
uint64
sys_return_sum_tickets(void)
{
	return myproc()->sum_tickets;
}
uint64
sys_change_tickets(void)
{
	int tick;
	if(argint(0, &tick) < 0)
	  return -1;
	myproc()->tickets = tick;
	return 0;
}
uint64
sys_return_pass(void)
{
	return myproc()->pass;
}
uint64
sys_thread_create(void)
{
	return thread_create();
}
uint64
sys_t_exit(void)
{
	return t_exit();
}
uint64
sys_join(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return join(p);
}

/*
uint64
sys_init_list(void)
{
	uint64 L;
	if(argaddr(0, &L) < 0)
		return -1;
	struct list_t *l = (struct list_t *)L;
	initlist(l);
	return 0;
}

uint64
sys_insert_list(void)
{
	uint64 L;
	uint64 N;
	if(argaddr(0, &L) < 0)
		return -1;
	if(argaddr(1, &N) < 0)
		return -1;
	struct list_t *l = (struct list_t *)L;
	struct node_t *n = (struct node_t *)N;
	insertlist(l, n);
	return 0;
}

uint64
sys_show_all_hash(void)
{
	struct hash_t H;
	if(argaddr(0, &H) < 0)
		return -1;
	showallhash(&H);
	return 0;
}
*/
