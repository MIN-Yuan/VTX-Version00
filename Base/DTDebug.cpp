#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "dtdebug.h"
#include "dtres.h"
#include "dtmisc.h"
#include "DTComm.h"
#include "DTXML.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL GMsgProgrammer=FALSE;
BOOL GMsgConfiger=FALSE;
BOOL GMsgUser=FALSE;
BOOL GMsgKeeper=FALSE;
BOOL GDebugOn = FALSE ;
CMutex GDMSGMutex;

#ifdef DEBUG_MSG	//////////////////////////////////////////////////////////////////////////


#ifdef MSG_BY_FILE
	HANDLE GHDebugFile=INVALID_HANDLE_VALUE;
#else
	CDTComm GDebugComm;
#endif



const CHAR * STR_DEBUG_FILE_NAME="%s%s\\debug.txt";
const CHAR * STR_DEBUG_BACKUP_FILE_NAME="%s%s\\%d-%d-%d-%d%d%d%d.txt";
char GDebugMsgBuffer[MAX_MSG_SIZE];


const char * STR_ERROR_SOMETHING="Error, %s\r\n";
const char * STR_ERROR_OPEN	="Error, Unable Open %s\r\n";
const char * STR_ERROR_ENTER="Error, Unable Enter %s\r\n";
const char * STR_ERROR_ENTER_OF="Error, Unable Enter %s of %s\r\n";
const char * STR_ERROR_MAKE	="Error, Unable Make %s\r\n";
const char * STR_ERROR_INVALID="Error, Invalid %s\r\n";


void GInitDebugMsg()
{
	CDTXML xml;
	CDTStr port;
	CDTStr path;
	path.Format("%ssystem\\debug.xml",GStrFolderRoot.GetBuffer());
	if(!xml.Open(path.GetBuffer())){
		return;
	}
	
	if(!xml.Enter(STR_TOP)){
		return;
	}
//////////////////////////////////////////////////////////////////////////
	//add by fanze
	//08.11.29
	//增加设置DEBUG开启和关闭
	if(!xml.Enter(STR_CONFIG))
		return ;
	
	
	GDebugOn = xml.GetInteger(STR_DEBUGON ,0) ;
	xml.Back() ;
	
	if(!GDebugOn){
		//AfxMessageBox(_T("不启用调试")) ;
		xml.Close() ;
		return  ;
	}
//////////////////////////////////////////////////////////////////////////

	
	if(!xml.Enter(STR_MESSAGE)){
		return;
	}

	GMsgProgrammer=xml.GetInteger("PROGRAMMER",0);
	GMsgConfiger=xml.GetInteger("CONFIGER",0);
	GMsgUser=xml.GetInteger("USER",0);
	GMsgKeeper=xml.GetInteger("KEEPER",0);

	xml.Back();

	//////////////////////////////////////////////////////////////////////////

#ifdef MSG_BY_FILE
	CHAR filename[MAX_PATH];
	TCHAR unicode_filename[MAX_PATH];

	sprintf(filename,STR_DEBUG_FILE_NAME,GStrFolderRoot.GetBuffer(),STR_DEBUG);
	MultiByteToWideChar(CP_ACP,0,filename,-1,unicode_filename,MAX_PATH);

// 	Test For Storage Card
// 		if(!GIsFileExistU(unicode_filename)){
// 			MessageBox(0,L"Unable to access root-folder",L"",0);
// 		}

	//Test For Storage Card
	if(!GIsFileExistU(unicode_filename)){
		MessageBox(0,L"Unable to access root-folder",L"",0);
		return;
	}

	//For Debug File
	if(GHDebugFile==INVALID_HANDLE_VALUE){
		GHDebugFile=CreateFile(unicode_filename,
			GENERIC_WRITE|GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if(GHDebugFile==INVALID_HANDLE_VALUE){
			DMSG(GMsgConfiger,("Init Debug Msg failed\r\n"));
			return;
		}
		SetFilePointer(GHDebugFile,0,NULL,FILE_END);
	}
#else
	if(!xml.Enter(STR_CONFIG)){
		return;
	}

	DWORD baud;
	INT parity;
	INT bits;
	INT stopbits;


	port=xml.GetString(STR_PORT,"COM4:");
	baud=xml.GetInteger(STR_BAUD,115200);
	parity=xml.GetInteger(STR_PARITY,0);
	bits=xml.GetInteger(STR_BITS,8);
	stopbits=xml.GetInteger(STR_STOPBITS,0);

	if(!GDebugComm.OpenComm(port.GetBuffer(),baud,parity,bits,stopbits)){
		MessageBox(0,_T("Unalbe Open Serial Port for DEBUG MESSAGE"),NULL,MB_OK);
		return;
	}

#endif
}

void GUninitDebugMsg()
{
#ifdef MSG_BY_FILE
	//Release For Debug File.
	if(GHDebugFile!=INVALID_HANDLE_VALUE){
		CloseHandle(GHDebugFile);
		GHDebugFile=INVALID_HANDLE_VALUE;
	}
#else
	if(!GDebugOn){
		return  ;
	}
	GDebugComm.CloseComm();
#endif
}

void GDebugMsg(const char * format, ...)
{
	if(!GDebugOn){
		return ;
	}
	GDMSGMutex.Lock();
		
#ifdef MSG_BY_FILE
	int len;
	va_list vaptr;
	DWORD written;
	DWORD filesize;

	if(GHDebugFile==INVALID_HANDLE_VALUE){
		GDMSGMutex.Unlock();
		return;
	}

	va_start(vaptr,format);
	len=_vsnprintf(GDebugMsgBuffer,MAX_MSG_SIZE-1,format,vaptr);
	va_end(vaptr);
	
	if(len<0){
		GDMSGMutex.Unlock();
		return;		
	}

	WriteFile(GHDebugFile,GDebugMsgBuffer,len,&written,NULL);

	filesize=GetFileSize(GHDebugFile,NULL);
	if(filesize>MAX_MSG_FILE_SIZE){
		CHAR str_buf[MAX_PATH];
		TCHAR uni_backfile[MAX_PATH];
		TCHAR uni_debugfile[MAX_PATH];
		SYSTEMTIME time;
		GetSystemTime(&time);

		sprintf(str_buf, STR_DEBUG_BACKUP_FILE_NAME,GStrFolderRoot.GetBuffer(),STR_DEBUG,time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		MultiByteToWideChar(CP_ACP,0,str_buf,-1,uni_backfile,MAX_PATH);

		sprintf(str_buf, STR_DEBUG_FILE_NAME, GStrFolderRoot.GetBuffer(), STR_DEBUG);
		MultiByteToWideChar(CP_ACP,0,str_buf,-1,uni_debugfile,MAX_PATH);

		CopyFile(uni_debugfile,uni_backfile,FALSE);

		SetFilePointer(GHDebugFile,0,NULL,FILE_BEGIN);
		SetEndOfFile(GHDebugFile);
	}
#else
	if(!GDebugOn){
		GDMSGMutex.Unlock() ;
		return ;
	}
	if(!GDebugComm.IsOpen()){
		GDMSGMutex.Unlock();
		return;
	}

	int len;
	va_list vaptr;

	va_start(vaptr,format);
	len=_vsnprintf(GDebugMsgBuffer,MAX_MSG_SIZE-1,format,vaptr);
	va_end(vaptr);

	if(len<0){
		GDMSGMutex.Unlock();
		return;		
	}
	
	GDebugComm.WriteComm((PBYTE)GDebugMsgBuffer,len);
#endif
	GDMSGMutex.Unlock();
}

VOID USE(...)
{
	
}

#endif	//#ifdef DEBUG_MSG	//////////////////////////////////////////////////////////////////////////





