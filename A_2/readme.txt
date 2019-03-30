1. Compile and insert the module dor.ko
2. Check for correct partitions using ------- sudo fdisk -l
3. To test read write functionality use commands
		
		sudo dd if=input.txt of=/dev/dor bs=512 count=1 seek=4 | hexdump -C ------ to write to the disk "Skyrim belongs to the Nords"
		sudo dd if=/dev/dor bs=512 count=16  | hexdump -C  					------ to read from disk the first 16 sectors

4. After done testing remove the module


AUTHOR 			: PRATYUSH BHARATI
ID				: 2016A8PS0313P
KERNEL VERSION 	: 4.10.0-28-generic
