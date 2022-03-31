#ifndef ICDHANDLER_H_
#define ICDHANDLER_H_

#include "server.h"

#define CMD_KEEP_ALIVE				(0x00)
#define CMD_EMULATOR_TURN_ON		(0x01)
#define CMD_EMULATOR_TURN_OFF		(0x02)
#define CMD_SYSTEM_TURN_OFF			(0xFF)
#define CMD_COMMAND_FAIL			(0x10)

#define SK_LTE						(0x00)
#define KT_LTE						(0x01)
#define LG_LTE						(0x02)
#define SK_5G						(0x03)
#define KT_5G						(0x04)
#define LG_5G						(0x05)

bool icdHandler(char* i_pchRecvQData, OPERATION_DATA* pstOpData);


#endif /* ICDHANDLER_H_ */
