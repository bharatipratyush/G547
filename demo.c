#include<linux/init.h>
#include<linux/module.h>

static int __init firstmod_init(void)
{	
	printk(KERN_ALERT "Hello Linux\n");
	return 0;
}

static void __exit firstmod_exit(void)
{
	printk(KERN_ALERT "Bye Linux\n\n");
}

module_init(firstmod_init);
module_exit(firstmod_exit);
