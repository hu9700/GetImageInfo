#include "bootimg.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

//========I got that part from mkbootimg.c
//static unsigned base           = 0x10000000;
static unsigned kernel_offset  = 0x00008000;
static unsigned ramdisk_offset = 0x01000000;
//static unsigned second_offset  = 0x00f00000;
//static unsigned tags_offset    = 0x00000100;
//====================================

void printUsage(void)
{
	fprintf(stderr,"GetImageInfo <filename>\n");
}

int isArgOk(int argc, char **argv)
{
	int result = 1;

	if(argc != 2)
	{
		printUsage();
		result = 0;
	}
	else if(access(argv[1], R_OK) != 0)
	{
		perror("Fail to access the file");
		result = 0;
	}

	return result;
}

//return 0 means error
//I assume the base address will not be 0
unsigned getBaseAddress(unsigned temp_kernel_offset, unsigned temp_ramdisk_offset)
{
	unsigned base_address = 0;

	if(temp_kernel_offset < temp_ramdisk_offset)
	{
		if(temp_ramdisk_offset - temp_kernel_offset == ramdisk_offset - kernel_offset)
		{
			base_address = temp_ramdisk_offset - ramdisk_offset;
		}
	}

	return base_address;
}

void printHeaderInfo(boot_img_hdr *pheader)
{
	printf("kernel size     = 0x%x\n",pheader->kernel_size);
	printf("kernel address  = 0x%x\n",pheader->kernel_addr);

	printf("ramdisk size    = 0x%x\n",pheader->ramdisk_size);
	printf("ramdisk address = 0x%x\n",pheader->ramdisk_addr);

	printf("second size     = 0x%x\n",pheader->second_size);
	printf("second address  = 0x%x\n",pheader->second_addr);

	printf("tag address     = 0x%x\n",pheader->tags_addr);
	printf("page size       = 0x%x\n",pheader->page_size);
}

//First parameter will be the path of the img file
int main(int argc, char **argv)
{
    int fd = 0;//the file description to open the image file
    char *image_path = NULL;
    int result = 0;
    boot_img_hdr header;
    unsigned base_address = 0;

    if(isArgOk(argc, argv))
    {
    	image_path = argv[1];
    	fd = open(image_path, O_RDONLY);//open image file
    	if(fd < 0)
    	{
    		perror("Fail to open file");
    		result = -1;
    	}
    	else
    	{//succeed to open file
    		if(read(fd, &header, sizeof(boot_img_hdr)) != sizeof(boot_img_hdr))
    		{
    			perror("Fail to read file");
    			result = -1;
    		}
    		else
    		{//succeed to read file
    			base_address = getBaseAddress(header.kernel_addr, header.ramdisk_addr);

    			if(base_address == 0)
    			{//Header Information is wrong
    				fprintf(stderr,"Image Header is not in correct format\n");
    				result = -1;
    				printf("Error Header Information:\n");
    			}

    			//Print Header Information anyway
    			printHeaderInfo(&header);
    			if(base_address != 0)
    			{
    				printf("base address    = 0x%x\n", base_address);
    			}
    		}

    		//close file description
    		if(fd != 0)
    		{
    			close(fd);
    		}
    	}
    }
    else
    {
    	result = -1;
    }

    return result;
}
