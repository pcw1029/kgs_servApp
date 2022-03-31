
#ifndef _SERVER_H_
#define _SERVER_H_

#include "queue.h"
#include "errexit.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief OPERATION_DATA 구조체
 * 제어 운용에 필요한 변수 및 구조체 집합
 */

typedef struct  __attribute__((__packed__)) _OPERATION_DATA{
	QUEUE* pstRecvQueue;				/**< 제어PC로부터 수신한 명령을 저장하는 큐 */
	QUEUE* pstSendQueue;				/**< 제어PC로 송신할 명령처리 결과를 저장하는 큐 */

	int iThreadStatus;				/**< 운용에 사용되는 쓰레드의 상태값 */
	int iClientSock;					/**< 제어PC와 TCP/IP통신에 사용되는 소켓정보 */
}OPERATION_DATA;

#define DISCONNECT					-1

/**
 * @def OPERATION_THREAD_INIT
 * 쓰레드 초기상태값 정의
 */
#define OPERATION_THREAD_INIT		0x00

/**
 * @def RECV_FROM_CTRL_PC_THREAD
 * recvFromCtrlPc쓰레드 상태 정의
 */
#define RECV_FROM_CTRL_PC_THREAD	0x01

/**
 * @def RECV_FROM_CTRL_PC_THREAD
 * recvFromCtrlPc쓰레드 상태 정의
 */
#define COMMAND_PRCESSING_THREAD	0x02


/**
 * @def RECV_FROM_CTRL_PC_THREAD
 * recvFromCtrlPc쓰레드 상태 정의
 */
#define SEND_TO_CTRL_PC_THREAD		0x04

bool initOperationDataStruct(OPERATION_DATA* i_pstOpData);


#ifdef __cplusplus
}
#endif

#endif /* _SERVER_H_ */
