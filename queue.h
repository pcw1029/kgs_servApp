/*
 * queue.h
 *
 *  Created on: 2020. 4. 28.
 *      Author: pcw1029
 */

#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_

#include <stddef.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EOK
#define EOK 0
#endif

#define MAX_Q_DATA 			50

/**
 * @brief QUEUE 구조체
 * 큐 관리를 위한 큐핸들러 구조체
 */
typedef struct  __attribute__((__packed__)){
	void 				*pvHead;			/**< 큐안에 저장된 데이터의 처음을 나타내는 포인터 */
	void 				*pvTail;			/**< 큐안에 저장된 데이터의 끝을 나나내는 포인터 */
	int 				iCount;			/**< 큐안에 저장된 데이터의 개수 */
	int					iMax;				/**< 큐안에 저장할 수 있는 데이터 개수 */
	pthread_mutex_t 	stPthredMutex;	/**< 임계영역을 보호하기 위한 Muext Key */
	char 				*pchQueueName;	/**< 해당 큐의 이름 */
}QUEUE;

/**
 * @brief QLINK 구조체
 * 큐에 들어있는 데이터의 이전/다음으로 이동 하기위한 구조체
 */
typedef struct  __attribute__((__packed__)){
	void	*pvNext;	/**< 다음 데이터 포인터 */
	void 	*pvPrev;	/**< 이전 데이터 포인터 */
	QUEUE	*pstQueue;	/**< 데이터를 가지고 있는 큐의 주소 */
}QLINK;

/**
 * @brief QDATA 구조체
 * 큐에 저장될 데이터를 가진 구조체
 */
typedef struct  __attribute__((__packed__)){
	QLINK 		stQLink;		/**< 큐에 들어있는 데이터간의 연결정보를 가진 Linked List*/
	void		*pvData;		/**< 큐에 저장할 데이터 */
	int 		iDataLength;	/**< 큐에 저장할 데이터 길이 */
}QDATA;

/**
 * @brief ACU_AZ_EL_SET_Q_FRAME 구조체
 * 방위각/고각 설정 큐의 데이터 프레임
 */
typedef struct  __attribute__((__packed__)) {
	char chMode;
	double dAz; 	/**< 방위각 */
	double dEl;	/**< 고각 */
}ACU_AZ_EL_SET_Q_FRAME;


/**
* @brief 새로운 큐 생성
* @param i_pchQueueName 생성될 큐의 이름
* @param i_iMax 큐에 저장될 최대 데이터 개수
* @return 생선된 큐의 주소를 반환
*/
QUEUE* 		createQueue(const char *i_pchQueueName, int i_iMax);

/**
* @brief 큐에 저장될 데이터 생성
* @param i_pvData 큐에 저장될 데이터
* @param i_iDataLength 큐에 저장될 데이터 길이
* @return 큐에 저장될 데이터의 주소를 반환
*/
QDATA* 		qDataCreate(const void* i_pvData, int i_iDataLength);

/**
* @brief 큐에서 데이터 삭제
* @param i_pstQdata 큐에 저장된 데이터 주소
* @return NULL반환
*/
void* 			qDataDestroy(QDATA *i_pstQdata);

/**
* @brief 생성된 큐 삭제
* @param i_pstQueue 생성된 큐 주소
* @return 큐 삭제 결과 반환
*/
int 			destroyQueue(QUEUE** i_pstQueue);

/**
* @brief 큐에 데이터 Push
* @param i_pstQueue 생성된 큐 주소
* @param i_pstQLink 생성된 데이터 주소
* @return 큐에 데이터 Push 결과 반환
*/
int 			qDataPushMutex(QUEUE* i_pstQueue, QLINK* i_pstQLink);

/**
* @brief 큐에 데이터 Pop
* @param i_pstQueue 생성된 큐 주소
* @return 큐에 데이터 Pop 결과 반환
*/
void* 			qDataPopMutex(QUEUE* i_pstQueue);

/**
* @brief 큐에 Push된 데이터 개수 확인
* @param i_pstQueue 생성된 큐 주소
* @return 큐에 Push된 데이터 개수 반환
*/
int 			qGetCountMutex(QUEUE* i_pstQueue);

#ifdef __cplusplus
}
#endif
#endif /* SRC_QUEUE_H_ */


