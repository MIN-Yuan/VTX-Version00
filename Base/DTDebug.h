///////////////////////////////////////////////////////////////////////////////////////////
// DEBUG Message out put module
// Author : Dengting
// Edition: 2007-06-21
//
///////////////////////////////////////////////////////////////////////////////////////////
#ifndef HEAD_DEBUG_H
#define HEAD_DEBUG_H

#define DEBUG_MSG	1
//#define MSG_BY_FILE	1	//定义为空,则为串口消息输入

#define MAX_MSG_FILE_SIZE 409600
#define MAX_MSG_SIZE 4096


#ifdef _X86_
#define __unaligned
#endif


extern BOOL GMsgProgrammer;
extern BOOL GMsgConfiger;
extern BOOL GMsgUser;
extern BOOL GMsgKeeper;


#ifdef DEBUG_MSG
	extern const CHAR * STR_DEBUG_FILE_NAME;
	extern const CHAR * STR_DEBUG_BACKUP_FILE_NAME;
	extern char GDebugMsgBuffer[MAX_MSG_SIZE];


	extern const char * STR_ERROR_SOMETHING;
	extern const char * STR_ERROR_OPEN;
	extern const char * STR_ERROR_ENTER;
	extern const char * STR_ERROR_ENTER_OF;
	extern const char * STR_ERROR_MAKE;
	extern const char * STR_ERROR_INVALID;
	extern const char * STR_MSG_SEND;
	extern const char * STR_MSG_RCIV;


	void GInitDebugMsg();
	void GUninitDebugMsg();
	void GDebugMsg(const char * format, ...);

	#define INITDEBUGMSG() GInitDebugMsg()
	#define UNINITDEBUGMSG() GUninitDebugMsg()
	#define DMSG(cond,printf_exp) ((cond)?(GDebugMsg printf_exp),1:0)

	//RETAILMSG ((cond)?(NKDbgPrintfW printf_exp),1:0)

	#define DMSG_ERROR(a) DMSG(1,(STR_ERROR_SOMETHING,(a)))
	#define DMSG_ERROR_OPEN(a) DMSG(1,(STR_ERROR_OPEN,(a)))
	#define DMSG_ERROR_ENTER(a) DMSG(1,(STR_ERROR_ENTER,(a)))
	#define DMSG_ERROR_ENTER_OF(a,b) DMSG(1,(STR_ERROR_ENTER_OF,(a),(b)))
	#define DMSG_ERROR_MAKE(a) DMSG(1,(STR_ERROR_MAKE,(a)))
	#define DMSG_ERROR_INVALID(a) DMSG(1,(STR_ERROR_INVALID,(a)))
#else
	#define INITDEBUGMSG()
	#define UNINITDEBUGMSG()
	#define DMSG(cond,printf_exp)

	#define DMSG_ERROR(a)
	#define DMSG_ERROR_OPEN(a)
	#define DMSG_ERROR_ENTER(a)
	#define DMSG_ERROR_ENTER_OF(a,b)
	#define DMSG_ERROR_MAKE(a)
	#define DMSG_ERROR_INVALID(a)


#endif

VOID USE(...);


#endif	//HEAD_DEBUG_H