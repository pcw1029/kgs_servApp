/*
 * @file queue.c
 * @brief 큐를 생성하고, 큐를 사용하기위한 함수모음
 * @date 2019/1/14
 * @author 박철우
 */

#include "queue.h"

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include<sys/types.h>
#include<sys/errno.h>
#include <stdbool.h>

extern int errno;

QUEUE* createQueue(const char *i_pchQueueName, int i_iMax)
{
	QUEUE* pstQueue;
	int iLocalStatus;

	pstQueue = (QUEUE *)malloc(sizeof(QUEUE));
	if(pstQueue == NULL){
		return NULL;
	} else {
		pstQueue->pvHead = NULL;
		pstQueue->pvTail = NULL;
		pstQueue->iCount = 0;
		pstQueue->iMax = i_iMax;
		iLocalStatus = pthread_mutex_init(&pstQueue->stPthredMutex, NULL);
		pstQueue->pchQueueName = (char *)malloc(strlen(i_pchQueueName)+1);
		memset(pstQueue->pchQueueName, 0x0, strlen(i_pchQueueName)+1);
		memcpy(pstQueue->pchQueueName, i_pchQueueName, strlen(i_pchQueueName));
		if(iLocalStatus != 0){
			free(pstQueue);
			pstQueue = NULL;
			return false;
		}
	}
	return pstQueue;
}

QDATA* qDataCreate(const void* i_pvData, int i_iDataLength)
{
	QDATA* pstQData;

	pstQData = (QDATA*)malloc(sizeof(QDATA));
	if(pstQData == NULL){
		return NULL;
	}else{
		memset(pstQData, 0x0, sizeof(QDATA));
		pstQData->pvData = malloc(i_iDataLength);
		if(pstQData->pvData == NULL){
			free(pstQData);
			pstQData = NULL;
		}else{
			pstQData->stQLink.pvNext = NULL;
			pstQData->stQLink.pvPrev = NULL;
			pstQData->stQLink.pstQueue = NULL;
			memset(pstQData->pvData,0x0, i_iDataLength);
			memcpy(pstQData->pvData, i_pvData, i_iDataLength);
			pstQData->iDataLength = i_iDataLength;
		}
	}
	return pstQData;
}

void* qDataDestroy(QDATA *i_pstQdata)
{
	if(i_pstQdata->pvData != NULL){
		free(i_pstQdata->pvData);
	}
	if(i_pstQdata != NULL){
		free(i_pstQdata);
		i_pstQdata = NULL;
	}
	return NULL;
}

int destroyQueue(QUEUE** i_pstQueue)
{
	QUEUE* pstQueue = *i_pstQueue;
	if(pstQueue == NULL){
		return ENOENT;
	}else{
		if(pthread_mutex_lock(&pstQueue->stPthredMutex) != EOK){
			return errno;
		}
		if((pstQueue->pvHead != NULL) || (pstQueue->pvTail != NULL) || (pstQueue->iCount > 0)){
			if(pthread_mutex_unlock(&pstQueue->stPthredMutex) != EOK){
				return errno;
			}

			for(int i=0; i<pstQueue->iMax; i++){
				if( !((pstQueue->pvHead != NULL) || (pstQueue->pvTail != NULL) || (pstQueue->iCount > 0) ))
					break;
				QDATA* pstQData;
				pstQData = (QDATA *)qDataPopMutex(pstQueue);
				pstQData = (QDATA *)qDataDestroy(pstQData);
			}
		} else {
			if(pthread_mutex_unlock(&pstQueue->stPthredMutex) != EOK)
				return errno;
		}
		free(pstQueue);
		*i_pstQueue = NULL;
	}
	return EOK;
}

/**
 * @brief 큐에 데이터를 추가한다. 외부로 노출되지 않는 함수
 * @param i_pstQueue 큐 핸들러
 * @param i_pstQLink 큐에 들어있는 데이터를 접근하기 위한 Linked List
 * @return 큐에 데이터 추가시 정상이면 0(EOK)이고, 다른값이면 Error
 */
static int qDataPush(QUEUE* i_pstQueue, QLINK* i_pstQLink)
{
	QLINK* pstQLink_CurHead;

	if(i_pstQueue == NULL){
		return EINVAL;
	}else{
		if(i_pstQLink == NULL){
			return EINVAL;
		}else{
			if(i_pstQueue->iCount >= i_pstQueue->iMax){
				return ENOMEM;
			}
			assert(i_pstQLink->pvNext == NULL);
			assert(i_pstQLink->pvPrev == NULL);
			i_pstQLink->pstQueue = i_pstQueue;
			if(i_pstQueue->pvHead == NULL){
				i_pstQueue->pvHead = i_pstQLink;
				i_pstQueue->pvTail = i_pstQLink;
				i_pstQLink->pvNext = NULL;
				i_pstQLink->pvPrev = NULL;
			} else {
				pstQLink_CurHead = (QLINK *)i_pstQueue->pvHead;
				pstQLink_CurHead->pvPrev = i_pstQLink;
				i_pstQLink->pvNext = pstQLink_CurHead;
				i_pstQLink->pvPrev = NULL;
				i_pstQueue->pvHead = i_pstQLink;
			}
			i_pstQueue->iCount++;
			return EOK;
		}
	}
}

int qDataPushMutex(QUEUE* i_pstQueue, QLINK* i_pstQLink)
{
	int iStatus;

	if(pthread_mutex_lock(&i_pstQueue->stPthredMutex) != EOK){
		iStatus = errno;
	}else {
		iStatus = qDataPush(i_pstQueue, i_pstQLink);
		if(pthread_mutex_unlock(&i_pstQueue->stPthredMutex) != EOK){
			iStatus = errno;
		}
	}
	return iStatus;
}

/**
 * @brief 큐에 데이터를 꺼내오기 위한 함수. 외부로 노출되지 않는 함수
 * @param i_pstQueue 큐 핸들러
 * @return 큐에서 꺼내온 데이터에 접근할수 있는 Linked List를 리턴한다.
 */
static QLINK* qDataPop(QUEUE* i_pstQueue)
{
	QLINK* pstQLink_CurHead;
	QLINK* pstQLink_NewTail;
	if(i_pstQueue == NULL){
		pstQLink_CurHead=NULL;
	}else{
		if(i_pstQueue->pvTail == NULL){
			pstQLink_CurHead=NULL;
		}else{
			pstQLink_CurHead = (QLINK *)i_pstQueue->pvTail;
			if(i_pstQueue->pvHead == i_pstQueue->pvTail){
				i_pstQueue->pvHead = NULL;
				i_pstQueue->pvTail = NULL;
			}else{
				i_pstQueue->pvTail = pstQLink_CurHead->pvPrev;
				pstQLink_NewTail = (QLINK *)pstQLink_CurHead->pvPrev;
				pstQLink_NewTail->pvNext = NULL;
			}
			pstQLink_CurHead->pvNext = NULL;
			pstQLink_CurHead->pvPrev = NULL;
			pstQLink_CurHead->pstQueue = NULL;
			i_pstQueue->iCount--;
		}
	}
	return pstQLink_CurHead;
}

void* qDataPopMutex(QUEUE* i_pstQueue)
{
	QLINK* pstQLink_CurHead;

	if(pthread_mutex_lock(&i_pstQueue->stPthredMutex) != EOK){
		return NULL;
	}else
		pstQLink_CurHead = qDataPop(i_pstQueue);

	if(pthread_mutex_unlock(&i_pstQueue->stPthredMutex) != EOK){
		return NULL;
	}
	return (void *)pstQLink_CurHead;
}

/**
 * @brief 큐에 저장된 데이터의 개수를 알수있다. 외부로 노출되지 않는 함수
 * @param i_pstQueue 큐 핸들러
 * @return 큐에 저장된 데이터 개수를 리턴한다.
 */
static int qGetCount(QUEUE* i_pstQueue)
{
	return i_pstQueue->iCount;
}

int qGetCountMutex(QUEUE* i_pstQueue)
{
	int iCount;
	if(pthread_mutex_lock(&i_pstQueue->stPthredMutex) != EOK){
		return errno;
	}else{
		iCount = qGetCount(i_pstQueue);
	}
	if(pthread_mutex_unlock(&i_pstQueue->stPthredMutex) != EOK){
		return errno;
	}
	return iCount;
}

