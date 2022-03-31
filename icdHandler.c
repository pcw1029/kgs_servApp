#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "sock.h"
#include "icdHandler.h"

/**
 * @brief 송신큐에 명령처리 결과 데이터 저장
 * @param [in] i_pstSendQueue 송신큐 포인터
 * @param [in] i_pchSendData 송신큐에 저장할 데이터
 * @param [in] i_iSize 송신큐에 저장할 데이터 크기
 * @return 송신큐에 데이터 저장 결과 [성공 : true, 실패 : false]
 */
bool pushResultDataToQueue(QUEUE* i_pstSendQueue, char* i_pchSendData, int i_iSize)
{
	QDATA* pstQData;
	pstQData = qDataCreate((void*)i_pchSendData, i_iSize);

	if(pstQData == NULL){
		printLog("로컬통신의 큐 데이터 생성 실패");
		return false;
	}else{
		if(qDataPushMutex(i_pstSendQueue, &pstQData->stQLink) != EOK){
			printLog("%s():%d QUEUE PUSH 실패",__func__, __LINE__);
		}
	}
	return true;
}

/**
 * @brief 통신상태 점검
 * @param [in] i_pstSendQueue 송신큐 포인터
 * @return 통신상태 점검 결과를 송신큐에 저장 결과 [성공 : true, 실패 : false]
 */
bool keepAlive(QUEUE* i_pstSendQueue)
{
	ETH_FRAME* pstSendEthFrame;
	char chSendData[sizeof(ETH_FRAME)];
	char chResult = OK;

	memset(chSendData, 0x0, sizeof(chSendData));
	pstSendEthFrame = (ETH_FRAME*)chSendData;
	pstSendEthFrame->uchOpCode = CMD_KEEP_ALIVE;
	pstSendEthFrame->chCtrlItem = chResult;

	if(pushResultDataToQueue(i_pstSendQueue, chSendData, sizeof(chSendData))==false)
		return false;
	return true;
}

/**
 * @brief 작동할 통신사 선택 
 * @param [in] i_pchRecvQData 추적 모드 정보
 * @param [in] i_pstSendQueue 송신큐 포인터
 * @return 작동할 통신사 선택 결과 반환[성공 : true, 실패 : false]
 */
bool emulatorTurnOn(QUEUE* i_pstSendQueue, char chCtrlValue)
{
	ETH_FRAME* pstSendEthFrame;
	char chSendData[sizeof(ETH_FRAME)];
	char chResult = OK;

	if(access("/tmp/.start",0)==0){
		chResult = NOK;
	}else{
		if(chCtrlValue == SK_LTE){
			printDebugLog("작동할 통신사 lte sk");
			system("/root/start.sh sk_lte");
		}else if(chCtrlValue == KT_LTE){
			printDebugLog("작동할 통신사 lte kt");
			system("/root/start.sh kt_lte");
		}else if(chCtrlValue == LG_LTE){
			printDebugLog("작동할 통신사 lte lg");
			system("/root/start.sh lg_lte");
		}else if(chCtrlValue == SK_5G){
			printDebugLog("작동할 통신사 5g sk");
			system("/root/start.sh sk_5g");
		}else if(chCtrlValue == KT_5G){
			printDebugLog("작동할 통신사 5g kt");
			system("/root/start.sh kt_5g");
		}else if(chCtrlValue == LG_5G){
			printDebugLog("작동할 통신사 5g lg");
			system("/root/start.sh lg_5g");
		}else{
			printDebugLog("작동할 통신사를 알수없습니다.");
			chResult = NOK;
		}
	}
	usleep(10);

	memset(chSendData, 0x0, sizeof(chSendData));
	pstSendEthFrame = (ETH_FRAME*)chSendData;
	pstSendEthFrame->uchOpCode = CMD_EMULATOR_TURN_ON;
	pstSendEthFrame->chCtrlItem = chResult;

	if(pushResultDataToQueue(i_pstSendQueue, chSendData, sizeof(chSendData))==false)
		return false;
	return true;
}

/**
 * @brief 작동 중지할 통신사 선택 
 * @param [in] i_pchRecvQData 추적 모드 정보
 * @param [in] i_pstSendQueue 송신큐 포인터
 * @return 작동할 통신사 선택 결과 반환[성공 : true, 실패 : false]
 */
bool emulatorTurnOff(QUEUE* i_pstSendQueue)
{
	ETH_FRAME* pstSendEthFrame;
	char chSendData[sizeof(ETH_FRAME)];
	char chResult = OK;

	system("/root/stop.sh");
	usleep(10);

	memset(chSendData, 0x0, sizeof(chSendData));
	pstSendEthFrame = (ETH_FRAME*)chSendData;
	pstSendEthFrame->uchOpCode = CMD_EMULATOR_TURN_OFF;
	pstSendEthFrame->chCtrlItem = chResult;

	if(pushResultDataToQueue(i_pstSendQueue, chSendData, sizeof(chSendData))==false)
		return false;
	return true;

}

/**
 * @brief 시스템 shutdown
 * @param [in] i_pstSendQueue 송신큐 포인터
 * @return 시스템 shutdown 실행 결과 반환[성공 : true, 실패 : false]
 */
bool systemShutdown(QUEUE* i_pstSendQueue)
{
	ETH_FRAME* pstSendEthFrame;
	char chSendData[sizeof(ETH_FRAME)];
	char chResult = OK;

	printLog("시스템 파워 OFF");
	system("/root/stop.sh");
	system("/root/shutdown.sh &");
	memset(chSendData, 0x0, sizeof(chSendData));
	pstSendEthFrame = (ETH_FRAME*)chSendData;
	pstSendEthFrame->uchOpCode = CMD_SYSTEM_TURN_OFF;
	pstSendEthFrame->chCtrlItem = chResult;

	if(pushResultDataToQueue(i_pstSendQueue, chSendData, sizeof(chSendData))==false)
		return false;
	return true;
}


bool icdHandler(char* i_pchRecvQData, OPERATION_DATA* pstOpData)
{
	bool bRetVal;
	unsigned char uchOpCode;
	char chCtrlValue;
	ETH_FRAME* pstEthDataFrame;

	pstEthDataFrame = (ETH_FRAME *)i_pchRecvQData;
	uchOpCode = pstEthDataFrame->uchOpCode;
	chCtrlValue = pstEthDataFrame->chCtrlItem;

	switch(uchOpCode){
		case CMD_KEEP_ALIVE :
			printLog("통신상태 점검");
			bRetVal = keepAlive(pstOpData->pstSendQueue);
			break;
		case CMD_EMULATOR_TURN_ON:
			printLog("작동할 통신사 선택");
			bRetVal = emulatorTurnOn(pstOpData->pstSendQueue, chCtrlValue);
			break;
		case CMD_EMULATOR_TURN_OFF:
			printLog("작동 중지할 통신사 선택");
			bRetVal = emulatorTurnOff(pstOpData->pstSendQueue);
			break;
		case CMD_SYSTEM_TURN_OFF:
			printLog("시스템 SHUTDOWN");
			bRetVal = systemShutdown(pstOpData->pstSendQueue);
			break;
		default :
			printLog("알수없는 명령입니다.[명령 ICD번호 : %02X]", uchOpCode);
			bRetVal = false;
			break;
	}
	return bRetVal;
}
