#include "stdafx.h"
#include "dtcomm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CDTComm::CDTComm()
{
	Handle=INVALID_HANDLE_VALUE;
}

CDTComm::~CDTComm()
{
	CloseComm();
}

BOOL CDTComm::IsOpen() //串口是否打开
{
	return Handle!=INVALID_HANDLE_VALUE;
}

//打开串口
BOOL CDTComm::OpenComm(LPCSTR port, DWORD baud, int parity, int bits, int stopbits)
{
	WCHAR unicode_port[MAX_PATH];
	WCHAR unicode_port2[MAX_PATH+10] ={0};
	WCHAR *wszCov = L"\\\\.\\" ;

	char buff[32] ={0} ;
	char *pTag = (char *)strstr(port ,":") ;
	if(pTag != NULL)
	{
		strncpy(buff ,port ,pTag - port) ;
	}else
	{
		strcpy(buff ,port) ;
	}

	MultiByteToWideChar(CP_ACP,0,buff,-1,unicode_port,MAX_PATH);

	wcscat(unicode_port2 ,wszCov) ;
	wcscat(unicode_port2 ,unicode_port) ;
	//AfxMessageBox(unicode_port2) ;



	return OpenCommU(unicode_port2,baud,parity,bits,stopbits);
}

//打开串口，UNICODE方法
BOOL CDTComm::OpenCommU(LPCWSTR port, DWORD baud, int parity, int bits, int stopbits)
{
	CString strError ;
	COMMTIMEOUTS timeouts={	// 串口超时控制参数
		MAXDWORD,			// 读字符间隔超时时间: 100 ms
		0,					// 读操作时每字符的时间: 1 ms (n个字符总共为n ms)
		0,					// 基本的(额外的)读超时时间: 500 ms
		0,					// 写操作时每字符的时间: 1 ms (n个字符总共为n ms)
		0};					// 基本的(额外的)写超时时间: 100 ms

	Handle= CreateFileW(port,	// 串口名称或设备路径
			GENERIC_READ|GENERIC_WRITE,	// 读写方式
			0,					// 共享方式：独占
			NULL,				// 默认的安全描述符
			OPEN_EXISTING,		// 创建方式
			0,					// 不需设置文件属性
			NULL);				// 不需参照模板文件

	if(Handle==INVALID_HANDLE_VALUE){
		strError.Format(TEXT("Open com err: %d") ,GetLastError()) ;
		//AfxMessageBox(strError) ;
		return FALSE;
	}

	if(!SetCommTimeouts(Handle, &timeouts)){
		return FALSE;
	}

	if(!SetCommBufferSize(COMM_DEFAULT_INPUT_BUFFER_SIZE, COMM_DEFAULT_OUTPUT_BUFFER_SIZE)){
		return FALSE;
	}

	if(!SetCommPara(baud, parity, bits, stopbits)){
		return FALSE;
	}

	return TRUE;
}

//串口的缓冲区大小
BOOL CDTComm::SetCommBufferSize(int inbufsize, int outbufsize)
{
	if(Handle==INVALID_HANDLE_VALUE){
		return FALSE;
	}

	if(!SetupComm(Handle, inbufsize, outbufsize)){
		return FALSE;
	}

	return TRUE;
}

//设置串口的参数
BOOL CDTComm::SetCommPara(DWORD baud, int parity, int bits, int stopbits)
{
	DCB dcb;
	
	if(Handle==INVALID_HANDLE_VALUE){
		return FALSE;
	}

	if(!GetCommState(Handle, &dcb)){
		return FALSE;
	}
	dcb.BaudRate = baud;
	dcb.ByteSize = bits;
	dcb.Parity = parity;
	dcb.StopBits = stopbits;


	//计算出 OnByteTakeTime
	INT one_byte_bits;
	one_byte_bits=1;	//起始位
	one_byte_bits+=bits;	//数据位
	one_byte_bits+=(parity?1:0);
	one_byte_bits+=(stopbits?2:1);

	OnByteTakeTimeMicroSecond=1000000/(baud/one_byte_bits);

	dcb.fRtsControl=0;	//关闭RTS

	if(!SetCommState(Handle, &dcb)){
		return FALSE;
	}

	
	return TRUE;
}


//关闭串口
BOOL CDTComm::CloseComm()
{
	if(Handle==INVALID_HANDLE_VALUE){
		return FALSE;
	}

	if(!CloseHandle(Handle)){
		return FALSE;
	}

	Handle=INVALID_HANDLE_VALUE;

	return TRUE;
}

//写数据到串口
DWORD CDTComm::WriteComm(BYTE * pdata, DWORD datalen)
{
	if(Handle==INVALID_HANDLE_VALUE){
		return 0;
	}
	DWORD write;
	WriteFile(Handle, pdata, datalen, &write, NULL);
	return write;
}

//读串口数据
DWORD CDTComm::ReadComm(BYTE * buffer, DWORD readcount)
{
	DWORD read;

	if(Handle==INVALID_HANDLE_VALUE){
		return 0;
	}
	ReadFile(Handle, buffer, readcount, &read, NULL);
	return read;
}

//读串口
DWORD CDTComm::ReadComm(PBYTE buffer, DWORD readcount, BOOL * breakflag)
{
	if(Handle==INVALID_HANDLE_VALUE){
		return 0;
	}

	DWORD total_read=0;
	DWORD read=0;	// 串口收到的数据长度

	//10倍超时
	DWORD end_time = GetTickCount() + (readcount * OnByteTakeTimeMicroSecond)/100;
	//DWORD end_time = GetTickCount() + (readcount * OnByteTakeTimeMicroSecond)/1000;

	while(total_read < readcount)
	{
		if(breakflag)
		{
			if(*breakflag)
			{
				break;
			}
		}

		read=0;
		ReadFile(Handle, buffer+total_read, readcount-total_read, &read, NULL);
		total_read+=read;		
		
		if(GetTickCount() > end_time)
		{
			break;
		}
	}

	return total_read;
}


BOOL CDTComm::PurgeComm(DWORD flag)
{
	if(Handle==INVALID_HANDLE_VALUE){
		return 0;
	}

//	FlushFileBuffers(Handle) ;
	::PurgeComm(Handle,flag);

	return TRUE;
}

BOOL CDTComm::SetRTS(BOOL state)
{
	DCB dcb;
	
	if(Handle==INVALID_HANDLE_VALUE){
		return FALSE;
	}
	
	if(!GetCommState(Handle, &dcb)){
		return FALSE;
	}
	
	dcb.fRtsControl=state;
	
	if(!SetCommState(Handle, &dcb)){
		return FALSE;
	}
	
	return TRUE;
}

BOOL CDTComm::GetRTS(BOOL & state)
{
	DCB dcb;
	
	if(Handle==INVALID_HANDLE_VALUE){
		return FALSE;
	}
	
	if(!GetCommState(Handle, &dcb)){
		return FALSE;
	}

	state=dcb.fRtsControl;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CDTSocket::CDTSocket()
{
	m_Socket = INVALID_SOCKET ;
	m_bInit = FALSE ;
	m_bConnect = FALSE ;
}

CDTSocket::~CDTSocket()
{
	if(m_Socket != INVALID_SOCKET)
	{
		CloseConnect() ;
	}
	WSACleanup() ;
}

BOOL CDTSocket::InitNetWork() //初始化网络
{
	WSADATA WSA ;
	int nRet = WSAStartup(MAKEWORD(2, 2) ,&WSA) ;
	if(nRet != 0)
	{
		return FALSE ;
	}
	m_Socket = socket(AF_INET ,SOCK_STREAM ,0) ; //构建套接字
	if(m_Socket == INVALID_SOCKET )
		return FALSE ;
	m_bInit = TRUE ;
	return  TRUE ;
}


BOOL CDTSocket::Connect(const char *szIP ,UINT nPort) //连接
{
	if(!szIP)
		return FALSE ;
	if(nPort <= 0 || nPort >65534 )
		return FALSE ;

	if(!m_bInit)
		return FALSE ;

	sockaddr_in stRemote ;
	stRemote.sin_family = AF_INET ;
	stRemote.sin_port = htons(nPort) ;
	stRemote.sin_addr.s_addr = inet_addr(szIP) ;

	int nReturn = connect(m_Socket ,(const sockaddr*)&stRemote ,sizeof(stRemote)) ;
	if(nReturn != SOCKET_ERROR ) //连接失败
	{
		m_bConnect = TRUE ;
	}else
	{
		m_bConnect = FALSE ;
	}

	return  m_bConnect ;
}

BOOL CDTSocket::IsConnected()
{
	return m_bConnect ;
}

int CDTSocket::SendBuf(LPVOID buf ,int nSize) //发送
{
	/*
	if(!buf)
		return 0 ;
	if(nSize <= 0 )
		return 0;

	if(!m_bConnect)
		return 0 ;

	FD_SET fdWrite ;
	TIMEVAL timeOut ;

	FD_ZERO(&fdWrite) ;
	int nSent   = 0 ;
	
	
	FD_SET(m_Socket ,&fdWrite) ;
	
	timeOut.tv_sec = 0; 
	timeOut.tv_usec = 100 ;//100ms 
	
 	int nRet = select(0 ,NULL ,&fdWrite ,NULL ,&timeOut) ;//是否可写
 	if(nRet == SOCKET_ERROR )
		return  nRet ;

	if(FD_ISSET(m_Socket ,&fdWrite))//是否可写
	{
		nSent = send(m_Socket ,(char*)buf ,nSize ,0) ; //接受数据
	}else
	{
		return  2 ;//timeout
	}
*/

	//阻塞式的调用
	int nSent = 0 ;
	if(!buf )
		return 0 ;

	if(!m_bConnect){
		return 0 ;
	}

	nSent = send(m_Socket ,(char*)buf ,nSize ,0) ; //接受数据
	return nSent ;

}

int CDTSocket::RcvBuf(LPVOID buf ,int nSize) //接受
{
// 	if(!buf)
// 		return 0 ;
// 
// 	if((!m_bInit) || (!m_bConnect ))
// 		return 0 ;
// 
// 	FD_SET fdRead ;
// 	TIMEVAL timeOut ;
// 
// 	FD_ZERO(&fdRead) ;
// 	int nRecvd  = 0 ;
// 
// 
// 	FD_SET(m_Socket ,&fdRead) ;
// 
// 	timeOut.tv_sec = 0 ; 
// 	timeOut.tv_usec = 100 ;//100ms 
// 
// 	int nRet = select(0 ,&fdRead ,NULL ,NULL ,&timeOut) ; //是否可读
// 	if(nRet == SOCKET_ERROR ){
// 		return  -1 ;
// 	}
// 
// 	if(FD_ISSET(m_Socket ,&fdRead))	{
// 		nRecvd= recv(m_Socket ,(char*)buf ,nSize ,0 ) ; //接受
// 	}else
// 	{
// 		return  2/*COMM_FAIL_TIMEOUTtimeOut*/ ;
// 	}
// 	
// 	return nRecvd ;

	int nRcvd ;

	if(!buf)
		return 0 ;
	if((!m_bInit) || (!m_bConnect ))
		return 0 ;

	nRcvd= recv(m_Socket ,(char*)buf ,nSize ,0 ) ; //接受
	return nRcvd ;

}

VOID CDTSocket::CloseConnect() //断开连接
{
	if(m_Socket != INVALID_SOCKET){
		shutdown(m_Socket ,SD_BOTH) ;
		closesocket(m_Socket) ;
		m_Socket = INVALID_SOCKET ;
		m_bConnect = FALSE ;
		m_bInit = FALSE ;
		WSACleanup() ;
		DMSG(GMsgConfiger,("Socket close() \r\n")); 
	}
 
}

