#include <unistd.h>
#include <stdio.h>
#include "server.h"
#include "sock.h"

void* sendResponseToCtrlPc(void* i_pvArg)
{
	int iSendSize;
	int iSendQCount;
	int iWriteSize;
	char chSendData[SEND_DATA_SIZE];
	OPERATION_DATA *pstOpData;
	QDATA* pstQData;

	pstOpData = (OPERATION_DATA *)i_pvArg;
	pstOpData->iThreadStatus |= SEND_TO_CTRL_PC_THREAD;

	printLog("[리눅스 에뮬레이터 <-> 제어PC] 명령 처리 결과 전송 쓰레드 시작");

	while(1){
		if(pstOpData->iClientSock == DISCONNECTED){
			break;
		}
		iSendQCount = qGetCountMutex(pstOpData->pstSendQueue);
		if(iSendQCount >= 1){
			pstQData = qDataPopMutex(pstOpData->pstSendQueue);
			printDebugLog("send data length : %d", pstQData->iDataLength);
			memset(chSendData, 0x0, sizeof(chSendData));
			memcpy(chSendData, pstQData->pvData, pstQData->iDataLength);
			iSendSize = pstQData->iDataLength;
			qDataDestroy(pstQData);
			printRawData(chSendData, iSendSize, "SEND_TO_CTRL_PC");
			iWriteSize = sendDataToCtrlPC(pstOpData->iClientSock, chSendData, iSendSize);
			if(iWriteSize != iSendSize){
				printLog("[리눅스 에뮬레이터 <-> 제어PC] 명령 처리 결과 전송 실패");
				//todo 명령처리 결과 RAW데이터 출력
			}
		}else{
			usleep(100 * 1000);
		}
	}

	pstOpData->iThreadStatus &= (~SEND_TO_CTRL_PC_THREAD);
	printLog("[리눅스 에뮬레이터 <-> 제어PC] 명령 처리 결과 전송 쓰레드 종료");
	return NULL;
}
