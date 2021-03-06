/**HEADER********************************************************************
* 
* Copyright (c) 2013 Freescale Semiconductor;
* All Rights Reserved                       
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: shell_task.c$
* $Version : 3.8.19.0$
* $Date    : Mar-13-2013$
*
* Comments:
*
*   This file contains the source for board tests program.
*
*END************************************************************************/

#include <mqx.h>
#include <shell.h>
#include "main.h"
#include <part_mgr.h>
#include <bsp.h> 
#include <mfs.h>
#include "sh_mfs.h"
#include <rtcs.h>
#include <ftpc.h> 

#define DEMOCFG_ENABLE_FTP_TEST 1
extern int32_t  Shell_test_bigfile(int32_t argc, char * argv[] );
extern int32_t  Shell_test_smallfiles(int32_t argc, char * argv[] );
extern int32_t  Shell_drivertest(int32_t argc, char * argv[] );
extern int32_t  Shell_test(int32_t argc, char * argv[] );
extern int32_t  Shell_spinor_erase_chip(int32_t argc, char * argv[] );
extern int32_t Shell_fs_open(int32_t argc, char * argv[] );
extern int32_t Shell_fs_close(int32_t argc, char * argv[] );
extern int32_t Shell_ipconfig(int32_t argc, char *argv[] );
extern int32_t Shell_ping(int32_t argc, char *argv[] ); 
extern int32_t Shell_TFTPd(int32_t argc, char *argv[] );
extern int32_t Shell_TFTP_client(int32_t argc, char *argv[] );
extern int32_t Shell_FTP_client(int32_t argc, char *argv[] );
extern int32_t Shell_ftpsrv(int32_t argc, char *argv[] );

const SHELL_COMMAND_STRUCT Shell_commands[] = {   
    { "cd",             Shell_cd },       
    { "copy",           Shell_copy },
    { "create",         Shell_create },
    { "del",            Shell_del },       
    { "disect",         Shell_disect},      
    { "dir",            Shell_dir },       
    { "di",             Shell_di   },          
    { "exit",           Shell_exit }, 
    { "format",         Shell_format },
    { "help",           Shell_help }, 
    { "mkdir",          Shell_mkdir },     
    { "pwd",            Shell_pwd },       
    { "read",           Shell_read },      
    { "ren",            Shell_rename },     
    { "rmdir",          Shell_rmdir },
    { "sh",             Shell_sh },
    { "type",           Shell_type },
    { "write",          Shell_write },      
    { "?",              Shell_command_list },  
    { "fsopen",         Shell_fs_open },     
    { "fsclose",        Shell_fs_close },     
    { "erasechip",      Shell_spinor_erase_chip },
    { "bigfile",        Shell_test_bigfile },
    { "smallfile",      Shell_test_smallfiles },
    { "drivertest",	Shell_drivertest },
    { "test",           Shell_test }, 
    { "ipconfig",       Shell_ipconfig },   
    #if RTCSCFG_ENABLE_ICMP      
    { "ping",           Shell_ping },      
    #endif
    #if DEMOCFG_ENABLE_FTP_TEST
    { "ftp",            Shell_FTP_client },
    { "ftpsrv",         Shell_ftpsrv },
    #endif
    #if RTCSCFG_ENABLE_UDP
    //#if DEMOCFG_ENABLE_TFTP_TEST
    { "tftp",           Shell_TFTP_client },
    { "tftpd",          Shell_TFTPd },
    //#endif
    #endif
    { NULL,             NULL } 
};

#if DEMOCFG_ENABLE_TELNET_TEST
const SHELL_COMMAND_STRUCT Telnet_commands[] = {
    { "exit",      Shell_exit },
    { "help",      Shell_help },
    { "ipconfig",  Shell_ipconfig },
    { "netstat",   Shell_netstat },
    { "pause",     Shell_pause },
#if RTCSCFG_ENABLE_ICMP
    { "ping",      Shell_ping },
#endif
#if DEMOCFG_ENABLE_TELNET_TEST
    { "telnet",    Shell_Telnet_client },
#endif
#if RTCSCFG_ENABLE_UDP
#if DEMOCFG_ENABLE_TFTP_TEST
    { "tftp",      Shell_TFTP_client },
#endif
#endif
    { "?",         Shell_command_list },
    { NULL,        NULL }
};

const SHELL_COMMAND_STRUCT Telnetd_shell_commands[] = {
    { "exit",      Shell_exit },
    { "help",      Shell_help },
    { "ipconfig",  Shell_ipconfig },
    { "netstat",   Shell_netstat },
    { "pause",     Shell_pause },
#if RTCSCFG_ENABLE_ICMP
    { "ping",      Shell_ping },
#endif
#if DEMOCFG_ENABLE_TELNET_TEST
    { "telnet",    Shell_Telnet_client },
#endif
#if RTCSCFG_ENABLE_UDP
#if DEMOCFG_ENABLE_TFTP_TEST
    { "tftp",      Shell_TFTP_client },
#endif
#endif
    { "walkrt",    Shell_walkroute },
    { "?",         Shell_command_list },
    { NULL,        NULL }
};
#endif

#if DEMOCFG_ENABLE_FTP_TEST
// ftp root dir
char FTPd_rootdir[] = {"a:\\"}; 

const FTPc_COMMAND_STRUCT FTPc_commands[] = {
   { "ascii",        FTPc_ascii       },
   { "binary",       FTPc_binary      },
   { "bye",          FTPc_bye         },
   { "cd",           FTPc_cd          },
   { "close",        FTPc_close       },
   { "delete",       FTPc_delete      },
   { "get",          FTPc_get         },
   { "help",         FTPc_help        },
   { "ls",           FTPc_ls          },
   { "mkdir",        FTPc_mkdir       },
   { "open",         FTPc_open        },
   { "pass",         FTPc_pass        },
   { "put",          FTPc_put         },
   { "pwd",          FTPc_pwd         },
   { "remotehelp",   FTPc_remotehelp  },
   { "rename",       FTPc_rename      },
   { "rmdir",        FTPc_rmdir       },
   { "user",         FTPc_user        },
   { NULL,   NULL }
};
#endif

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

void Shell_Task(uint32_t param)
{ 
    /* Run the shell on the serial port */
    for(;;)  {
//        _lwsem_wait(&TM_Sem);
//        TM_TaskStatus |= SHELL_TASK_STATUS_BIT;
//        _lwsem_post(&TM_Sem);
        Shell(Shell_commands, NULL);
    }
}

int32_t  Shell_ext_sram_test(int32_t argc, char * argv[] )
{
    bool  print_usage, shorthelp = FALSE;
    int32_t   return_code = SHELL_EXIT_SUCCESS;
    uint32_t para;
    
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );  
    
    if(!print_usage){
        if(argc == 1){
            para = 1;
        }else{
            Shell_parse_number(argv[1], &para);
        }
        
        _task_create(0, EXT_SRAM_TASK_ID, para);
    }
    
    if (print_usage)  {
        if (shorthelp)  {
            printf("%s <times> \n", argv[0]);
        } else  {
            printf("This command to varify the external sram.\n");
            printf("Usage: %s <times>, such as: %s 2\n", argv[0], argv[0]);
            printf("   <times> = how many times the external sram to be tested\n");
        }
    }
    
    return return_code;
}

int32_t  Shell_gpio_test(int32_t argc, char * argv[])
{
    bool  print_usage, shorthelp = FALSE;
    int32_t   return_code = SHELL_EXIT_SUCCESS;
    uint32_t para;
    
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );  
    
    if(!print_usage){
        if(argc == 1){
            para = 1;
        }else{
            Shell_parse_number(argv[1], &para);
        }
        
        _task_create(0, GPIO_TOGGLE_TASK_ID, para);
    }
    
    if (print_usage)  
    {
        if (shorthelp)  
        {
            printf("%s <times> \n", argv[0]);
        } 
        else  
        {
            printf("GPIO test.\n");
        }
    }
    
    return return_code;
}

int32_t  Shell_rs485_test(int32_t argc, char * argv[] )
{
    bool  print_usage, shorthelp = FALSE;
    int32_t   return_code = SHELL_EXIT_SUCCESS;
    uint32_t para;
    
    print_usage = Shell_check_help_request(argc, argv, &shorthelp );  
    
    if(!print_usage){
        if(argc == 1){
            para = 1;
        }else{
            Shell_parse_number(argv[1], &para);
        }
        
        if((para == 1) || (para == 3))
        {
            _task_create(0, RS485_WRITE_TASK_ID, para);
        }
        else if((para == 0) || (para == 2))
        {
            _task_create(0, RS485_READ_TASK_ID, para);
        }
        else
        {
            printf("Input parm error\n");
        }
    }
    
    if (print_usage)  
    {
        if (shorthelp)  
        {
            printf("%s <transmit/receive>\n", argv[0]);
        } 
        else  
        {
            printf("rs485 test\n");
            printf("Usage: %s <transmit/receive>, such as: %s 1\n", argv[0], argv[0]);
            printf("<transmit/receive>: 1 = transmit, 0 = receive\n");
        }
    }
    
    return return_code;
}

