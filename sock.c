/*
 * sock.c
 *
 *  Created on: 2020. 4. 26.
 *      Author: pcw1029
 */


#include "sock.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>

#include "errexit.h"

extern int errno;

#ifdef SOCKET_UNIT_TEST
	int (*pConnectSock)(int i_iPort) = MOCKABLE(connectSock);
	int connectSock(int i_iPort){
		return pConnectSock(i_iPort);
	}

	int (*pAcceptClient)(int i_iListenSock) = MOCKABLE(acceptClient);
	int acceptClient(int i_iListenSock){
		return pAcceptClient(i_iListenSock);
	}

	int (*pCreateSocket)(int i_iPort) = MOCKABLE(createSocket);
	int createSocket(int i_iPort){
		return pCreateSocket(i_iPort);
	}

	int (*pRecvDataFromCtrlPC)(const int i_iClientSock, char* i_chRecvBuff, const int i_iRecvSize) = MOCKABLE(recvDataFromCtrlPC);
	int recvDataFromCtrlPC(const int i_iClientSock, char* i_chRecvBuff, const int i_iRecvSize){
		return pRecvDataFromCtrlPC(i_iClientSock, i_chRecvBuff, i_iRecvSize);
	}

	int (*pSendDataToCtrlPC)(const int i_iClientSock, const char* i_chSendBuff, const int i_iSendSize) = MOCKABLE(sendDataToCtrlPC);
	int sendDataToCtrlPC(const int i_iClientSock, const char* i_chSendBuff, const int i_iSendSize){
		return pSendDataToCtrlPC(i_iClientSock, i_chSendBuff, i_iSendSize);
	}

#endif

int MOCKABLE(createSocket)(int i_iPort)
{
	int iListenFd;
	int iKeepAlive = 1, iKeepAliveIdle = 30, iKeepAliveCnt = 2, iKeepAliveInterval=10;
	int iSetSock = 1;

	int opt = SO_REUSEADDR;
	struct sockaddr_in stSockServerAddr;

	iListenFd = socket(PF_INET, SOCK_STREAM, 0);
	if (iListenFd  < 0){
		printLog("TCP/IP 소켓 생성 실패(%s)", strerror(errno));
		return -1;
	}
	// Allow socket reuse local addr and port
	setsockopt(iListenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	setsockopt(iListenFd, SOL_SOCKET, SO_KEEPALIVE, &iKeepAlive, sizeof(iKeepAlive));
	setsockopt(iListenFd, SOL_TCP, TCP_KEEPIDLE, &iKeepAliveIdle, sizeof(iKeepAliveIdle));
	setsockopt(iListenFd, SOL_TCP, TCP_KEEPCNT, &iKeepAliveCnt, sizeof(iKeepAliveCnt));
	setsockopt(iListenFd, SOL_TCP, TCP_KEEPINTVL, &iKeepAliveInterval, sizeof(iKeepAliveInterval));
	setsockopt(iListenFd, IPPROTO_TCP, TCP_QUICKACK, (void*)&iSetSock, sizeof(iSetSock));
	setsockopt(iListenFd, IPPROTO_TCP, TCP_NODELAY, (void*)&iSetSock, sizeof(iSetSock));

	// Initialize socket addr
	memset(&stSockServerAddr, 0x0, sizeof(struct sockaddr_in));
	stSockServerAddr.sin_family = AF_INET;
	stSockServerAddr.sin_port = htons((unsigned short)i_iPort);
	stSockServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind socket to server address
	if (bind(iListenFd, (struct sockaddr*)&stSockServerAddr, sizeof(struct sockaddr_in)) < 0){
		printLog("TCP/IP 소켓 bind() 실패(%s)", i_iPort, strerror(errno));
    	close(iListenFd);
    	return -1;
    }

    // For TCP socket, convert it to passive mode
    if (listen(iListenFd, BACK_LOG) < 0){
    	printLog("TCP/IP 소켓 listen() 실패(%s)", i_iPort, strerror(errno));
    	close(iListenFd);
    	return -1;
    }
    return iListenFd;
}

int MOCKABLE(acceptClient)(int i_iListenSock)
{
	struct sockaddr_in stSockClientAddr;
    socklen_t socklenSize;
    int iClientFd;
    // Client socket addr
    socklenSize = sizeof(struct sockaddr_in);
    iClientFd = accept(i_iListenSock, (struct sockaddr*)&stSockClientAddr, &socklenSize);
    if (iClientFd < 0){
        printLog("accept failed: %s", strerror(errno));
        return -1;
    }
    return iClientFd;
}

int MOCKABLE(connectSock)(int i_iPort)
{
	int iClientSock;
	int iRetVal;
	struct sockaddr_in   stSockServAddr;

	iClientSock  = socket( PF_INET, SOCK_STREAM, 0);
	if( -1 == iClientSock) {
	  printf( "TCP/IP Client소켓 생성 실패(%s)", strerror(errno));
	  return -1;
	}

	memset( &stSockServAddr, 0, sizeof( stSockServAddr));
	stSockServAddr.sin_family     = AF_INET;
	stSockServAddr.sin_port       = htons( i_iPort);
	stSockServAddr.sin_addr.s_addr= inet_addr( "127.0.0.1");
	iRetVal = connect( iClientSock, (struct sockaddr*)&stSockServAddr, sizeof(stSockServAddr));
	if( iRetVal ==  -1) {
		printLog("TCP/IP 소켓 connect() 실패(%s)",strerror(errno));
	}else{
		printLog("TCP/IP 소켓 연결 성공");
	}
	return iClientSock;
}

int MOCKABLE(recvDataFromCtrlPC)(const int i_iClientSock, char* i_chRecvBuff, const int i_iRecvSize)
{
	return read(i_iClientSock, i_chRecvBuff, i_iRecvSize);
}

int MOCKABLE(sendDataToCtrlPC)(const int i_iClientSock, const char* i_chSendBuff, const int i_iSendSize)
{
	return write(i_iClientSock, i_chSendBuff, i_iSendSize);
}


