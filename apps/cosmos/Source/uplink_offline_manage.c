#include "global.h"
#include "rf_type_cmd.h"
#include "internal_mem.h"
#include <mfs.h>

/**
*	�������ݹ��� TASK
*/
#pragma   pack(1)
//typedef struct
//{
//   uint16_t 	YEAR;	/* 1970 - 2099 */
//   uint16_t 	MONTH;	/* 1 - 12 */
//   uint16_t 	DAY;	/* 1 - 31 (depending on month) */
//   uint16_t 	HOUR;	/* 0 - 23 */
//   uint16_t 	MINUTE;	/* 0 - 59 */
//   uint16_t 	SECOND;	/* 0 - 59 */
//}SAVE_TIME;

typedef struct 
{
	uint8_t  	type;
	uint32_t    Uid;
	uint8_t	  	dummy;
	uint32_t 	save_time;
	uint16_t   	len;
	uint8_t    	data[51*22];
} SAVE_DATA_BUFFER;
#pragma   pack()

extern uint32_t uplk_send(char *buffer, uint16_t buflen, ...);
extern int32_t MFS_filesync(MQX_FILE_PTR fd_ptr);

void uplk_offline_task(uint32_t	parameter)
{
	TIME_STRUCT 	mqx_time;
	TIME_STRUCT 	save_time;
	DATE_STRUCT 	mqx_date_time;
	DATE_STRUCT 	save_date_time;
	
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
	offline_package.len		= htons(0x0462);
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

			/* ���������ʱ������ */
			save_date_time.YEAR		= mqx_date_time.YEAR;
			save_date_time.MONTH	= mqx_date_time.MONTH;
			save_date_time.DAY		= mqx_date_time.DAY;
			save_date_time.HOUR		= (rbuff[1]-'0')<<4 |(rbuff[2]-'0');
			save_date_time.MINUTE	= (rbuff[4]-'0')<<4 |(rbuff[5]-'0');
			save_date_time.SECOND	= (rbuff[7]-'0')<<4 |(rbuff[8]-'0');
			_time_from_date(&save_date_time, &save_time);
			
			offline_package.save_time = htonl(save_time.SECONDS);
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

