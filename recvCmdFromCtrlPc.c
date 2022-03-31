#include "cmdProcessing.h"
#include "server.h"
#include "sock.h"
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include "sendResponseToCtrlPc.h"
extern int errno;

void* recvCmdFromCtrlPc(void* i_pvArg)
{
	int iListenSock;
	int iClientSock;
	int iRecvSize;
	char chRecvData[RECV_DATA_SIZE];
	char *pchRecvQData;
	OPERATION_DATA *pstOpData;
	QDATA* pstQData;
	pthread_t cmdProcessingId;
	pthread_t sendToCtrlPcId;
	pstOpData = (OPERATION_DATA *)i_pvArg;

	pstOpData->iThreadStatus |= RECV_FROM_CTRL_PC_THREAD;

	printLog("[리눅스 에뮬레이터 <-> 제어PC] 명령 수신 쓰레드 시작");
	iListenSock = createSocket(8899);
	printLog("[리눅스 에뮬레이터 <-> 제어PC] 제어PC 접속 대기");
	iClientSock = acceptClient(iListenSock);
	printLog("[리눅스 에뮬레이터 <-> 제어PC] 제어PC 접속 수락");

	pstOpData->iClientSock = iClientSock;
	if(pthread_create(&cmdProcessingId, NULL, cmdProcessing, i_pvArg))
		errexit("create commRawSock thread failed: %s\n", strerror(errno));

	if(pthread_create(&sendToCtrlPcId, NULL, sendResponseToCtrlPc, i_pvArg))
		errexit("create sendAzEl thread failed: %s\n", strerror(errno));

	while(1){
		memset(chRecvData, 0x0, sizeof(chRecvData));
		iRecvSize = recvDataFromCtrlPC(iClientSock, chRecvData, RECV_DATA_SIZE);

		if(iRecvSize <= 0){
			printLog("[리눅스 에뮬레이터 <-> 제어PC] 제어PC 연결해지 요청 수신");
			break;
		}
		printRawData(chRecvData, iRecvSize, "RECV_FROM_PC");
		pchRecvQData = (char *)malloc(iRecvSize);
		memset(pchRecvQData, 0x0, iRecvSize);
		memcpy(pchRecvQData, chRecvData, iRecvSize);

		pstQData = qDataCreate((void*)pchRecvQData, iRecvSize);
		if(pstQData == NULL){
			printLog("[리눅스 에뮬레이터] 로컬통신의 큐 데이터 생성 실패");
		}else{
			if(qDataPushMutex(pstOpData->pstRecvQueue, &pstQData->stQLink) != EOK){
				printLog("%s():%d QUEUE PUSH 실패",__func__, __LINE__);
			}
		}
		free(pchRecvQData);
	}
	close(iClientSock);
	close(iListenSock);
	pstOpData->iClientSock = DISCONNECTED;
	pstOpData->iThreadStatus &= (~RECV_FROM_CTRL_PC_THREAD);
	pthread_join(cmdProcessingId, NULL);
	pthread_join(sendToCtrlPcId, NULL);
	printLog("[리눅스 에뮬레이터 <-> 제어PC] 명령 수신 쓰레드 종료");
	return NULL;
}

