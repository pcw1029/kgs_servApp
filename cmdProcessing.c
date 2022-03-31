#include "server.h"
#include "icdHandler.h"
#include "sock.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DISCONNECTED							0x00
/**
* @brief 제어PC로부터 수신한 명령 처리 쓰레드
* @param i_pvArg
* @return 쓰레드 종료 NULL 반환
*/
void* cmdProcessing(void* i_pvArg)
{
	int iRecvQCount;
	char *pchRecvQData;
	OPERATION_DATA *pstOpData;
	QDATA* pstQData;
	pstOpData = (OPERATION_DATA *)i_pvArg;

	pstOpData->iThreadStatus |= COMMAND_PRCESSING_THREAD;

	printLog("[리눅스 에뮬레이터] 명령 처리 쓰레드 시작");
	while(1){
		if(pstOpData->iClientSock == DISCONNECTED){
			break;
		}
		iRecvQCount = qGetCountMutex(pstOpData->pstRecvQueue);
		if(iRecvQCount >= 1){
			pstQData = qDataPopMutex(pstOpData->pstRecvQueue);
			pchRecvQData = (char *)malloc(pstQData->iDataLength);
			memset(pchRecvQData, 0x0, pstQData->iDataLength);
			memcpy(pchRecvQData, pstQData->pvData, pstQData->iDataLength);
			qDataDestroy(pstQData);
			if(icdHandler(pchRecvQData, pstOpData) == false){
				//todo sendQueue에 실패 저장
			}
			free(pchRecvQData);
		}else{
			usleep(100 * 1000);
		}
	}
	pstOpData->iThreadStatus &= (~COMMAND_PRCESSING_THREAD);
	printLog("[리눅스 에뮬레이터] 명령 처리 쓰레드 종료");
	return NULL;
}
