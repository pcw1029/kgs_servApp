#include "recvCmdFromCtrlPc.h"

#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include "server.h"


extern int errno;

/**
 * @brief 제어 운용에 필요한 변수및 구조체 초기화
 * @param [in] i_pstOpData 운용에 필요한 변수 및 큐 구조체
 * @return 초기화 결과 반환 [성공 : true, 실패 : false]
 */
bool initOperationDataStruct(OPERATION_DATA* i_pstOpData)
{
	i_pstOpData->pstRecvQueue = createQueue("RECV_CMD_QUEUE", MAX_Q_DATA);
	if(i_pstOpData->pstRecvQueue == NULL){
		printLog("RECV_CMD_QUEUE 큐 생성 실패");
		return false;
	}

	i_pstOpData->pstSendQueue = createQueue("SEND_QUEUE", MAX_Q_DATA);
	if(i_pstOpData->pstSendQueue == NULL){
		printLog("SEND_QUEUE 큐 생성 실패");
		return false;
	}

	i_pstOpData->iClientSock = DISCONNECT;
	i_pstOpData->iThreadStatus = OPERATION_THREAD_INIT;
	return true;
}

int main()
{
	pthread_t recvFromCtrlPcId;

	OPERATION_DATA stOpData;

	if(initOperationDataStruct(&stOpData) == false){
		errexit("운용에 필요한 변수 및 구조체 초기화가 비정상입니다.");
	}

	if(pthread_create(&recvFromCtrlPcId, NULL, recvCmdFromCtrlPc, (void*)(&stOpData)))
		errexit("create recvCommand thread failed: %s\n", strerror(errno));

	usleep(200 * 1000);
	while(1){
		if((stOpData.iThreadStatus & RECV_FROM_CTRL_PC_THREAD) != RECV_FROM_CTRL_PC_THREAD){
			pthread_join(recvFromCtrlPcId, NULL);
			if(pthread_create(&recvFromCtrlPcId, NULL, recvCmdFromCtrlPc, (void*)(&stOpData)))
				errexit("create recvCommand thread failed: %s\n", strerror(errno));
			usleep(50 * 1000);
		}
		usleep(50 * 1000);
	}
}
