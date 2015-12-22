#include "global.h"
#include "rf_type_cmd.h"
#include "internal_mem.h"
#include <mfs.h>

/**
*	�������ݹ��� TASK
*/
#pragma   pack(1)
typedef struct
{
   uint16_t 	YEAR;	/* 1970 - 2099 */
   uint16_t 	MONTH;	/* 1 - 12 */
   uint16_t 	DAY;		/* 1 - 31 (depending on month) */
   uint16_t 	HOUR;	/* 0 - 23 */
   uint16_t 	MINUTE;	/* 0 - 59 */
   uint16_t 	SECOND;	/* 0 - 59 */
}SAVE_TIME;

typedef struct 
{
	uint8_t  	type;
	uint32_t    Uid;
	uint8_t	  	dummy;
	SAVE_TIME 	save_time;
	uint16_t   	len;
	uint8_t    	data[51*22];
} SAVE_DATA_BUFFER;
#pragma   pack()

extern uint32_t uplk_send(char *buffer, uint16_t buflen, ...);
extern int32_t MFS_filesync(MQX_FILE_PTR fd_ptr);

void uplk_offline_task(uint32_t	parameter)
{
	TIME_STRUCT 	mqx_time;
	DATE_STRUCT 	mqx_date_time;
	
	uint32_t		len;
	uint32_t 		freesize;
	char 			logfilename[16];
	char 			path[32];
	MQX_FILE_PTR    fd = NULL;
	char 			rbuff[2048]={0};
	uint32_t		daynum=0;
	/* test offline data */
	uchar 					swbuf[900];
	uint16_t				vtemp;
	RF_SAMPLE_TYPE_PTR		cell_rfdata_ptr;
	cell_rfdata_ptr	= _mem_alloc_zero(sizeof(RF_SAMPLE_TYPE_t));
	
	SAVE_DATA_BUFFER		offline_package;
	offline_package.type 	= 0x14;
	offline_package.Uid		= htonl(CosmosInfoPtr->CosmosUid);
	offline_package.dummy	= 0x00;
	//offline_package.save_time
	offline_package.len		= 0x6204;
	_mem_zero(offline_package.data, 51*22);
	/* ������: ���˳������ڷ�����ɺ�block */
	
	_time_get(&mqx_time);
	/* �������������־�ļ��Ƿ������ */
	while(1){
		mqx_time.SECONDS = mqx_time.SECONDS - (86400U)*daynum++;
		/* ת������ʱ���ʽ */
		if (_time_to_date(&mqx_time, &mqx_date_time) == FALSE ) {
			printf("\n Cannot convert mqx_time to date format");
		}
		/* �����ļ�·�� */
		snprintf(logfilename, sizeof(logfilename), "%04d%02d%02d", mqx_date_time.YEAR, mqx_date_time.MONTH,mqx_date_time.DAY);	
		snprintf(path, sizeof(path), "%s%s", "c:/datalog/", logfilename);
		printf("\nCheck date:%.8s offline file", logfilename);
//		if (MFS_alloc_path(&path) != MFS_NO_ERROR) {
//		   printf("Error, unable to allocate memory for paths\n" );
//		} else {
//		   error = _io_rel2abs(path,shell_ptr->CURRENT_DIR,(char *) argv[1],PATHNAME_SIZE,shell_ptr->CURRENT_DEVICE_NAME);
//		   if(!error)
//		   {
//			  fd = fopen(abs_path, "r");
//		   }
//		   MFS_free_path(abs_path);		
//		   if (fd && !error)  {
//			  bytes = 0;
//			  if (fseek(fd, offset, seek_mode) != IO_ERROR)  {
//				 printf("Reading from %s:\n", argv[1]);
//				 while ((c=fgetc(fd))>=0) {
//					fputc(c, stdout);
//					if (++bytes == count) break;
//				 } 
//				 printf("\nDone.\n");
//				 fclose(fd);
//			  } else  {
//				 printf("Error, unable to seek file %s.\n", argv[1] );
//				 return_code = SHELL_EXIT_ERROR;
//			  }
//		   } else  {
//			  printf("Error, unable to open file %s.\n", argv[1] );
//			  return_code = SHELL_EXIT_ERROR;
//		   }
//		}
		/* ���ļ�����ʽΪ"r" */
		fd=fopen(path,"r");
		if (fd == NULL){
			printf("\nHaven't date:%.8s offline data", logfilename);
			printf("\nOffline data of All date uploaded completed");
			break;
		}
		/* ֻ�н��յ������źŲ���������ķ��� */

		/* ��ȡ��һ������ */
		while(fgets(rbuff,2048,fd) != NULL)
		{
			len = strlen(rbuff);
			rbuff[len-1] = '\0';  /*ȥ�����з�*/
			printf("\n%s %d ",rbuff,len - 1);

			/* ��¼����������ʱ�� */
			
			/* ������ת��Ϊ16���� */
			StrToHex(rbuff+10, swbuf);
			
			for (int i=0; i<swbuf[8]; i++)
			{
				offline_package.data[22*i] 	= 0x10;
				offline_package.data[1 + 22*i]= 0x16;
#if 1			/* �ɰ������Vin Vout�պ��෴ */
				/* ƴ����������Ϊ�ɰ汾���ݸ�ʽ */
				_mem_copy(swbuf + 9 + 16*i, cell_rfdata_ptr, sizeof(RF_SAMPLE_TYPE_t));
				vtemp = cell_rfdata_ptr->Vout;
				cell_rfdata_ptr->Vout = cell_rfdata_ptr->Vin;
				cell_rfdata_ptr->Vin =vtemp;
				_mem_copy((uchar *)cell_rfdata_ptr, offline_package.data + 22*i + 2, 16);
#else
				_mem_copy(buffer + 9 + 16*i, offline_package.data + 22*i + 2, 16);
#endif 
			}
			/* ���ͱ����������� */	
			uplk_send((char *)&offline_package, sizeof(SAVE_DATA_BUFFER));
			/* ɾ������---��ʱ���н��в��������ڷ��͹����ж��������ط����ļ� */
			
			/* ��ʱ100ms */
			_time_delay(100);
		}
		/* �����Ƿ񵽴��ļ�ĩβ */
		if(feof(fd)){
			printf("\nWe have reached endof file");
		}
		fclose(fd);
		/* �ļ�������ϣ���ɾ�����ļ� */
		ioctl(fd, IO_IOCTL_DELETE_FILE, (void *) path);
		//MFS_filesync(fd);
		_time_delay(1000*3);
	}
	_mem_free(cell_rfdata_ptr);
	/* ��ϵͳ���ͽ��������ϴ����ź� */
}










///* �����һ��������c�������£�������win-tc��Dev-c++���ѵ���ͨ���� */ 
//#include<stdio.h> 
//#include<stdlib.h> 
//#include<string.h> 
//#define N 255 /* �����²�����255�У��ɸ��� */ 
//
//int main() 
//{ 
//int i=0,max; 
//char s[N][81],*p="A.txt";/*���ļ�A.txtÿ�����²�����80�ַ����ɸ���*/ 
//FILE *fp; 
//if ((fp=fopen("A.txt","r"))==NULL) 
//{ printf("Open file %s error! Strike any key to exit.",p); 
//system("pause"); 
//exit(0); 
//} 
//while (i<N&&fgets(s[i],81,fp)!=NULL)/*���ж�ԭ����*/ 
//{ 
//p=strchr(s[i],'\n');/*����ÿ�����Ļس���*/ 
//if(p) 
//*p='\0'; 
//i++; 
//} 
//max=i;/*maxΪ����ʵ�ʵ��������*/ 
//fclose(fp); 
//fp=fopen("A.txt","w"); 
//for(i=0;i<max-2;i++) /*д���ļ����������Ļ��ʾ�����һ�к��������*/ 
//{ fprintf(fp,"%s\n",s[i]); 
//printf("%s\n",s[i]); 
//} 
//fprintf(fp,"%s",s[i]); 
//printf("%s\n",s[i]); 
//fclose(fp); 
//system("pause"); 
//return 0; 
//} 
//�ҷ������Ƶ�����ܶ�ģ�����Բ���: 
//http://zhidao.baidu.com/question/79123769.html 
//http://zhidao.baidu.com/question/80045856.html
//http://zhidao.baidu.com/question/80034032.html 

