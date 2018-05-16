///////////////////////////////////////////////////////////////////////////////////////
// Windows Serial Port Operatings
// DENGTING

// 2007.10.2
// 2007.01.25
///////////////////////////////////////////////////////////////////////////////////////
#ifndef H_DT_COMM_MODULE
#define H_DT_COMM_MODULE

#define COMM_DEFAULT_INPUT_BUFFER_SIZE	4096	//���ڻ���512��Ϊ4096 RTT/20081023
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
CDTSocket ��װWINSOCK�׽��ֵ�ͨ��
����WIFIͨ��
add by:Fanze (08.12.08)

*/
class CDTSocket  
{
public:
	CDTSocket();
	virtual ~CDTSocket();
	
	SOCKET m_Socket ; //�׽���
	BOOL m_bInit ; //�Ƿ��ʼ��
	BOOL m_bConnect ; //�Ƿ�������

	BOOL IsConnected() ;
	//��ʼ������
	BOOL InitNetWork() ;
	//����,szIP ��Ŀ��������IP��ַ nPort�Ƕ˿�
	BOOL Connect(const char *szIP ,UINT nPort) ;
	
	//���� buf���ݻ�������nSize ��������С
	int SendBuf(LPVOID buf ,int nSize) ;//����
	//ͬ��
	int RcvBuf(LPVOID buf ,int nSize) ;//����
	//�Ͽ�����
	VOID CloseConnect() ;
	
};



#endif
