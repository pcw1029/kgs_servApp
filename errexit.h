/**
  * @file errexit.h
  * @brief 로그를 기록하기위해 사용되는 함수정의
  * @date 2019/1/14
  * @author 박철우
*/

#ifndef _ERR_EXIT_H_
#define _ERR_EXIT_H_

#include<string.h>
#include<stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define DEBUG_PRINT			"/tmp/.debug"
#define DETAIL_DEBUG_PRINT 	"/tmp/.detailDebug"

#define LOG_SIZE 				256
#define OK						0x00
#define NOK						0xFF


/**
 * @brief 오류상태의 로그를 남기고 종료하는 함수
 * @param format 로그 출력할 문자열들의 주소를 갖는 변수
 * @param ... 가변 파라미터
 */
void errexit(const char* format, ...);

/**
 * @brief 시스템 로그를 남기는 함수
 * @param format 로그 출력할 문자열들의 주소를 갖는 변수
 * @param ... 가변 파라미터
 */
void printLog(const char* format, ...);

/**
 * @brief /tmp/.debug파일이 존재하면 시스템 로그를 남기는 함수
 * @param format 로그 출력할 문자열들의 주소를 갖는 변수
 * @param ... 가변 파라미터
 */
void printDebugLog(const char* format, ...);

/**
 * @brief /tmp/.detailDebug파일이 존재하면 매개변수로 들어온 데이터를 HEX값으로 시스템 로그에 기록
 * @param i_pchBuff 로그 출력할 데이터
 * @param i_iSize 로그에 출력할 데이터 크기
 * @param i_chSubject 로그 출력을 요청한 주체자 이름
 */
void printRawData(const char* i_pchBuff, int i_iSize, char* i_chSubject);


#ifdef __cplusplus
}
#endif

#endif

