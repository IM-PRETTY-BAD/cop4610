#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include<linux/linkage.h>


void DFS(struct task_struct *task)
{   
    struct task_struct *child;
    struct list_head *list;

    printk("name: %s, parent pid: [%d], pid: [%d], state: %li\n", task->comm, task->parent->pid, task->pid, task->state);
    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        DFS(child);
    }
}

asmlinkage long sys_garrett_siqueira_weiland(void)
{
    DFS(&init_task);
    return 0;
}
