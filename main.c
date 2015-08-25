#include "bootimg.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

//========I got that part from mkbootimg.c
//static unsigned base           = 0x10000000;
static unsigned kernel_offset  = 0x00008000;
static unsigned ramdisk_offset = 0x01000000;
//static unsigned second_offset  = 0x00f00000;
//static unsigned tags_offset    = 0x00000100;
//========Arguments Variable==============
static char *arg_file = NULL;
//========String Constant=================
static char *string_usage = "GetImageInfo [--kernel_offset <KernalOffset>] [--ramdisk_offset <RamdiskOffset>] <filename>\n";

void printUsage(void)
{
	fprintf(stderr, string_usage);
}

int handle_arg(int argc, char **argv)
{
	int result = -1;

	if(argc < 2)//if no parameter given
	{//too little arguments
		printUsage();
	}
	else
	{//if with enough arguments
		//the last argument is the image file name
		arg_file = argv[argc - 1];
		if(access(arg_file, R_OK) != 0)
		{//check the access of the file, if fail
			perror(NULL);
			printf("Fail to access the file %s\n", arg_file);
		}
		else
		{//if file is able to be access, take the rest of the arguments
			int count;
			char *opt_string;
			char *value_string;
			unsigned long value;

			result = 0;
			for(count = 1; count < argc - 2; count = count + 2)
			{
				opt_string = argv[count];
				value_string = argv[count + 1];
				if(!strcmp(opt_string, "--kernel_offset"))
				{//parameter for kernel offset
					value = strtol(value_string, NULL, 0);
					if((errno == ERANGE && value == ULONG_MAX) ||
							(errno != 0 && value == 0))
					{
						perror("strtol");
						result = -1;
						break;
					}
					else
					{
						kernel_offset = value;
					}
				}
				else if(!strcmp(opt_string, "--ramdisk_offset"))
				{//parameter for ramdisk offset
					value = strtol(value_string, NULL, 0);
					if((errno == ERANGE && value == ULONG_MAX) ||
							(errno != 0 && value == 0))
					{
						perror("strtol");
						result = -1;
						break;
					}
					else
					{
						ramdisk_offset = value;
					}
				}
				else
				{//undefined parameter
					printf("wrong parameter\n%s");
					printUsage();
					result = -1;
					break;
				}
			}
		}
	}

	return result;
}

//return 0 means error
//I assume the base address will not be 0
unsigned getBaseAddress(unsigned kernel_address, unsigned ramdisk_address)
{
	unsigned base_address = 0;

	if(kernel_address < ramdisk_address)
	{
		if(ramdisk_address - ramdisk_offset == kernel_address - kernel_offset)
		{
			base_address = kernel_address - kernel_offset;//ramdisk_address - ramdisk_offset;
		}
	}

	return base_address;
}

void printHeaderInfo(boot_img_hdr *pheader)
{
	printf("kernel size     = 0x%x\n",pheader->kernel_size);
	printf("kernel address  = 0x%x\n",pheader->kernel_addr);
	printf("kernel offset	= 0x%x\n\n",kernel_offset);

	printf("ramdisk size    = 0x%x\n",pheader->ramdisk_size);
	printf("ramdisk address = 0x%x\n",pheader->ramdisk_addr);
	printf("ramdisk offset	= 0x%x\n\n",ramdisk_offset);

	printf("second size     = 0x%x\n",pheader->second_size);
	printf("second address  = 0x%x\n\n",pheader->second_addr);

	printf("tag address     = 0x%x\n",pheader->tags_addr);
	printf("page size       = 0x%x\n\n",pheader->page_size);
}

//First parameter will be the path of the img file
int main(int argc, char **argv)
{
    int fd = 0;//the file description to open the image file
    int result = 0;
    boot_img_hdr header;
    unsigned base_address = 0;

    if(handle_arg(argc, argv) >= 0)
    {
    	fd = open(arg_file, O_RDONLY);//open image file
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
