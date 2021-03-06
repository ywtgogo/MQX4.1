/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: spinorshell.c$
* $Version : 4.0.0$
* $Date    : Mar-28-2013$ 
*
* Comments: Provide MFS file system on internal SPI-NOR Flash memory.
*
*END************************************************************************/


#include <mqx.h>  
#include <bsp.h>
#include <fio.h>
#include <mfs.h>

#include <shell.h>
#include <part_mgr.h>
#include "nftl.h"
#include "spiflash_config.h"
#include "spiflash_api.h"
#include "spinorshell.h"

#if ! SHELLCFG_USES_MFS
#error This application requires SHELLCFG_USES_MFS defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

extern NFTL_IO_INFO bsp_spiflash_info;

void shell_task(uint32_t);
extern int32_t MFS_filesync(MQX_FILE_PTR fd_ptr);
extern int32_t MFS_get_freesize(MQX_FILE_PTR fd_ptr);

extern int32_t Shell_test(int32_t argc, char *argv[]);
int32_t mfs_spiflash_open(void);
int32_t mfs_spiflash_close(void);

uint32_t filebuf[8*1024];	//8KB
_mqx_int error_code;

MQX_FILE_PTR  spinor_handle, filesystem_handle;//, partman_handle;//com_handle,
char filesystem_name[] = "a:";

int32_t deletfile(char* filepath) 
{
    MQX_FILE_PTR fs_ptr;
    
    fs_ptr = _io_get_fs_by_name(filesystem_name);
    if (fs_ptr == NULL)  
    {
        return SHELL_EXIT_ERROR;
    }
    else
    {
        if(ioctl(fs_ptr, IO_IOCTL_DELETE_FILE, filepath))
        {
            return SHELL_EXIT_ERROR;
        }
    }	
    
    return SHELL_EXIT_SUCCESS;
}


int32_t  Shell_nftl_test(int32_t argc, char_ptr argv[] )
{
    bool              print_usage, shorthelp = FALSE;
    int32_t               return_code = SHELL_EXIT_SUCCESS;
    //bool enable=TRUE;
    FILE*fd;
    
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );
    
    if (!print_usage)  {  
        if (argc != 1)  
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        } 
        else
        {
            //_io_mfs_ioctl(filesystem_handle,IO_IOCTL_VERIFY_WRITES,&enable);
            fd=fopen("a:\\xx.bin","w");
			if (fd == NULL)
			{
				return return_code;
			}
            if(7!=fwrite("abcdefg",1,7,fd))
                printf("write xx.bin error\n");
            else
                printf("write xx.bin ok\n");
            
            if(4!=fwrite("hijk",1,4,fd))
                printf("write xx.bin error\n");
            else
                printf("write xx.bin ok\n");	
            
            fclose(fd);
            MFS_filesync(fd);
        }
    }	
    
    return return_code;
}


/*TASK*-----------------------------------------------------------------
*
* Function Name  : mfs_spiflash_open
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t mfs_spiflash_open()
{
	uint64_t		big_number;
    if (spinor_handle == NULL)
    {
        /* Open spi-nor flash wl in normal mode */
        spinor_handle = fopen(NF_DEVICE, NULL); 
        if ( spinor_handle == NULL ) 
        {
            printf("Unable to open spinor flash device.\n");
            printf("\nYou must run Repair command\n");
            return -1;
        }
        
        /* Install MFS over spinor_wl driver */
        error_code = _io_mfs_install(spinor_handle, filesystem_name, (_file_size)0);
        if (error_code != MFS_NO_ERROR) 
        {
            printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
            _task_block();
        }
        
        if (filesystem_handle == NULL)
        {
            /* Open file system */
            filesystem_handle = fopen(filesystem_name, NULL); 
            error_code = ferror (filesystem_handle);
            if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK))
            {
                printf("Error opening filesystem: %s(0x%x)\n", MFS_Error_text((uint32_t)error_code),error_code);
                _task_block();
            }
            
            if ( error_code == MFS_NOT_A_DOS_DISK ) 
            {
                printf("\nNOT A DOS DISK! You must format to continue.\n");
				printf("Formating...\n");
				error_code = ioctl(filesystem_handle, IO_IOCTL_DEFAULT_FORMAT,  NULL);
				if (!error_code) {
					ioctl(filesystem_handle, IO_IOCTL_FREE_SPACE, &big_number);
					big_number>>=10; 
		   			printf("Done. free disk space: %lu kB\n", (uint32_t)big_number);
					
					//error_code = ioctl(filesystem_handle, IO_IOCTL_CREATE_SUBDIR, (void *) "\\datalog");
					error_code = ioctl(filesystem_handle, IO_IOCTL_CREATE_SUBDIR, (void *) "\\spinor");
					if (error_code)  {
						printf("Error creating directory datalog\n");
					}else
						return -1;						
				} 								
                //return -1;                 
            }   
            
            printf ("\nSPI-NOR flash installed to %s", filesystem_name);
        }
        else
        {
            printf("\nSPI-NOR flash was installed.\n");
        }     
    }
    else
    {
        printf("\nSPI-NOR flash device was opened.\n");
    } 
    
    return 0;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : mfs_spiflash_close
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t mfs_spiflash_close()
{
    if (filesystem_handle != NULL)
    {
        /* Close the filesystem */
        if (MQX_OK != fclose (filesystem_handle))
        {
            printf("Error closing filesystem.\n");
            _task_block();
        }
        filesystem_handle = NULL;
    }
    
    if (spinor_handle != NULL)
    {
        /* Uninstall MFS  */
        error_code = _io_dev_uninstall(filesystem_name);
        if (error_code != MFS_NO_ERROR) 
        {
            printf("Error uninstalling filesystem.\n");
            _task_block();
        } 
        
        /* Close the spinor flash device */
        if (MQX_OK != fclose (spinor_handle)) 
        {
            printf("Unable to close spinor device.\n");
            _task_block();
        }
        spinor_handle = NULL;
        
        printf ("spinor uninstalled.\n");
    }
    
    return 0;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : shell_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void shell_task(uint32_t temp)
{ 
    /* Run the shell on the serial4 port */    
    for(;;) 
    {
        Shell(Shell_commands, NULL);
        printf("Shell exited, restarting...\n");
    }
}

/*FUNCTION*------------------------------------------------------------
*
* Function Name  : Shell_fs_open
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t nftl_init()
{
	if (nftl_install(&bsp_spiflash_info, NF_DEVICE) != MQX_OK)
    {
        printf("Can't install NFTL!\n");
        _task_block();
    }

	mfs_spiflash_open(); 
	
	return 0;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_fs_open
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t  Shell_fs_open(int32_t argc, char_ptr argv[] )
{
    bool              print_usage, shorthelp = FALSE;
    int32_t               return_code = SHELL_EXIT_SUCCESS;
    
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );
    
    if (!print_usage)  {
        if (argc != 1)  
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        } 
        else
        {
            mfs_spiflash_open(); 
        }
    }
    
    if (print_usage)  {
        if (shorthelp)  {
            printf("%s \n", argv[0]);
        } else  {
            printf("Usage: %s\n", argv[0]);
            printf ("   Open the Flash file system\n");
        }
    }
    return return_code;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_fs_close
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/
int32_t  Shell_fs_close(int32_t argc, char_ptr argv[] )
{
    bool              print_usage, shorthelp = FALSE;
    int32_t               return_code = SHELL_EXIT_SUCCESS;
    
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );
    
    if (!print_usage)  {
        if (argc != 1)  
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        } 
        else
        {
            mfs_spiflash_close();
        }
    }
    
    if (print_usage)  {
        if (shorthelp)  {
            printf("%s \n", argv[0]);
        } else  {
            printf("Usage: %s\n", argv[0]);
            printf ("   Close the Flash file system\n");
        }
    }
    return return_code;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_spinor_erase_chip
* Returned Value : int32_t
* Comments       :
*
*END------------------------------------------------------------------*/

//uint32_t spiflash_erasechip(NFTL_DEV_INFO_PTR flash_ptr);

int32_t  Shell_spinor_erase_chip(int32_t argc, char_ptr argv[] )
{
    bool     print_usage, shorthelp = FALSE;
    int32_t      return_code = SHELL_EXIT_SUCCESS;
    
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );
    
    if (!print_usage)  {
        if (argc != 1)  
        {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage=TRUE;
        } 
        else
        {
          //  spiflash_erasechip(NULL);
          spinor_handle = fopen(NF_DEVICE, NULL);

            if (FLASH_OK != ioctl (spinor_handle, NFTL_IOCTL_ERASE_CHIP, NULL))
            {
                printf("Erase chip fail.\n");
            }
            if (spinor_handle != NULL)
            {
                
                if (MQX_OK != fclose (spinor_handle))
                {
                    printf("Error closing spinor_handle.\n");
                }
                spinor_handle = NULL;
            }
        }
    }
    
    if (print_usage)  {
        if (shorthelp)  {
            printf("%s \n", argv[0]);
        } else  {
            printf("Usage: %s\n", argv[0]);
            printf("   Erase SPI-NOR chip completely\n");
        }
    }
    return return_code;
    
}

int32_t  Shell_test_bigfile(int32_t argc, char_ptr argv[] )
{
    bool     print_usage, shorthelp = FALSE;
    uint32_t i,j,cnt,count;
    uint32_t freesize;
    FILE* fd;
    
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );
    if(print_usage) {
        if(shorthelp) {
            printf("%s \n", argv[0]);
        }else{
            printf("Usage: %s\n", argv[0]);
            printf("   Test big file read & write in SPI-NORFLASH\n");
        }
  	
        return SHELL_EXIT_SUCCESS;
    }
    
    for(i=0;i<8*1024;i++)
        filebuf[i]=i;	
    
    count = 70;//6000;
    
    printf("1. write bigfile.bin, Please wait...");
    fd=fopen("a:\\bigfile.bin","w");
    if(fd==NULL)
    {
        printf("\n   Open bigfile.bin error\n");
        return SHELL_EXIT_ERROR;
    }	
    
   
    for(i=0;i<count;i++)	//76800Byte
    {
        freesize = MFS_get_freesize(fd);
        if(freesize<4*128)
        {
          printf("\n  No sufficient disk space\n");
          fclose(fd);
          MFS_filesync(fd);
          return SHELL_EXIT_ERROR;
        }
      
        cnt=fwrite(filebuf,4,128,fd);
        if(cnt!=128)
        {
            printf("\n   write data error in block%d,only %d byte is done\n",i,cnt);
            fclose(fd);
            MFS_filesync(fd);	
            return SHELL_EXIT_ERROR;
        }
        //printf(".");
    }
    fclose(fd);
    MFS_filesync(fd);	
    printf("Done!!!\n");		
     
    printf("2. read bigfile.bin, please wait...");
    fd=fopen("a:\\bigfile.bin","r");
    if(fd==NULL)
    {
        printf("   Open bigfile.bin error\n");
        return SHELL_EXIT_ERROR;
    }
    
    for(i=0;i<count;i++)
    {
        cnt=fread(filebuf,4,128,fd);
        if(cnt!=128)
        {
            printf("\n   read data error, only %d byte is done\n",cnt);
            fclose(fd);
            return SHELL_EXIT_ERROR;
        } 
        
        for(j=0;j<128;j++)
        {  
            if(filebuf[j]!=j)
            {
                printf("\n   Verify error in Page%d: read=0x%x,exp=0x%x)\n",i,filebuf[j],j);
                fclose(fd);
                return SHELL_EXIT_ERROR;
            }
        }
        printf(".");
    }	
    
    fclose(fd);	
    printf("Done!!!\n");
/*    
    printf("3. delete bigfile.bin...");
    deletfile("bigfile.bin");
    printf("Done!!!\n");
    
    printf("big file read & write testing is SUCCESSFUL!!!!!!!!\n\n");	
 */   
    return SHELL_EXIT_SUCCESS;
}

int32_t  Shell_test_smallfiles(int32_t argc, char_ptr argv[] )
{
    bool     print_usage, shorthelp = FALSE;
    uint32_t i,cnt,files;
    FILE* fd;
    char filename[20];
    uint32_t freesize;
    
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );
    if(print_usage) {
  	if(shorthelp) {
            printf("%s \n", argv[0]);
  	}else{
            printf("Usage: %s\n", argv[0]);
            printf("   Test multiple small files read & write in SPI-NORFLASH\n");
  	}
  	
  	return SHELL_EXIT_SUCCESS;
    }
    
    for(i=0;i<8*1024;i++)
        filebuf[i]=i;	
    
    for(files=0;files<40;files++)
    {
        sprintf(filename,"a:\\sfile%d.bin",files);
        printf("%s is writing...",filename);
        
        fd=fopen(filename,"w");
        if(fd==NULL)
        {
            printf("\n   Open %s error\n",filename);
            return SHELL_EXIT_ERROR;
        }	
        
        freesize = MFS_get_freesize(fd);
        if(freesize<4*1234) 
        {
          printf("\n  No sufficient disk space\n");
          fclose(fd);
          MFS_filesync(fd);
          return SHELL_EXIT_ERROR;
        }
       
        cnt=fwrite(filebuf,4,1234,fd);
        if(cnt!=1234)
        {
            printf("\n   write error\n");
            fclose(fd);
            return SHELL_EXIT_ERROR;
        }
        fclose(fd);
        printf("Done!!!\n");	
    }
    
    for(files=0;files<40;files++)
    {
        sprintf(filename,"a:\\sfile%d.bin",files);		
        printf("%s is reading...",filename);
        fd=fopen(filename,"r");
        if(fd==NULL)
        {
            printf("   Open %s error\n",filename);
            return SHELL_EXIT_ERROR;
        }
        
        cnt=fread(filebuf,4,1234,fd);
        if(cnt!=1234)
        {
            printf("\n   read error\n");
            fclose(fd);
            return SHELL_EXIT_ERROR;
        } 
        
        for(i=0;i<1234;i++)
        {  
            if(filebuf[i]!=i)
            {
                printf("\n   data verification error(read=%d,exp=%d)\n",filebuf[i],i);
                fclose(fd);
                return SHELL_EXIT_ERROR;
            }
        }
        fclose(fd);	
        printf("Done!!!\n");
        
    }	
    
    for(files=0;files<40;files++)
    {
        sprintf(filename,"a:\\sfile%d.bin",files);		
        printf("deleting file %s...",filename);
        deletfile(filename);
        printf("Done!!!\n");
    }
    
    printf("small files read & write testing is SUCCESSFUL!!!!!!!!\n\n");
    
    return SHELL_EXIT_SUCCESS;
}


int32_t  Shell_drivertest(int32_t argc, char_ptr argv[] )
{
    bool     print_usage, shorthelp = FALSE;
    uint32_t did;
    uint32_t address;
    uint32_t i,j,k;
    char *testbuf = (char*) filebuf;
    char readbuf[516];
    NFTL_DEV_INFO nf_dev;
    
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );
    if(print_usage) {
        if(shorthelp) {
            printf("%s \n", argv[0]);
        }else{
            printf("Usage: %s\n", argv[0]);
            printf("   SPI-NOR flash driver test\n");
        }
  	
        return SHELL_EXIT_SUCCESS;
    }	
    
    memset(&nf_dev,0, sizeof(nf_dev));
    _lwsem_create(&nf_dev.lwsem, 1);
    nf_dev.block_shift_number = FLASH_BLOCK_SHIFT_NUMBER;
    
    if(spiflash_init(&nf_dev)!=FLASH_OK)
    {
        printf("Error in init spinorflash\n");
        return SHELL_EXIT_ERROR;
    }
    
    if(spiflash_readid(&nf_dev,&did)!=FLASH_OK)
    {
        printf("Error in read device id\r\n");
        return SHELL_EXIT_ERROR;
    }
    
    printf("device id=0x%x\n",did);
  		
	printf("erase block    ");    
    for(i=0;i<FLASH_BLOCKCOUNT;i++)
    {
        if(spiflash_eraseblock(&nf_dev,i)!=FLASH_OK)
        {
            printf("\n error in erase block%d\n",i);
            return SHELL_EXIT_ERROR;
        }  	
        printf("\033[%dD%03d", (3), i );        //debug: 速度慢，会打印一个小点后，程序死在某个地方
    }

    _lwsem_destroy(&nf_dev.lwsem);
    printf("Done\n");
    printf("Driver testing is SUCCESSFUL!!!!!!!!\n\n");
   
    return SHELL_EXIT_SUCCESS;
}
