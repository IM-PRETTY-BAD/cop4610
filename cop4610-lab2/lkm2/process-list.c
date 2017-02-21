/* Winicius Siqueira */
/* COP 4610 Lab 2, Spring 2017 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>

/**
 * Performs a DFS on a given task's children.
 *
 * @void
 */
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

/**
 * This function is called when the module is loaded. 
 *
 * @return 0  upon success
 */ 
int task_lister_init(void)
{
    printk(KERN_INFO "Loading Task Lister Module...\n");
    DFS(&init_task);

    return 0;
}

/**
 * This function is called when the module is removed.
 *
 * @void
 */
void task_lister_exit(void)
{
    printk(KERN_INFO "Removing Task Lister Module...\n");
}

// Macros for registering module entry and exit points.
module_init(task_lister_init);
module_exit(task_lister_exit);


MODULE_AUTHOR("Winicius Siqueira");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello World, this is Winicius Siqueira");

