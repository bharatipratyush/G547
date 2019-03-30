/* Disk on RAM Driver */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/errno.h>

#include "ram_device.h"

#define DOR_FIRST_MINOR 0
#define DOR_MINOR_CNT 16

static u_int dor_major = 0;

/* 
 * The internal structure representation of our Device
 */
static struct dor_device
{
	/* Size is the size of the device (in sectors) */
	unsigned int size;
	/* For exclusive access to our request queue */
	spinlock_t lock;
	/* Our request queue */
	struct request_queue *dor_queue;
	/* This is kernel's representation of an individual disk device */
	struct gendisk *dor_disk;
} dor_dev;

static int dor_open(struct block_device *bdev, fmode_t mode)
{
	unsigned unit = iminor(bdev->bd_inode);

	printk(KERN_INFO "dor: Device is opened\n");
	printk(KERN_INFO "dor: Inode number is %d\n", unit);

	if (unit > DOR_MINOR_CNT)
		return -ENODEV;
	return 0;
}

static void dor_close(struct gendisk *disk, fmode_t mode)
{
	printk(KERN_INFO "dor: Device is closed\n");
	return ;
}

/* 
 * Actual Data transfer
 */
static int dor_transfer(struct request *req)
{
	//struct dor_device *dev = (struct dor_device *)(req->rq_disk->private_data);
	sector_t start_sector = blk_rq_pos(req);
	unsigned int sector_cnt = blk_rq_sectors(req);
	struct bio_vec bvec;
	struct req_iterator iter;
	sector_t sector, sector_offset =0; 
	char *buffer;
	unsigned long offset=0;
	size_t len;
	int dir;
	


	int ret = 0;

	//printk(KERN_DEBUG "dor: Dir:%d; Sec:%lld; Cnt:%d\n", dir, start_sector, sector_cnt);

	sector_offset = 0;
	rq_for_each_segment(bvec, req, iter)
	{
		sector = iter.iter.bi_sector;
		buffer = kmap_atomic(bvec.bv_page);
		offset = bvec.bv_offset;
		len = bvec.bv_len/DOR_SECTOR_SIZE;
		dir = bio_data_dir(iter.bio);

		printk(KERN_DEBUG "dor: Sector Offset: %lld; Buffer: %p; Length: %d sectors\n",
			offset, buffer, sector_cnt);
		if (dir == WRITE) /* Write to the device */
		{
			ramdevice_write(start_sector + offset, buffer, len);
		}
		else /* Read from the device */
		{
			ramdevice_read(start_sector + offset, buffer, len);
		}
		sector_offset += len;
		kunmap_atomic(buffer);
	}
	if (sector_offset != sector_cnt)
	{
		printk(KERN_ERR "dor: bio info doesn't match with the request info");
		ret = -EIO;
	}

	return ret;
}
	
/*
 * Represents a block I/O request for us to execute
 */
static void dor_request(struct request_queue *q)
{
	struct request *req;
	int ret;

	/* Gets the current request from the dispatch queue */
	while ((req = blk_fetch_request(q)) != NULL)
	{

		ret = dor_transfer(req);
		__blk_end_request_all(req, ret);
		
	}
}


static struct block_device_operations dor_fops =
{
	.owner = THIS_MODULE,
	.open = dor_open,
	.release = dor_close,
};
	

static int __init dor_init(void)
{
	int ret;

	
	if ((ret = ramdevice_init()) < 0)
	{
		return ret;
	}
	dor_dev.size = ret;

	
	dor_major = register_blkdev(dor_major, "dor");
	if (dor_major <= 0)
	{
		printk(KERN_ERR "dor: Unable to get Major Number\n");
		ramdevice_cleanup();
		return -EBUSY;
	}

	
	spin_lock_init(&dor_dev.lock);
	dor_dev.dor_queue = blk_init_queue(dor_request, &dor_dev.lock);
	if (dor_dev.dor_queue == NULL)
	{
		printk(KERN_ERR "dor: blk_init_queue failure\n");
		unregister_blkdev(dor_major, "dor");
		ramdevice_cleanup();
		return -ENOMEM;
	}
	
	
	dor_dev.dor_disk = alloc_disk(DOR_MINOR_CNT);
	if (!dor_dev.dor_disk)
	{
		printk(KERN_ERR "dor: alloc_disk failure\n");
		blk_cleanup_queue(dor_dev.dor_queue);
		unregister_blkdev(dor_major, "dor");
		ramdevice_cleanup();
		return -ENOMEM;
	}

 	
	dor_dev.dor_disk->major = dor_major;
  	
	dor_dev.dor_disk->first_minor = DOR_FIRST_MINOR;
 	
	dor_dev.dor_disk->fops = &dor_fops;
 	
	dor_dev.dor_disk->private_data = &dor_dev;
	dor_dev.dor_disk->queue = dor_dev.dor_queue;
	
	sprintf(dor_dev.dor_disk->disk_name, "dor");
	
	set_capacity(dor_dev.dor_disk, dor_dev.size);

	
	add_disk(dor_dev.dor_disk);
	
	printk(KERN_INFO "dor: Ram Block driver initialised (%d sectors; %d bytes)\n",
		dor_dev.size, dor_dev.size * DOR_SECTOR_SIZE);

	return 0;
}

static void __exit dor_cleanup(void)
{
	del_gendisk(dor_dev.dor_disk);
	put_disk(dor_dev.dor_disk);
	blk_cleanup_queue(dor_dev.dor_queue);
	unregister_blkdev(dor_major, "dor");
	ramdevice_cleanup();
}

module_init(dor_init);
module_exit(dor_cleanup);


