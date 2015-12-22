#include "global.h"
#include <timer.h>
#include "gw_serial.h"
#include "kwtj_GWII.h"
#include "rf_scan_task.h"
#include "uplink_data_type.h"
#include "internal_mem.h"

extern void wifi_upgrade_cosmos(char *, uint32_t);
_timer_id  	active_rfscan_timer_id;

static void LED_on
   (
      _timer_id id,
      void   *data_ptr,
      MQX_TICK_STRUCT_PTR tick_ptr
   )
{
   //rf_led_blink();
   user_task_create("port_scan_task");
   //_task_create(0, SCAN_TASK, 0);
   //_timer_cancel(active_rfscan_timer_id);
}

_timer_id active_rfscan_timer(void)//0xfe 
{
    MQX_TICK_STRUCT     active_rfscan_ticks;
	
	_timer_cancel(active_rfscan_timer_id);
	_time_get_ticks(&active_rfscan_ticks);
	//_time_add_sec_to_ticks(&active_rfscan_ticks, ); //1S*60=1min
	_time_add_min_to_ticks(&active_rfscan_ticks, 5);
	return _timer_start_oneshot_at_ticks(LED_on, 0, TIMER_KERNEL_TIME_MODE, &active_rfscan_ticks);
}

static void uplk_passthrough(char *buffer, uint32_t buflen)//�����ȥ��֡ͷ
{
//	uchar p[512]; 
//	
//	buflen = buflen-6+2;
//	p[0] = 0x80|buffer[7];
//	p[1] = buffer[8];
//	p[2] = buffer[5];
//	_mem_copy(buffer+9, p+3, buflen-6);
//	
//	rflk_wirte(p, buflen-6);
//	task_isr_readrf(&p[2], 15);
	
	uchar *p = _mem_alloc_zero(buflen-6+2);

	p[0] = 0x80|buffer[7];
	p[1] = buffer[8];
	p[2] = buffer[5];
	_mem_copy(buffer+9, p+3, buflen-6);
	
	rflk_wirte(p, buflen-6);
	task_isr_readrf(&p[2], 45);
	_mem_free(p);
}

static void uplkuart_passthrough(char *buffer, uint32_t buflen)
{
	rflk_wirte((uchar *)buffer, buflen);
	task_isr_readrf((uchar *)&buffer[2], 15);
}

/****** �ú����пɼ���ȫ�ֶ��� ******
*	������Ҫ���У��ô�ֱ�ӿ�ʼ��������
*
*/
/******
*	������Ҫ��ʵʱ��������ò�������������
*	����ʵʱ����ǰ��Ҫ�ȷ�����ֹ�ڵ���ѯ����������ͺ��ֶ����ͽڵ���ѯ�����ʱ30�����Զ���ʼ��ѯ
*	������������ʱ����ʱ��ע���flash�������洢���ʻ��⴦��
*/
/*
* ʹ�ö��д������ݷŵ���һ��
*/
void uplink_recv_mix(char *buffer, uint32_t buflen, uint32_t socketID)
{
	/* Check zhe first byte of data frame */
	switch (buffer[0])
	{
		case	'@':
			while (_task_get_id_from_name("port_scan_task") != MQX_NULL_TASK_ID){
				user_task_abort("port_scan_task");
				_time_delay(5000);
			};
			if(_mutex_try_lock(&mutex_serial_rf) != MQX_EOK){
				printf("\næ");
				return;
			}
			active_rfscan_timer_id = active_rfscan_timer();
			buffer[buflen]='\0';
			download_cfgmix(buffer);
			
			_mutex_unlock(&mutex_serial_rf);
			break;
		case	0x20:	
			/* ȷ���������Ϊ��̨Cosmos��Check UID */
			if((buffer[1] == (uint8_t)(CosmosInfoPtr->CosmosUid>>16))&&\
				(buffer[2] == (uint8_t)(CosmosInfoPtr->CosmosUid>>8))&&\
					(buffer[3] == (uint8_t)CosmosInfoPtr->CosmosUid)) 
			{
				while (_task_get_id_from_name("port_scan_task") != MQX_NULL_TASK_ID){
					user_task_abort("port_scan_task");
					_time_delay(5000);
				};
				if(_mutex_try_lock(&mutex_serial_rf) != MQX_EOK){
					printf("\næ");
					return;
				}
				active_rfscan_timer_id = active_rfscan_timer();
			} else {
				printf("\nOld protocol Check UID error.");
				return;			
			}
			/* ���ݾɷ������·�ָ��͸�� */
			switch (buffer[UPLK_CMD_COMMON_Apn_OFFSET])
			{
				case	UPLK_CFG_10_FUNC:
					printf("\ncfg 10");
					cfg_10_save(buffer+6);
					break;
				case	UPLK_CFG_51_FUNC:
					printf("\ncfg 51");
					cfg_51_save(buffer+6);
					break;
				case	UPLK_CFG_66_FUNC:
					printf("\ncfg 66");
					cfg_66_save(buffer+6);
					break;
				case	UPLK_CFG_45_FUNC:
					printf("\ncfg 45");
					break;
				case	UPLK_RLTIME_43_FUNC:
					printf("\nrltime 43");
					break;
				case	UPLK_RLTIME_46_FUNC:
					printf("\nrltime 46");
					break;
				case	UPLK_RLTIME_04_FUNC:
					printf("\nrltime 04");
					break;
				case	UPLK_RLTIME_03_FUNC:
					printf("\nrltime 03");					
					break;	
				case	UPLK_RLTIME_41_FUNC:
					printf("\nrltime 41");					
					break;	
				case	UPLK_RLTIME_44_FUNC:
					printf("\nrltime 44");					
					break;
				case	UPLK_UPGRADE_05_FUNC:
					printf("\nupgrade 05");
					wifi_upgrade_cosmos(buffer, buflen);
					_mutex_unlock(&mutex_serial_rf);
					return;
					//����������Ĵ���
					break;
				case	UPLK_UPGRADE_31_FUNC:
					printf("\nupgrade 31");
					//wifi_upgrade_rf(buffer+ );
					break;					
				default:
					printf("\nunknown cmd");
					break;
			}
			/* ���ݾɷ������·�ָ��͸�� */
			uplk_passthrough(buffer, buflen);
			_mutex_unlock(&mutex_serial_rf);
			break;
		case	0x80:
			while (_task_get_id_from_name("port_scan_task") != MQX_NULL_TASK_ID){
				user_task_abort("port_scan_task");
				_time_delay(1500);
			};
			if(_mutex_try_lock(&mutex_serial_rf) != MQX_EOK){
				printf("\næ");
				return;
			}
			
			buflen = buflen-2;/* 2: CRC_LEN */
			uplkuart_passthrough(buffer, buflen);
			
			_mutex_unlock(&mutex_serial_rf);
			break;
		default:
			/* �����������ָ������ */
			printf("\nUnknow Cmd Head");
			break;
	}
	return;
}


