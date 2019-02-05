/**
 * @file   testebbchar.c
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief  A Linux user space program that communicates with the ebbchar.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/ebbchar.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
*/
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_LENGTH 2               ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

void outit(char *receive, int choice)
{
	int len, i;
	uint32_t result;
		len = strlen(receive);
	for(i=0; i<len; i++){
		result = result * 20 + ( receive[i] - '0' );
	}
	printf("Value Read from axis (%d): %d \n",choice,((result*result)%1023));
}

int main(){
	int ret, fd;
	int choice;	
	char *p, s[100];
	int result;
	int len, i, flag;
	printf("Starting device test code example...\n");
	
             // Read in a string (with spaces)


  	flag = 1;
	
	do
	{
			printf("Type in the axis to read: 1.(X)  2.(Y) 3.(Z) 0.(Exit program)\n");
	   while (fgets(s, sizeof(s), stdin)) {
       	choice = strtol(s, &p, 10);
      if (p == s || *p != '\n') {
         	printf("Please enter an integer: ");
      	} else break;
    	} 

			switch(choice)

	{
	case 1:
	fd = open("/dev/DEV_ADXL_X", O_RDWR);             // Open the device with read/write access
	if (fd < 0){
		perror("Failed to open the device...");
		return errno;
	}
	ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
	if (ret < 0){
		perror("Failed to read the data from the device.");
		return errno;
	}

	outit(receive, choice);

	ret = close(fd);             // Close device
	if (ret < 0){
		perror("Failed to close the device...");
		return errno;
	}  break;

	case 2:
	fd = open("/dev/DEV_ADXL_Y", O_RDWR);             // Open the device with read/write access
	if (fd < 0){
		perror("Failed to open the device...");
		return errno;
	}
	ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
	if (ret < 0){
		perror("Failed to read the data from the device.");
		return errno;
	}

	outit(receive, choice);

	ret = close(fd);             // Open the device with read/write access
	if (ret < 0){
		perror("Failed to close the device...");
		return errno;
	} break;

	case 3:
	fd = open("/dev/DEV_ADXL_Z", O_RDWR);             // Open the device with read/write access
	if (fd < 0){
		perror("Failed to open the device...");
		return errno;
	}
	ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
	if (ret < 0){
		perror("Failed to read the data from the device.");
		return errno;
	}

	outit(receive, choice);	 

	ret = close(fd);             // Open the device with read/write access
	if (ret < 0){
		perror("Failed to close the device...");
		return errno;
	} break;

	case 0:
	flag=-1;
	printf("End of the program\n");
	return 0;
	break;

	default:
	printf("Incorrect Entry\n");
	break;
	}

	} while (flag>0); 

	printf("End of the program\n");
	return 0;
}


