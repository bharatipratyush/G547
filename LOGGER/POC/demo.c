#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include <linux/types.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/errno.h>
#include <linux/bio.h>
#include <asm/segment.h>
#include <linux/buffer_head.h>
#include <linux/jiffies.h>


char buf[4096];char buf1[7000][48];
	

__init int firstmod_init(void)
{
	struct file *f = NULL;
	
	int i, start;
	for(i=0;i<512;i++)
		buf[i] = 'r';
	int sample_rate =7000;

	
	printk(KERN_ALERT "FileRW Demo Module\n");
	
	f=filp_open("/dev/ttyACM0",0666,O_RDWR);
	if(f==NULL){
		printk(KERN_ALERT "File Open Err\n"); return 0;
	}
	else{

		start =jiffies; 


		 for (i = 0; i < sample_rate; ++i)
		 {		
		 	kernel_write(f,buf,1,0);
		 	kernel_read(f,0,buf1[i],48);
		 	
		 	//printk(KERN_INFO "%s",buf1);
			 
		 }




		printk("Time of transfer(ms):%d \n", jiffies_to_msecs(jiffies-start));
		printk("HZ:%d \n", HZ);
		//for (i = 0; i < 140; ++i)
		 	printk(KERN_INFO "%s",buf1[0]);
		 	printk(KERN_INFO "Sample Rate: %d \n", sample_rate*12);


	}
	filp_close(f,NULL);


	f = filp_open("sample.txt",0666,O_RDWR);
	if(f==NULL){
		printk(KERN_ALERT "File Open Err\n"); return 0;
	}

	kernel_write(f,buf1[0],336000,0);
	
	filp_close(f,NULL); 
	
	
	return 0;
}


__exit void firstmod_exit(void)
{
	printk(KERN_ALERT "Bye FILERW module\n\n");
	return;
}

module_init(firstmod_init);
module_exit(firstmod_exit);

MODULE_DESCRIPTION("File RW, EEE G547");
MODULE_AUTHOR("Devesh Samaiya <samaiya.devesh@gmail.com>");
MODULE_LICENSE("GPL");

