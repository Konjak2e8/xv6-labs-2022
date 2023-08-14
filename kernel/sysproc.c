#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "stat.h"

extern struct inode*
create(char *path, short type, short major, short minor);

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
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

  argint(0, &pid);
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
sys_symlink(void)
{
  char dst[MAXPATH], path[MAXPATH];
  struct inode *ip;
  int n;

  if((n = argstr(0, dst, MAXPATH)) < 0 || argstr(1, path, MAXPATH) < 0)
    return -1;

  begin_op();

  // create a new symlink, return with a locked inode
  ip = create(path, T_SYMLINK, 0, 0);
  if (ip == 0) {
      end_op();
      return -1;
  }

  // write the target into the symlink's data block
  if (writei(ip, 0, (uint64)dst, 0, n) != n) {
    end_op();
    return -1;
  }

  iunlockput(ip);
  end_op();
  return 0;
}