///////////////////////////////////////////////////////////////////////////////////////
// Windows Serial Port Operatings
// DENGTING

// 2007.10.2
// 2007.01.25
///////////////////////////////////////////////////////////////////////////////////////
#ifndef H_DT_COMM_MODULE
#define H_DT_COMM_MODULE

#define COMM_DEFAULT_INPUT_BUFFER_SIZE	4096	//串口缓冲512改为4096 RTT/20081023
#define COMM_DEFAULT_OUTPUT_BUFFER_SIZE	4096

class CDTComm
{
public:
	CDTComm();
	virtual ~CDTComm();
	BOOL OpenComm(LPCSTR port, DWORD baud, int parity, int bits, int stopbits);
	BOOL OpenCommU(LPCWSTR port, DWORD baud, int parity, int bits, int stopbits);
	BOOL SetCommPara(DWORD baud, int parity, int bits, int stopbits);
	BOOL SetCommBufferSize(int inbufsize, int outbufsize);
	BOOL CloseComm();
	BOOL PurgeComm(DWORD flag);
	BOOL ClearCommError(LPDWORD lpErrors, LPCOMSTAT lpStat){return ::ClearCommError(Handle,lpErrors,lpStat);}

	BOOL SetRTS(BOOL state);
	BOOL GetRTS(BOOL & state);

	DWORD WriteComm(BYTE * pdata, DWORD datalen);
	DWORD ReadComm(BYTE * buffer, DWORD readcount);
	DWORD ReadComm(PBYTE buffer, DWORD readcount, BOOL * breakflag);



	BOOL IsOpen();
	HANDLE Handle;

protected:
	DWORD OnByteTakeTimeMicroSecond;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <WINSOCK2.H>
#ifdef _WIN32_WCE
#pragma comment(lib ,"Ws2.lib")
#else
#pragma comment(lib ,"WS2_32.LIB")
#endif


/*
CDTSocket 封装WINSOCK套接字的通信
用于WIFI通信
add by:Fanze (08.12.08)

*/
class CDTSocket  
{
public:
	CDTSocket();
	virtual ~CDTSocket();
	
	SOCKET m_Socket ; //套接字
	BOOL m_bInit ; //是否初始化
	BOOL m_bConnect ; //是否已连接

	BOOL IsConnected() ;
	//初始化网络
	BOOL InitNetWork() ;
	//连接,szIP 是目标主机的IP地址 nPort是端口
	BOOL Connect(const char *szIP ,UINT nPort) ;
	
	//发送 buf数据缓冲区，nSize 缓冲区大小
	int SendBuf(LPVOID buf ,int nSize) ;//发送
	//同上
	int RcvBuf(LPVOID buf ,int nSize) ;//接受
	//断开连接
	VOID CloseConnect() ;
	
};



#endif
