#include "global.h"
#include <string.h>
#include <ipcfg.h>
#include "uplink_data_type.h"
#include "kwtj_GWII.h"

#include "internal_mem.h"

#define SOCKET_COUNT	3
SOCKET_CONNECT_t socket_info[SOCKET_COUNT];

extern uint32_t wifilk_wirte(uchar *wbuf, uint16_t len) ;

static bool sntp_get_time()
{
    uint32_t		sntp_addr;

	if (TRUE != RTCS_resolve_ip_address("time.nist.gov", &sntp_addr, NULL, 0)){//time.nist.gov
	    //_rtc_sync_with_mqx(FALSE);
        printf("\nFailed in getting sntp_addr in DNS");
		return FALSE;
    }else {
		printf("\nGetting time from time server time.nist.gov");	
        //printf("\nGetting time from time server [%d.%d.%d.%d]", IPBYTES(sntp_addr));		
		//SNTP_init("SNTP client", 17, 1000, sntp_addr, 5); 	
        if (RTCS_OK != SNTP_oneshot(sntp_addr, 5000)) {
            //_rtc_sync_with_mqx(FALSE);
			printf(" failed");
			return FALSE;
        }
    }
	return TRUE;
}

uint32_t systime_get(void)
{
	bool     	result;
	
#if 0  /* ENABLE SNTP Client task */
	result = SNTP_init("SNTP", 18, 512, IPADDR(61, 129, 42, 44), 2); 			//�����ַ��λ����Ҫ��̬��ȡ�����е�ser��ַ
	_time_delay(1000*5);	
#endif
	if (check_network()){
		result = sntp_get_time();
	}else{
		result = FALSE;
	}

	_rtc_sync_with_mqx(result);

	print_rtc_time();
	print_sys_time();
	return 0;
}

void rtctime_set(uint32_t timedata)
{
	_rtc_set_time(timedata);
	_time_delay(50);
	_rtc_sync_with_mqx(FALSE);//�˲�������systimeget�����
}

/*
*	����ע�����������
*	{0x20,|0x20,0x00,0x01,0x1c,|0x00,0x00,|0x01,0x08,0x21,|0x00,0x00};//03 08 06 05 28 �汾��+����ʱ��
*/
bool uplk_send_connreg(void)
{
	CONNREG_TYPE_t		connreg;

	connreg.Head	= 0x20;
	connreg.Uid		= htonl(CosmosInfoPtr->CosmosUid);
	connreg.VerNo	= htonl(VERNO);
	//connreg.VerTime	= ;
	uplk_send((char*)&connreg, sizeof(CONNREG_TYPE_t), 0);	

	return TRUE;
}

/*	
���ͽӿڣ���Ҫ���ļ�����TCP������֤�أ��Ƿ�Ҫ���ٷ������ݶ��У�log�ͷ��Ͷ����øö���
�����յ�һ���ͽ�����֤����ô����ÿ��tcp���͵�״̬��
UplinkSta uplk_send_tcp 
*
������socket�ķ��ͣ�У��������ӣ�����error log��event���ͼ�¼Ҫ���͵����ݣ�
event���ݸ��������ӵ�socket������������socket������λsocket��flag
*/
uint32_t uplk_send(char *buffer, uint16_t buflen, ...)
{
	uint32_t count;
	uint32_t i;
	UplinkSta result;
	
	
	if (!(CosmosInfoPtr->FuncEn&FUNC_EN_ETH)) {
		count = wifilk_wirte((uchar *)buffer, buflen);
		return count;
	}
	
	print_sys_time();
	printf("\n NETSEND: ");
	for (int i=0 ;  i<buflen; i++ ) {
	   printf("%.2x ", buffer[i]);
	}
	
	i = CosmosTcpPtr->CellCount;
	while(i--)
	{	
		if (socket_info[i].Status != SOCKET_OK)
		{
			continue;				
		}
		
		count = send(socket_info[i].ClientSock, buffer, buflen, 0);	
		if (count == RTCS_ERROR){ //RTCS_ERROR
			printf("\nError, send() failed with error code %lx, Occur: %s",RTCS_geterror(socket_info[i].ClientSock), socket_info[i].Domian);
			//�������жϣ������쳣��socket������λsocket flag��
				
			socket_info[i].Status = SOCKET_SEND_ERR;
			//shutdown(socket_info[i].ClientSock, FLAG_ABORT_CONNECTION);
		}
		_time_delay(100);
	}

	return count;
}

void uplk_read_task(uint32_t	parameter)
{
	uint32_t count;	//32767 BYTE
	uint32_t i=0,j;
	char	 uplkbuff[UPRX_BUFFER_SIZE];

    while(1)
    {
		/* check ipcfg status DNS status */

		/* read socket every */
		i = CosmosTcpPtr->CellCount;
		while(i--)
		{
			/* Let the other task run */			
			_time_delay(100);
			if (socket_info[i].Status != SOCKET_OK){
				continue;				
			}
			count = recv(socket_info[i].ClientSock, uplkbuff, UPRX_BUFFER_SIZE, 0);
			if (count != RTCS_ERROR) {
				if (count == 0) continue;
				printf("\n ");	
				print_sys_time();
				printf("\n FROM: %s", socket_info[i].Domian);	
				printf("\n NETRECV: ");	
				if (uplkbuff[0] == '@'){
					uplkbuff[count] = '\0';
					printf("%s", uplkbuff);
				}else{
					for (j=0 ;  j<count; j++ ) {
					   printf("%.2x ", uplkbuff[j]);
					}
				}
				if (count == 8) {
					printf("\nHeartbeat Packet\n");
					/* ���������ص�Ser�� */
					continue;
				}

				uplink_recv_mix(uplkbuff, count, i);//socket_id		
				continue;
			}else if (count == RTCS_ERROR) {
				printf("\nError, recv() failed with error code %lx, Occur: %s",RTCS_geterror(socket_info[i].ClientSock), socket_info[i].Domian);
				socket_info[i].Status = SOCKET_RECV_ERR;
				shutdown(socket_info[i].ClientSock, FLAG_ABORT_CONNECTION);
			}
			/* reconnect socket */
			_time_delay(500);
			if (socket_info[i].Status != SOCKET_OK){
				printf("\n%d -> %d", i, socket_info[i].Status);
				/* �˴�Ӧ��Ϊ�����źţ�����tcp���� */
			}
		}
    }
}






//���õ��ļ�����ȫ��sem/event/msg,���ڴ�������ipcfg��״̬
//init�еõ�ÿ����ʼ�����̵ķ���ֵ����������Ϊipcfg��״ֵ̬
//����������Ϊ��������

///*
//*	socket���Ӻ���
//*	����_ip_address��port
//*   return ����/���
//*/
//uint32_t connect_ser(_ip_address ser_ipadder, uint16_t	ser_port)
//{
//	uint32_t client_sock;
//	uint32_t error, is_error;
//	sockaddr_in server_addr;
//	uint32_t option;
//	uint32_t result;
//	uint32_t		sys_time;
//	TIME_STRUCT		mqx_time;
////	IPCFG_IP_ADDRESS_DATA gw_ipaddr;
//	char 	buffer[] = {0x20, 0x20,0x00,0x01,0x1c,0x00,0x00,0x01,0x08,0x21,0x00,0x00};//03 08 06 05 28
//	memset((char *) &server_addr, 0, sizeof(sockaddr_in));
//	client_sock = socket(PF_INET, SOCK_STREAM, 0);
//	if (client_sock == RTCS_SOCKET_ERROR) {
//		shutdown(client_sock, FLAG_ABORT_CONNECTION);
//		return RTCS_ERROR;
//	}
//#if 1	//����eth����
////	option = 2048;   
////	is_error = setsockopt(clinet_primarysock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
////	option = SOCKTSRVCFG_CONNECT_TIMEOUT;
////	is_error = setsockopt(clinet_primarysock, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));
////	option = SOCKTSRVCFG_TIMEWAIT_TIMEOUT;
////	is_error = setsockopt(clinet_primarysock, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));
//
//	option = 6000UL;	//6s���ӳ�ʱ
//	is_error = setsockopt(client_sock, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));	
//	option = 3000;
//	is_error = setsockopt(client_sock, SOL_TCP, OPT_MAXRTO, &option, sizeof(option));
//	option = FALSE;
//	is_error = setsockopt(client_sock, SOL_TCP, OPT_SEND_NOWAIT, &option, sizeof(option));	
//	option = 50;
//	is_error = setsockopt(client_sock, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));			
//	option = TRUE;
//	is_error = setsockopt(client_sock, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option)); //���ȴ���������������
//	option = TRUE;
//	is_error = setsockopt(client_sock, SOL_TCP, OPT_RECEIVE_PUSH, &option, sizeof(option));	//��������	
//	
//	option = SOCKTSRVCFG_RECEIVE_TIMEOUT;
//	is_error = setsockopt(client_sock, SOL_TCP, OPT_RECEIVE_TIMEOUT, &option, sizeof(option));	
//    if (is_error)
//	{
//		return RTCS_ERROR;
//	}	
////	ipcfg_get_ip(0, &gw_ipaddr);	
//#endif
//	server_addr.sin_family = AF_INET;
//	server_addr.sin_port = ser_port;
//	server_addr.sin_addr.s_addr = ser_ipadder;
//	
//	printf("\nconnecting to data center: %d.%d.%d.%d ", IPBYTES(server_addr.sin_addr.s_addr));
//	printf("\nport:%d ", server_addr.sin_port);\
//	result = connect(client_sock, &server_addr, sizeof(server_addr));
//	if (result != RTCS_OK)
//	{
////		shutdown(clinetsock, FLAG_ABORT_CONNECTION);
////		if (RTCS_OK != connect(clinetsock, &server_addr, sizeof(server_addr)))
////		{
//			printf(" failed");
//			printf("\nError--connect() failed with error code %lx.", result);
//			print_sys_time();
//			shutdown(client_sock, FLAG_ABORT_CONNECTION);
//			return RTCS_ERROR;
////		}
//	}	
//	printf(" connected");
//	send(client_sock, buffer, sizeof(buffer), 0);
//	
//	return client_sock;
//}


//UplinkSta eth_socket(uint8_t count, uchar cfginfo)
//{
//	uint32_t	ipaddr;
//	UplinkSta	result;
//	uint8_t		i;
//	char domain[3][32]= {"192.168.8.78", "ximeikj.xicp.net", "10.0.3.1"};//teletraan.convertergy.com
//	
//	for (i=0 ;i<count; i++)
//	{		
//		_mem_copy(domain[i], socket_info[i].Domian, strlen(domain[i]));
//		socket_info[i].IpPort = 10000;
////		socket_info[i].Status = SOCKET_OK;
//		/* ��֤���ӵ���Ч�� */
//		if (socket_info[i].Status != SOCKET_OK)
//		{	
//			_time_delay(100);
//			if (TRUE != RTCS_resolve_ip_address(socket_info[i].Domian, &ipaddr, NULL, 0)) {
//				//ipaddr = socket_info[i].IpAddr;	
//				printf("\n�������ݷ�����������ʱ %s", socket_info[i].Domian);
//				socket_info[i].Status = SOCKET_DNS_ERR;
//				result = DNS_ERR;
//				continue;
//			}
//			//socket_info[i].Status = connect_ser(socket_info[i].ClientSock, ipaddr, socket_info[i].IpPort);
//			socket_info[i].ClientSock = connect_ser(ipaddr, socket_info[i].IpPort);
//			if (socket_info[i].ClientSock == RTCS_ERROR){
//				//printf("\nError--connect() failed with error code %lx.", socket_info[i].ClientSock);
//				socket_info[i].Status = SOCKET_TIMEOUT;
//				result = CONNECT_ERR;
//			}else{
//				socket_info[i].Status = SOCKET_OK;
//				result = UPLINK_OK;
//			}
//		}else{
//			socket_info[i].Status = SOCKET_OK;
//			result = UPLINK_OK;
//		}
//	}
//	
//	return result;
//}

//UplinkSta eth_init(void)
//{
//	uint32_t     			error;
//	_enet_address   		enet_address;
//	IPCFG_IP_ADDRESS_DATA   auto_ip_data;
//	
//   /* runtime RTCS configuration for devices with small RAM, for others the default BSP setting is used */
//   //4,2,20
//	_RTCSPCB_init = 8;
//	_RTCSPCB_grow = 4;
//	_RTCSPCB_max = 32;
//	_RTCS_msgpool_init = 8;
//	_RTCS_msgpool_grow = 4;
//	_RTCS_msgpool_max  = 32;
//	_RTCS_socket_part_init = 8;
//	_RTCS_socket_part_grow = 4;
//	_RTCS_socket_part_max  = 32;
////  _RTCSTASK_stacksize = 3000;//default
//
//	error = RTCS_create();
//    if (error != RTCS_OK) 
//    {
//        printf("\nRTCS failed to initialize, error = %X", error);
//        //_task_block();
//		return RTCS_ERR;   
//	}
//    _IP_forward = TRUE;	//·��Э�飬����IPת��
//	/* Get the Ethernet address of the device */
//    ipcfg_get_mac(BSP_DEFAULT_ENET_DEVICE, enet_address);
//	/* Initialize the Ehternet device */
//	if (IPCFG_OK != ipcfg_init_device(BSP_DEFAULT_ENET_DEVICE, enet_address))
//	{
//		printf ("\nInit eth dev fails");
//		return INIT_ERR;
//	}
//	
//	if (TRUE != ipcfg_get_link_active(BSP_DEFAULT_ENET_DEVICE))
//	{
//		eth_plug_led_off();
//		eth_ser_led_off();
//		printf ("\nCable didn't plug");
//		return PLUG_ERR;
//	}
//		
////	ip_data.ip = ENET_IPADDR;
////	ip_data.mask = ENET_IPMASK;
////	ip_data.gateway = ENET_IPGATEWAY;
////	/* Bind Ethernet device to network using constant (static) IP address information */
////	error = ipcfg_bind_staticip(BSP_DEFAULT_ENET_DEVICE, &ip_data);
////	if (error) return STATIC_ERR;
//	
////	auto_ip_data.ip = 0x0a;
////	auto_ip_data.mask = PhubInfoPtr->EthCfg.MaskStatic;
////	auto_ip_data.gateway = 0x12345678;
//	error = ipcfg_bind_dhcp_wait(BSP_DEFAULT_ENET_DEVICE, FALSE, NULL);
//    if (error != IPCFG_OK)
//    {
//		printf ("\nDHCP binding fails\n");
//        printf ("Error during dhcp bind %08x!\n", error);
//        return DHCP_ERR;
//    }
//	ipcfg_task_create(8, 2000);
//	if (! ipcfg_task_status()) _task_block();//_task_restart������	
//	
//#if 0   //ENABLE SNTP Client task
//	error = SNTP_init("SNTP", 18, 512, IPADDR(61, 129, 42, 44), 2); 			//�����ַ��λ����Ҫ��̬��ȡ�����е�ser��ַ
//	_time_delay(1000*5);	
//#endif    
//	error = sntp_sync();
//    if (error != 0 )
//    {
//        ;
//    }
//
////	socket_info[0].Status = SOCKET_INIT;
////	socket_info[1].Status = SOCKET_INIT;
////	socket_info[2].Status = SOCKET_INIT;
////	eth_socket(0, 1);
//
//	return UPLINK_OK;
//}



