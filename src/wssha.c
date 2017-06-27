/**
 * @file   testwssha256kern.c
 * @author Brett Nicholas
 * @date   7 April 2015
 * @version 0.1
 * @brief  A Linux user space program that communicates with the wssha256kern.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/wssha256char.
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
 * Initialization function to check if device file exists 
 * and is read/writeable
 */
int32_t sha256_init(void)
{
	if( access( devicefname, F_OK ) == -1 ) {
		perror("Couldn't find device\n");
		return -1; 
	}
		return 1;
}


/*
 * main sha256 digest function 
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
	ret = write(fd, datap, SHA256_MSG_SIZE); // TODO why are we writing SHA256_MSG_SIZE? Shouldn't we be writing datalen? 
	if (ret < 0){
		perror("sha256: Failed to write the message to the device.");
		return errno;
	}

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
