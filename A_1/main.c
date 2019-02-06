	#include<linux/init.h>
	#include<linux/module.h>
	#include<linux/moduleparam.h>
	#include<linux/version.h>
	#include<linux/types.h>
	#include<linux/kdev_t.h>
	#include<linux/fs.h>
	#include<linux/device.h>
	#include<linux/cdev.h>
	#include<linux/kernel.h>
	#include <linux/random.h>
	#include <linux/uaccess.h>



	static dev_t ADXL_X;
	static dev_t ADXL_Y;
	static dev_t ADXL_Z;
	static struct cdev c_dev_x;
	static struct cdev c_dev_y;
	static struct cdev c_dev_z;
	static struct class *cls_X;
	static struct class *cls_Y;
	static struct class *cls_Z;
	static char *mystr = "assign1";
	static int myint = 666;

	//STEP 4.... callback functions

	static int my_open(struct inode *i, struct file *f)
		{
			printk(KERN_INFO "mychar: open()\n");
			return 0;
		}

	static int my_close(struct inode *i, struct file *f)
		{
			printk(KERN_INFO "mychar: close()\n");
			return 0;
		}

	static ssize_t read_x(struct file *f, char __user *buf, size_t len, loff_t *offp)
		{
			char rand[2];
			size_t count = len, status_length = strlen(rand);
    		ssize_t retval = 0;
    		unsigned long ret = 0;

			get_random_bytes(rand, sizeof(rand));
			printk(KERN_INFO "mychar: read_x(): %s \n", rand);
    		

    		if (*offp >= status_length)
        		goto out;
    		if (*offp + len > status_length)
       		count = status_length - *offp;

    		/* ret contains the amount of chars wasn't successfully written to `buf` */
    		ret = copy_to_user(buf, rand, count);
    		*offp += count - ret;
    		retval = count - ret;

			out:
    			return retval;
		}

	static ssize_t read_y(struct file *f, char __user *buf, size_t len, loff_t *offp)
		{
			char rand[2];
			size_t count = len, status_length = strlen(rand);
    		ssize_t retval = 0;
    		unsigned long ret = 0;

			get_random_bytes(rand, sizeof(rand));
			printk(KERN_INFO "mychar: read_y(): %s \n", rand);
    		

    		if (*offp >= status_length)
        		goto out;
    		if (*offp + len > status_length)
       		count = status_length - *offp;

    		/* ret contains the amount of chars wasn't successfully written to `buf` */
    		ret = copy_to_user(buf, rand, count);
    		*offp += count - ret;
    		retval = count - ret;

			out:
    			return retval;
		}

	static ssize_t read_z(struct file *f, char __user *buf, size_t len, loff_t *offp)
		{
			char rand[2];
			size_t count = len, status_length = strlen(rand);
    		ssize_t retval = 0;
    		unsigned long ret = 0;

			get_random_bytes(rand, sizeof(rand));
			printk(KERN_INFO "mychar: read_z(): %s \n", rand);
    		

    		if (*offp >= status_length)
        		goto out;
    		if (*offp + len > status_length)
       		count = status_length - *offp;

    		/* ret contains the amount of chars wasn't successfully written to `buf` */
    		ret = copy_to_user(buf, rand, count);
    		*offp += count - ret;
    		retval = count - ret;

			out:
    			return retval;
		}


	static struct file_operations fops_x =
					{
						.owner = THIS_MODULE,
						.open = my_open,
						.release = my_close,
						.read = read_x,
												
					};

	static struct file_operations fops_y =
					{
						.owner = THIS_MODULE,
						.open = my_open,
						.release = my_close,
						.read = read_y,
												
					};

	static struct file_operations fops_z =
					{
						.owner = THIS_MODULE,
						.open = my_open,
						.release = my_close,
						.read = read_z,
												
					};


	module_param(mystr, charp, S_IRUGO);
	module_param(myint, int, S_IRUGO);

	static int __init mychar_init(void)
	{	
		printk(KERN_INFO "ADXL_X \n\n");

		if((alloc_chrdev_region(&ADXL_X, 0, 1, "ADXL_X")) <0){
					printk(KERN_INFO "Cannot allocate major number for device ADXL_X\n");
					return -1;
			}

		printk(KERN_INFO "<Major, Minor>: <%d, %d>\n", MAJOR(ADXL_X), MINOR(ADXL_X));

		//----------------------------------------------------

		printk(KERN_INFO "ADXL_Y \n\n");

		if((alloc_chrdev_region(&ADXL_Y, 0, 1, "ADXL_Y")) <0){
					printk(KERN_INFO "Cannot allocate major number for device ADXL_Y\n");
					return -1;
			}

		printk(KERN_INFO "<Major, Minor>: <%d, %d>\n", MAJOR(ADXL_Y), MINOR(ADXL_Y));

		//------------------------------------------------------

		printk(KERN_INFO "ADXL_Z \n\n");

		if((alloc_chrdev_region(&ADXL_Z, 0, 1, "ADXL_Z")) <0){
					printk(KERN_INFO "Cannot allocate major number for device ADXL_Z\n");
					return -1;
			}

		printk(KERN_INFO "<Major, Minor>: <%d, %d>\n", MAJOR(ADXL_Z), MINOR(ADXL_Z));

		//---------------------------------------------------------
		//-------------------------------------------------------------
		//-------------------------------------------------------------

		//Step 2 creation of device fileS
		if((cls_X=class_create(THIS_MODULE, "CLASS_X"))==NULL)
		{
			unregister_chrdev_region(ADXL_X,1);
			return -1;
		}

		if(device_create(cls_X, NULL, ADXL_X, NULL, "DEV_ADXL_X")==NULL)
		{
			class_destroy(cls_X);
			unregister_chrdev_region(ADXL_X,1);
			return -1;
		}

		//-------------------------------------------------------------

		if((cls_Y=class_create(THIS_MODULE, "CLASS_Y"))==NULL)
		{
			unregister_chrdev_region(ADXL_Y,1);
			return -1;
		}

		if(device_create(cls_Y, NULL, ADXL_Y, NULL, "DEV_ADXL_Y")==NULL)
		{
			class_destroy(cls_Y);
			unregister_chrdev_region(ADXL_Y,1);
			return -1;
		}

		//-------------------------------------------------------------

		if((cls_Z=class_create(THIS_MODULE, "CLASS_Z"))==NULL)
		{
			unregister_chrdev_region(ADXL_Z,1);
			return -1;
		}

		if(device_create(cls_Z, NULL, ADXL_Z, NULL, "DEV_ADXL_Z")==NULL)
		{
			class_destroy(cls_Z);
			unregister_chrdev_region(ADXL_Z,1);
			return -1;
		}

		//-------------------------------------------------------------
		//-------------------------------------------------------------
		//-------------------------------------------------------------

		//STEP 3 Link fops and cdev to the device node
		cdev_init(&c_dev_x, &fops_x);
		if(cdev_add(&c_dev_x, ADXL_X,1)==-1)
		{
			printk(KERN_INFO "Linking fops_x and cdev_x\n");		
			device_destroy(cls_X,ADXL_X);
			class_destroy(cls_X);
			unregister_chrdev_region(ADXL_X,1);
			return -1;
		}

		//--------------------------------------------------------------

		cdev_init(&c_dev_y, &fops_y);
		if(cdev_add(&c_dev_y, ADXL_Y,1)==-1)
		{
			printk(KERN_INFO "Linking fops_y and cdev_y\n");		
			device_destroy(cls_Y,ADXL_Y);
			class_destroy(cls_Y);
			unregister_chrdev_region(ADXL_Y,1);
			return -1;
		}

		//----------------------------------------------------------------

		cdev_init(&c_dev_z, &fops_z);
		if(cdev_add(&c_dev_z, ADXL_Z,1)==-1)
		{
			printk(KERN_INFO "Linking fops_z and cdev_z\n");		
			device_destroy(cls_Z,ADXL_Z);
			class_destroy(cls_Z);
			unregister_chrdev_region(ADXL_Z,1);
			return -1;
		}

		//----------------------------------------------------------------
			
		return 0;
	}

	static void __exit mychar_exit(void)
	{
		cdev_del(&c_dev_x);
		device_destroy(cls_X,ADXL_X);
		class_destroy(cls_X);
		unregister_chrdev_region(ADXL_X,1);

		cdev_del(&c_dev_y);
		device_destroy(cls_Y,ADXL_Y);
		class_destroy(cls_Y);
		unregister_chrdev_region(ADXL_Y,1);

		cdev_del(&c_dev_z);
		device_destroy(cls_Z,ADXL_Z);
		class_destroy(cls_Z);
		unregister_chrdev_region(ADXL_Z,1);
		
		printk(KERN_INFO "Bye Linux\n\n");
	}

	module_init(mychar_init);
	module_exit(mychar_exit);

	MODULE_PARM_DESC(myint,"STUFF");
	MODULE_DESCRIPTION("ASSIGNMENT 1");
	MODULE_AUTHOR("PRATYUSH BHARATI");
	MODULE_LICENSE("GPL");
