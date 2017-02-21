#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/sched.h>


void DFS(struct task_struct *task)
{   
    struct task_struct *t;
    struct task_struct *p;

    for_each_process(p){
        t=p;
        printk("name: %s, parent pid:[%d], pid: [%d], state: %li\n", p->comm, p->parent->pid, p->pid, p->state);
        do{
            printk("---thread---name: %s, parent pid:[%d], pid: [%d], state: %li\n", t->comm, t->parent->pid, t->pid, t->state);
        }while_each_thread(p, t);
    }

}

asmlinkage long sys_garrett_siqueira_weiland(void)
{
  DFS(&init_task);
  return 0;
}
