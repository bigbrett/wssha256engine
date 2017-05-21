/**
 * @file   testwssha256kern.c
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief  A Linux user space program that communicates with the wssha256kern.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/wssha256char.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "wssha.h"

#define SHA256_MSG_SIZE 256
#define SHA256_DGST_SIZE 32

static const char *devicefname = "/dev/wssha256char";

/*
 *
 */
int32_t sha256_init(void)
{
	printf("Checking for kernel module...\n");
	if( access( devicefname, F_OK ) != -1 ) 
	{
		printf("Found device!\n");
		return 0;
	} 
  else 
	{
		printf("Couldn't find device\n");
		return -1; 
	}
}


/*
 * 
 */
int32_t sha256(uint8_t *datap, uint64_t datalen,uint8_t *digestp, uint32_t *digest_lenp)
{
	int32_t ret, fd; 

	// initialize digest to all zeros
	memset((void*)digestp,0,SHA256_DGST_SIZE);

	// Open the device with read/write access
	fd = open("/dev/wssha256char", O_RDWR);             
	if (fd < 0){
		perror("sha256: Failed to open the device...");
		return errno;
	}

	// send the test vector to LKM
	ret = write(fd, datap, SHA256_MSG_SIZE);
	if (ret < 0){
		perror("sha256: Failed to write the message to the device.");
		return errno;
	}

	// TODO THIS SLEEP IS NECESSARY TO ENSURE THERE IS ENOUGH TIME FOR THE HW DEVICE TO COMPLETE.
	// IT SHOULD BE REPLACED WITH A READY CHECK, TO BE IMPLEMENTED IN THE DRIVER USING IOCTL
	sleep(1);

	// read back the response from the LKM and print
	ret = read(fd, digestp, SHA256_DGST_SIZE);        
	if (ret < 0){
		perror("sha256: Failed to read the message from the device.");
		return errno;
	}

	// close and exit
	if(close(fd)<0)
		perror("sha256: Error closing file");

	return 0;
}
