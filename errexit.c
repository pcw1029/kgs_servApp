/**
  * @file errexit.c
  * @brief 로그를 남기는 함수 모음
  * @date 2019/1/14
  * @author 박철우
*/
#include "errexit.h"

#include <unistd.h>
#include <stdarg.h>     
#include <stdio.h>
#include <stdlib.h>
#include <sys/syslog.h>
#include <syslog.h>

void errexit(const char* format, ...)
{
    va_list args;
	char chDebugMsg[LOG_SIZE];

    va_start(args, format);
	memset(chDebugMsg, 0x0, sizeof(chDebugMsg));
	vsprintf(chDebugMsg, format, args);
   	syslog(LOG_INFO,"%s\n",chDebugMsg);
	va_end(args);
    exit(1);
}

void printLog(const char* format, ...)
{
    va_list args;
	char chDebugMsg[LOG_SIZE];

    va_start(args, format);
	memset(chDebugMsg, 0x0, sizeof(chDebugMsg));
	vsprintf(chDebugMsg, format, args);
#ifdef UNIT_TEST
	fprintf(stderr,"%s\n", chDebugMsg);
#else
	syslog(LOG_INFO,"%s\n",chDebugMsg);
#endif
	va_end(args);
}

void printDebugLog(const char* format, ...)
{
    va_list args;
	char chDebugMsg[LOG_SIZE];

    va_start(args, format);
	if(access(DEBUG_PRINT, 0)==0){
		memset(chDebugMsg, 0x0, sizeof(chDebugMsg));
		vsprintf(chDebugMsg, format, args);
#ifdef UNIT_TEST
		fprintf(stderr,"%s\n", chDebugMsg);
#else
    	syslog(LOG_INFO,"%s\n",chDebugMsg);
#endif
	}
	va_end(args);
}

void printRawData(const char* i_pchBuff, int i_iSize, char* i_chSubject)
{
	int i;
	const char *chTmp;
	char chMsg[128];

	if(access(DETAIL_DEBUG_PRINT, 0)==0){
		chTmp = i_pchBuff;
		printDebugLog("############# PRINT DATA(size:%03d) #############", i_iSize);
		printDebugLog("****************** [%s] RAW DATA *****************", i_chSubject);
		memset(chMsg,0x0, sizeof(chMsg));
		for(i=1; i<=i_iSize; i++){
			short nTmp;
			nTmp = chTmp[i-1] & 0x0000ff;
			if(i%16 == 1){
				memset(chMsg,0x0, sizeof(chMsg));
				sprintf(chMsg,"0x%08X : ",i-1);
			}

			if(i%16 == 0){
				sprintf(chMsg,"%s%02X", chMsg, nTmp);
				printDebugLog("%s",chMsg);
			}else if(i%8 == 0){
				if(i==i_iSize){
					sprintf(chMsg,"%s%02X",chMsg, nTmp);
					printDebugLog("%s",chMsg);
				}else
					sprintf(chMsg,"%s%02X - ",chMsg, nTmp);
			}else{
				if(i==i_iSize){
					sprintf(chMsg,"%s%02X",chMsg, nTmp);
					printDebugLog("%s",chMsg);
				}else
					sprintf(chMsg,"%s%02X|",chMsg, nTmp);
			}
		}
		printDebugLog("*************************************************");
		printDebugLog("##################################################");
	}
}


