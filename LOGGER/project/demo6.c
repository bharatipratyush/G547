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
#include <sound/core.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/info.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/ctype.h>
#include <linux/pm.h>
#include <linux/completion.h>
#include<linux/interrupt.h>
#include <sound/pcm.h>
#include <linux/uaccess.h>

#define IRQ_NUM 0x82


char *buf1;
static dev_t char_dev_dev_t;
static dev_t char_dev_2;
struct snd_card *card;
struct device* mydevice;
struct device* mydevice2;
void *dev_data;
static int dummy;
int i;
struct snd_pcm *pcm;
static struct class *cls_X;
static struct class *cls_X2;
const char* name= "MySoundCard";
struct file *f;
char buf = 'r';

static struct snd_device_ops ops = { NULL };

struct mydev {
        struct snd_card *card;
        struct snd_pcm *pcm;
        unsigned long port;
        char* buffer;
        int irq;
};

struct mydev my_dev;


static struct snd_pcm_hardware my_pcm_hw = {
        .info = (SNDRV_PCM_INFO_MMAP |
                 SNDRV_PCM_INFO_INTERLEAVED |
                 SNDRV_PCM_INFO_BLOCK_TRANSFER |
                 SNDRV_PCM_INFO_MMAP_VALID),
        .formats          = SNDRV_PCM_FMTBIT_U8,
        .rates            = SNDRV_PCM_RATE_5512,
        .rate_min         = 1400,
        .rate_max         = 5512,
        .channels_min     = 1,
        .channels_max     = 1,
        .buffer_bytes_max = (32 * 48),
        .period_bytes_min = 48,
        .period_bytes_max = 48,
        .periods_min      = 1,
        .periods_max      = 32,
};


int my_pcm_open(struct snd_pcm_substream *substream)
{
	f=filp_open("/dev/ttyACM1",0666,O_RDWR);
	if(f==NULL){
		printk(KERN_ALERT "File Open Err\n"); return 0;
	}
	substream->runtime->hw = my_pcm_hw;
	substream->private_data = &my_dev;
	printk(KERN_INFO "my_pcm_open \n\n"); return 0;

}

int my_pcm_close(struct snd_pcm_substream *substream)
{
	//filp_close(f,NULL);
	substream->private_data = NULL;
	printk(KERN_INFO "my_pcm_close \n\n"); return 0;

}


int my_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	printk(KERN_INFO "my_hw_params \n\n"); return snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(params));

}

int my_hw_free(struct snd_pcm_substream *substream)
{
	printk(KERN_INFO "my_hw_free \n\n"); return snd_pcm_lib_free_pages(substream);

}



int my_pcm_prepare(struct snd_pcm_substream *substream)
{
	printk(KERN_INFO "my_pcm_prepare \n\n"); return 0;

}

int my_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	printk(KERN_INFO "my_pcm_trigger \n\n");

	struct mydev *my_dev1 = snd_pcm_substream_chip(substream);
	int ret = 0;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
			enable_irq(IRQ_NUM);
	        // Start the hardware capture
			printk(KERN_INFO "my_pcm_trigger START\n\n");
	        break;
	case SNDRV_PCM_TRIGGER_STOP:
	        // Stop the hardware capture
			disable_irq(IRQ_NUM);
			printk(KERN_INFO "my_pcm_trigger STOP\n\n");
	        break;
	default:
	        ret = -EINVAL;
	}

	return ret;

}

snd_pcm_uframes_t my_pcm_pointer(struct snd_pcm_substream *substream)
{
	printk(KERN_INFO "my_pcm_pointer \n\n"); 
	
	struct mydev *my_dev1 = snd_pcm_substream_chip(substream);

	return 0;

}


static int my_pcm_copy(struct snd_pcm_substream *ss,
                       int channel, snd_pcm_uframes_t pos,
                       void __user *dst,
                       snd_pcm_uframes_t count)
{
        struct mydev *my_dev1 = snd_pcm_substream_chip(ss);

        return copy_to_user(dst, my_dev1->buffer + pos, count);
}

static struct snd_pcm_ops my_pcm_ops = {
        .open      = my_pcm_open,
        .close     = my_pcm_close,
        .ioctl     = snd_pcm_lib_ioctl,
        .hw_params = my_hw_params,
        .hw_free   = my_hw_free,
        .prepare   = my_pcm_prepare,
        .trigger   = my_pcm_trigger,
        .pointer   = my_pcm_pointer,
        .copy      = my_pcm_copy,
};

// static irqreturn_t consumer(int irq, void *p_dummy)
// {
// 	printk("inside interrupt handler\n");
	
// 	return IRQ_NONE;
// }


static irqreturn_t snd_mychip_interrupt(int irq, void *dev_id)
{
        struct mychip *chip = dev_id;
        printk(KERN_INFO "IN IRQ\n");
        return IRQ_NONE;
}




















/* hardware definition */
// static struct snd_pcm_hardware snd_mychip_capture_hw = {
//         .info = (SNDRV_PCM_INFO_MMAP |
//                  SNDRV_PCM_INFO_INTERLEAVED |
//                  SNDRV_PCM_INFO_BLOCK_TRANSFER |
//                  SNDRV_PCM_INFO_MMAP_VALID),
//         .formats =          SNDRV_PCM_FMTBIT_S16_LE,
//         .rates =            SNDRV_PCM_RATE_8000_48000,
//         .rate_min =         8000,
//         .rate_max =         48000,
//         .channels_min =     2,
//         .channels_max =     2,
//         .buffer_bytes_max = 32768,
//         .period_bytes_min = 4096,
//         .period_bytes_max = 32768,
//         .periods_min =      1,
//         .periods_max =      1024,
// };





/* open callback */
// static int snd_mychip_capture_open(struct snd_pcm_substream *substream)
// {
//         struct mychip *chip = snd_pcm_substream_chip(substream);
//         struct snd_pcm_runtime *runtime = substream->runtime;

//         runtime->hw = snd_mychip_capture_hw;
//         /* more hardware-initialization will be done here */
//         f=filp_open("/dev/ttyACM0",0666,O_RDWR);
//         if(f==NULL){
//         	printk(KERN_ALERT "File Open Err\n"); return 0;
//         }
//         else{
//         	printk(KERN_ALERT "File Opened\n");
//         	for (i = 0; i < 1000; ++i)
//         	{
//         		/* code */

//         	}}
        	

        
        
//         printk(KERN_INFO "my_pcm_open \n\n"); return 0;
        
// }

// /* close callback */
// static int snd_mychip_capture_close(struct snd_pcm_substream *substream)
// {
//         struct mychip *chip = snd_pcm_substream_chip(substream);
//         /* the hardware-specific codes will be here */
//         //filp_close(f,NULL);
        
//         printk(KERN_INFO "my_pcm_close \n\n"); return 0;

// }

// /* hw_params callback */
// static int snd_mychip_pcm_hw_params(struct snd_pcm_substream *substream,
//                              struct snd_pcm_hw_params *hw_params)
// {
// 		printk(KERN_INFO "my_pcm_hw_params \n\n");
//         return snd_pcm_lib_malloc_pages(substream,
//                                    params_buffer_bytes(hw_params));
// }

// /* hw_free callback */
// static int snd_mychip_pcm_hw_free(struct snd_pcm_substream *substream)
// {
//         printk(KERN_INFO "my_pcm_hw_free \n\n"); 
//         return snd_pcm_lib_free_pages(substream);
// }

// /* prepare callback */
// static int snd_mychip_pcm_prepare(struct snd_pcm_substream *substream)
// {
//         printk(KERN_INFO "my_pcm_prepare \n\n");
//         struct mychip *chip = snd_pcm_substream_chip(substream);
//         struct snd_pcm_runtime *runtime = substream->runtime;

//         /* set up the hardware with the current configuration
//          * for example...
//          */
//         // mychip_set_sample_format(chip, runtime->format);
//         // mychip_set_sample_rate(chip, runtime->rate);
//         // mychip_set_channels(chip, runtime->channels);
//         // mychip_set_dma_setup(chip, runtime->dma_addr,
//         //                      chip->buffer_size,
//         //                      chip->period_size);
//         return 0;
// }

// /* trigger callback */
// static int snd_mychip_pcm_trigger(struct snd_pcm_substream *substream,
//                                   int cmd)
// {
//         printk(KERN_INFO "my_pcm_trigger \n\n");
//         switch (cmd) {
//         case SNDRV_PCM_TRIGGER_START:
//                 /* do something to start the PCM engine */
//                 enable_irq(IRQ_NUM);

//                 break;
//         case SNDRV_PCM_TRIGGER_STOP:
//                 /* do something to stop the PCM engine */
//                 disable_irq(IRQ_NUM);

//                 break;
//         default:
//                 return -EINVAL;
//         }

//         return 0;
// }

// /* pointer callback */
// static snd_pcm_uframes_t
// snd_mychip_pcm_pointer(struct snd_pcm_substream *substream)
// {
//         printk(KERN_INFO "my_pcm_pointer \n\n");
//         struct mychip *chip = snd_pcm_substream_chip(substream);
//         unsigned int current_ptr; return 0;
//         /* get the current hardware pointer */
// //        current_ptr = mychip_get_hw_pointer(chip);
//         // return current_ptr;
// }

// int my_pcm_copy(struct snd_pcm_substream *substream, int channel, snd_pcm_uframes_t pos, void __user *dst, snd_pcm_uframes_t count)
// {
// 	int ret;
// 	printk(KERN_INFO "my_pcm_copy \n\n");
// 	//kernel_write(f,buf,1,0); 
// 	//kernel_read(f,0,buf1,48);
// 	struct my_device *my_dev = snd_pcm_substream_chip(substream);
// 	return copy_to_user(dst, "r2", 2);

// 	return 0;

// }

// /* operators */
// static struct snd_pcm_ops snd_mychip_capture_ops = {
//         .open =        snd_mychip_capture_open,
//         .close =       snd_mychip_capture_close,
//         .ioctl =       snd_pcm_lib_ioctl,
//         .hw_params =   snd_mychip_pcm_hw_params,
//         .hw_free =     snd_mychip_pcm_hw_free,
//         .prepare =     snd_mychip_pcm_prepare,
//         .trigger =     snd_mychip_pcm_trigger,
//         .pointer =     snd_mychip_pcm_pointer,
//         .copy = 	   my_pcm_copy,
// };

/*
 *  definitions of capture are omitted here...
 */

/* create a pcm device */
// static int snd_mychip_new_pcm(struct mychip *chip)
// {
//         struct snd_pcm *pcm;
//         int err;

//         err = snd_pcm_new(chip->card, "My Chip", 0, 1, 1, &pcm);
//         if (err < 0)
//                 return err;
//         pcm->private_data = chip;
//         strcpy(pcm->name, "My Chip");
//         chip->pcm = pcm;
//         /* set operators */

//         snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE,
//                         &snd_mychip_capture_ops);
//         /* pre-allocation of buffers */
//         /* NOTE: this may fail */
//         snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_CONTINUOUS, snd_dma_continuous_data(GFP_KERNEL), 48, 48);
//         return 0;
// }




























__init int firstmod_init(void)
{
	int i, ret;
	int sample_rate;
	f=NULL;
	sample_rate = 1400;
	
	dev_data = kzalloc(sizeof(int), GFP_KERNEL);

	printk(KERN_INFO "char_dev_dev_t \n\n");

	if((alloc_chrdev_region(&char_dev_dev_t, 0, 1, "char_dev_dev_t")) <0){
				printk(KERN_INFO "Cannot allocate major number for device char_dev_dev_t\n");
				return -1;
		}

	printk(KERN_INFO "<Major, Minor>: <%d, %d>\n", MAJOR(char_dev_dev_t), MINOR(char_dev_dev_t));

	printk(KERN_INFO "char_dev_2 \n\n");

	if((alloc_chrdev_region(&char_dev_2, 0, 1, "char_dev_2")) <0){
				printk(KERN_INFO "Cannot allocate major number for device char_dev_dev_t\n");
				return -1;
		}

	printk(KERN_INFO "<Major, Minor>: <%d, %d>\n", MAJOR(char_dev_2), MINOR(char_dev_2));

	//--------------------------------------------------------------------
	if((cls_X=class_create(THIS_MODULE, "CLASS_X"))==NULL)
	{
		unregister_chrdev_region(char_dev_dev_t,1);
		return -1;
	}

	mydevice = device_create(cls_X, NULL, char_dev_dev_t, NULL, "DEV_char_dev_dev_t");

	if(mydevice==NULL)
	{
		class_destroy(cls_X);
		unregister_chrdev_region(char_dev_dev_t,1);
		return -1;
	}




	if((cls_X2=class_create(THIS_MODULE, "CLASS_X2"))==NULL)
	{
		unregister_chrdev_region(char_dev_2,1);
		return -1;
	}

	mydevice2 = device_create(cls_X2, NULL, char_dev_2, NULL, "DEV_char_dev_2");

	if(mydevice2==NULL)
	{
		class_destroy(cls_X2);
		unregister_chrdev_region(char_dev_2,1);
		return -1;
	}

	//----------------------------------------------------------------

	printk(KERN_INFO "starting soundcard stuff\n\n");


	ret = snd_card_new(mydevice, -1, name, THIS_MODULE, 0, &card);
	if (ret < 0)
	        return ret;


			   
	 strcpy(card->driver, "my_driver");
	 strcpy(card->shortname, "MySoundCard Audio");

	snd_card_set_dev(card, mydevice2);




	ret = snd_device_new(card, SNDRV_DEV_LOWLEVEL, dev_data, &ops);
	if (ret < 0)
	        return ret;	

	
	    my_dev.card = card;


	//IRQ--------------------------------------------------------

	if(request_irq(IRQ_NUM, snd_mychip_interrupt, IRQF_SHARED, "sample", &dummy)<0)
			printk("interrupt installation failed\n");

	// if (request_irq(IRQ_NUM, snd_mychip_interrupt,
	//                 IRQF_SHARED, KBUILD_MODNAME, &dummy)) {
	//         printk(KERN_ERR "cannot grab irq %d\n", IRQ_NUM);
	//         snd_card_free(card);
	//         return -EBUSY;
	// }
	
		my_dev.irq = IRQ_NUM;
    //STARTING PCM STUFF---------------------------------------------



	printk(KERN_INFO "starting pcm stuff\n\n");

	ret = snd_pcm_new(card, card->driver, 0, 0, 1, &pcm);
	if (ret < 0)
	        return ret;

	

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &my_pcm_ops);
	pcm->private_data = &my_dev;
	my_dev.pcm = pcm;
	pcm->info_flags = 0;
	strcpy(pcm->name, card->shortname);

	ret = snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_CONTINUOUS, snd_dma_continuous_data(GFP_KERNEL), 48, 48);
	if (ret < 0)
	        return ret;







	if ((ret = snd_card_register(card)) < 0)
	        return ret;
	printk(KERN_INFO "soundcard registered\n\n");

	/*
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
		//for (i = 0; i < 1400; ++i)
		 	printk(KERN_INFO "%s",buf1[0]);
		 	printk(KERN_INFO "Sample Rate: %d \n", sample_rate*12);


	}
	filp_close(f,NULL);*/
	
	 
	
	
	return 0;
}


__exit void firstmod_exit(void)
{
	printk(KERN_ALERT "Bye FILERW module\n\n");
	snd_card_free(card);

	device_destroy(cls_X2,char_dev_2);
	class_destroy(cls_X2);
	unregister_chrdev_region(char_dev_2,1);

	device_destroy(cls_X,char_dev_dev_t);
	class_destroy(cls_X);
	unregister_chrdev_region(char_dev_dev_t,1);

    free_irq(IRQ_NUM, &dummy);

	return;
}

module_init(firstmod_init);
module_exit(firstmod_exit);

MODULE_DESCRIPTION("Soundcard, EEE G547");
MODULE_AUTHOR("Pratyush Bharati <bharati.pratyush@gmail.com>");
MODULE_LICENSE("GPL");

