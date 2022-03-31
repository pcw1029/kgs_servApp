/*
 * sock.h
 *
 *  Created on: 2020. 4. 26.
 *      Author: pcw1029
 */

#ifndef INCLUDE_SOCK_H_
#define INCLUDE_SOCK_H_

#include <stdbool.h>

#define BACK_LOG			5
#define CONNECTED			0x01
#define DISCONNECTED		0x00

#define RECV_DATA_SIZE		128
#define SEND_DATA_SIZE		128

#ifdef SOCKET_UNIT_TEST
	#define MOCKABLE(method) mockable##method
#else
	#define MOCKABLE(method) method
#endif

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief ETH_DATA_FRAME 구조체
 * 제어PC와 TCP/IP통신에 사용되는 데이터 포멧
 */
typedef struct __attribute__((__packed__)){
	unsigned char	uchOpCode;		/**< 제어 항목 */
	char			chCtrlItem;		/**< 제어 값 */
}ETH_FRAME;

/**
* @brief 제어PC와 TCP통신을 위한 소켓 생성 함수
* @param[in] i_iPort TCP통신에 사용될 포트 번호
* @return 소켓 생성 성공시 listen()의 반환된 소켓 파일디스크립트 반환, 실패시 -1
*/
int createSocket(int i_iPort);

/**
 * @brief 제어PC 접속 대기 함수
 * @param[in] i_iListenSock listen소켓 파일디스크립트 반환
 * @return 접속한 클라이언트 소켓 파일디스크립트 반환
 */
int acceptClient(int i_iListenSock);

/**
 * @brief RAW소켓 서버에 연결요청 함수
 * @param[in] i_iPort TCP통신에 사용될 포트 번호
 * @return 접속한 서버의 소켓 파일디스크립트
 */
int connectSock(int i_iPort);

/**
 * @brief 제어PC로 패킷 수신 함수
 * @param[in] i_iClientSock 소켓 파일디스크립트
 * @param[out] i_chRecvBuff 수신된 데이터 저장 변수
 * @param[in] i_iRecvSize 수신할 데이터 크기
 * @return read()함수의 반환값 [성공:데이터 크기, 접속 종료:0, 오류:-1]
 */
int recvDataFromCtrlPC(const int i_iClientSock, char* i_chRecvBuff, const int i_iRecvSize);

/**
 * @brief 제어PC로 패킷 송신 함수
 * @param[in] i_iClientSock 소켓 파일디스크립트
 * @param[in] i_chRecvBuff 송신할 데이터
 * @param[in] i_iSendSize 송신할 데이터 사이즈
 * @return write()함수의 반환값 [성공:데이터 크기, 접속 종료:0, 오류:-1]
 */
int sendDataToCtrlPC(const int i_iClientSock, const char* i_chSendBuff, const int i_iSendSize);

#ifdef SOCKET_UNIT_TEST
	int MOCKABLE(connectSock)(int i_iPort);
	extern int (*pConnectSock)(int i_iPort);

	int MOCKABLE(acceptClient)(int i_iListenSock);
	extern int (*pAcceptClient)(int i_iListenSock);

	int MOCKABLE(createSocket)(int i_iPort);
	extern int (*pCreateSocket)(int i_iPort);

	int MOCKABLE(recvDataFromCtrlPC)(const int i_iClientSock, char* i_chRecvBuff, const int i_iRecvSize);
	extern int (*pRecvDataFromCtrlPC)(const int i_iClientSock, char* i_chRecvBuff, const int i_iRecvSize);

	int MOCKABLE(sendDataToCtrlPC)(const int i_iClientSock, const char* i_chSendBuff, const int i_iSendSize);
	extern int (*pSendDataToCtrlPC)(const int i_iClientSock, const char* i_chSendBuff, const int i_iSendSize);
#endif


#ifdef __cplusplus
}
#endif

#endif /* SRC_SOCK_H_ */

