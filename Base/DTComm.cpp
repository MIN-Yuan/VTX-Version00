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

BOOL CDTComm::IsOpen() //�����Ƿ��
{
	return Handle!=INVALID_HANDLE_VALUE;
}

//�򿪴���
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

//�򿪴��ڣ�UNICODE����
BOOL CDTComm::OpenCommU(LPCWSTR port, DWORD baud, int parity, int bits, int stopbits)
{
	CString strError ;
	COMMTIMEOUTS timeouts={	// ���ڳ�ʱ���Ʋ���
		MAXDWORD,			// ���ַ������ʱʱ��: 100 ms
		0,					// ������ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
		0,					// ������(�����)����ʱʱ��: 500 ms
		0,					// д����ʱÿ�ַ���ʱ��: 1 ms (n���ַ��ܹ�Ϊn ms)
		0};					// ������(�����)д��ʱʱ��: 100 ms

	Handle= CreateFileW(port,	// �������ƻ��豸·��
			GENERIC_READ|GENERIC_WRITE,	// ��д��ʽ
			0,					// ����ʽ����ռ
			NULL,				// Ĭ�ϵİ�ȫ������
			OPEN_EXISTING,		// ������ʽ
			0,					// ���������ļ�����
			NULL);				// �������ģ���ļ�

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

//���ڵĻ�������С
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

//���ô��ڵĲ���
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


	//����� OnByteTakeTime
	INT one_byte_bits;
	one_byte_bits=1;	//��ʼλ
	one_byte_bits+=bits;	//����λ
	one_byte_bits+=(parity?1:0);
	one_byte_bits+=(stopbits?2:1);

	OnByteTakeTimeMicroSecond=1000000/(baud/one_byte_bits);

	dcb.fRtsControl=0;	//�ر�RTS

	if(!SetCommState(Handle, &dcb)){
		return FALSE;
	}

	
	return TRUE;
}


//�رմ���
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

//д���ݵ�����
DWORD CDTComm::WriteComm(BYTE * pdata, DWORD datalen)
{
	if(Handle==INVALID_HANDLE_VALUE){
		return 0;
	}
	DWORD write;
	WriteFile(Handle, pdata, datalen, &write, NULL);
	return write;
}

//����������
DWORD CDTComm::ReadComm(BYTE * buffer, DWORD readcount)
{
	DWORD read;

	if(Handle==INVALID_HANDLE_VALUE){
		return 0;
	}
	ReadFile(Handle, buffer, readcount, &read, NULL);
	return read;
}

//������
DWORD CDTComm::ReadComm(PBYTE buffer, DWORD readcount, BOOL * breakflag)
{
	if(Handle==INVALID_HANDLE_VALUE){
		return 0;
	}

	DWORD total_read=0;
	DWORD read=0;	// �����յ������ݳ���

	//10����ʱ
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

BOOL CDTSocket::InitNetWork() //��ʼ������
{
	WSADATA WSA ;
	int nRet = WSAStartup(MAKEWORD(2, 2) ,&WSA) ;
	if(nRet != 0)
	{
		return FALSE ;
	}
	m_Socket = socket(AF_INET ,SOCK_STREAM ,0) ; //�����׽���
	if(m_Socket == INVALID_SOCKET )
		return FALSE ;
	m_bInit = TRUE ;
	return  TRUE ;
}


BOOL CDTSocket::Connect(const char *szIP ,UINT nPort) //����
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
	if(nReturn != SOCKET_ERROR ) //����ʧ��
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

int CDTSocket::SendBuf(LPVOID buf ,int nSize) //����
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
	
 	int nRet = select(0 ,NULL ,&fdWrite ,NULL ,&timeOut) ;//�Ƿ��д
 	if(nRet == SOCKET_ERROR )
		return  nRet ;

	if(FD_ISSET(m_Socket ,&fdWrite))//�Ƿ��д
	{
		nSent = send(m_Socket ,(char*)buf ,nSize ,0) ; //��������
	}else
	{
		return  2 ;//timeout
	}
*/

	//����ʽ�ĵ���
	int nSent = 0 ;
	if(!buf )
		return 0 ;

	if(!m_bConnect){
		return 0 ;
	}

	nSent = send(m_Socket ,(char*)buf ,nSize ,0) ; //��������
	return nSent ;

}

int CDTSocket::RcvBuf(LPVOID buf ,int nSize) //����
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
// 	int nRet = select(0 ,&fdRead ,NULL ,NULL ,&timeOut) ; //�Ƿ�ɶ�
// 	if(nRet == SOCKET_ERROR ){
// 		return  -1 ;
// 	}
// 
// 	if(FD_ISSET(m_Socket ,&fdRead))	{
// 		nRecvd= recv(m_Socket ,(char*)buf ,nSize ,0 ) ; //����
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

	nRcvd= recv(m_Socket ,(char*)buf ,nSize ,0 ) ; //����
	return nRcvd ;

}

VOID CDTSocket::CloseConnect() //�Ͽ�����
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

