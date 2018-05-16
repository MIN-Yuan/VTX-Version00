// VirtualCarWnd.h : header file
//

#if !defined(AFX_VIRTUALCARWND_H__4EE5B99D_B233_49D5_AAE8_BA7EDDD9EEC1__INCLUDED_)
#define AFX_VIRTUALCARWND_H__4EE5B99D_B233_49D5_AAE8_BA7EDDD9EEC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVirtualCarWnd dialog

#include "DTExpex.h"
#include "DTComm.h"
#include "DTXML.h"

#include "MyListCtrl.h"

extern BOOL GModifyInitAlways;


extern const char * STR_APP_TITLE;
extern const char * STR_APP_CONFIG_FILE;
extern const char * STR_XML_FILE_INFO;

extern const char * STR_CONTENT;
extern const char * STR_COMMS;
extern const char * STR_DATA;
extern const char * STR_COMM;
extern const char * STR_CONFIG;
extern const char * STR_TITLE;
extern const char * STR_LOGFILE;
extern const char * STR_LOGENABLE;
extern const char * STR_FILTERSELFSENT;
extern const char * STR_THREADPRIORITY;
extern const char * STR_SLEEP;
extern const char * STR_WATCHLIST;
extern const char * STR_PORT;
extern const char * STR_SPEED;
extern const char * STR_PARITY;
extern const char * STR_BITS;
extern const char * STR_STOPBITS;
extern const char * STR_FRAMETYPE;
extern const char * STR_MINFRAMESPACE;
extern const char * STR_DELAYREALTIME;
extern const char * STR_SENDDELAY;
extern const char * STR_RECEIVEDELAY;
extern const char * STR_CHECKENABLE;
extern const char * STR_CHECKTYPE;
extern const char * STR_CHECKBEGIN;
extern const char * STR_CHECKEND;
extern const char * STR_CHECKTARGET;
extern const char * STR_CHECKSYMBOL;

extern const char * STR_AUTOREPLYENABLE;
extern const char * STR_AUTOREPLYSPACE;
extern const char * STR_AUTOREPLYID;
extern const char * STR_MANUALDATA;
extern const char * STR_REPLYBYMANUAL;

extern const char * STR_SEND;
extern const char * STR_RECEIVE;
extern const char * STR_TYPE;		//RTT/20081023
extern const char * STR_BREAK;
extern const char * STR_COUNT;
extern const char * STR_POS;
extern const char * STR_YX;
extern const char * STR_REMARK;
extern const char * STR_SYMBOL;
extern const char * STR_ACT;
extern const char * STR_SHOW;
extern const char * STR_ENABLE;
extern const char * STR_00;

extern const char * STR_DISABLESEND;
extern const char * STR_DISABLESENDSYMBOL;

#define FRAME_TYPE_MINIFRAMESPACE 0
#define FRAME_TYPE_KT600			1
#define FRAME_TYPE_VCX			2
#define FRAME_TYPE_VCXPLUS		3

typedef enum ENUM_REV_THREAD_STATE{
	RTS_NONE		=0,
	RTS_RUNNING		=1,
	RTS_EXSITING	=2,
	RTS_PAUSE		=3
}REV_THREAD_STATE;


DWORD WINAPI ThreadCheck(VOID * p);

#define SEND_TYPE_NORMAL	0	//普通顺序发送
#define SEND_TYPE_ORDINAL	1	//循环顺序发送
#define SEND_TYPE_RANDOM	2	//随机发送

#define	SEND_BREAK_NORMAL	0	//
#define	SEND_BREAK_BREAK	1	//

//#define COLUMN_WHO			0
#define COLUMN_DATA			0
#define COLUMN_DESCRIPTION	1
#define	COLUMN_TYPE			2
#define COLUMN_TIME			3

//Imagelist ID.
#define ID_IMG_NOTE		0
#define ID_IMG_CAR		1
#define ID_IMG_DIAGNOTOR 2
#define ID_IMG_ERR		3
#define ID_IMG_EMPTY	4

#define MAX_COMM_COUNT	1024
#define MAX_SEND_COUNT	8192
#define MAX_CONFIG		16

#define MAX_Y	8

#define ID_WIZARD_CANCEL 0
#define ID_WIZARD_NEXT	1
#define ID_WIZARD_IGNORE 2

#define ID_TIMER_WATCH_CFGFILECHANGE 911

#define WM_CLOSE_TAO	WM_USER+2008
typedef struct STRUCT_COMM
{
	UCHAR	Enable;	//使能
	CString Receive;
	CString Remark;
	UCHAR	Type;	//type :0 、1、2 固定、循环、随即
	UCHAR	Break;	//如果连续发送时收到数据是否打断 RTT/20081023
	UINT	Count;	//count ，与POS配合 ，发送的条数
	UINT	Pos;	//POS， 发送的起始序号
	CString Act;
	UCHAR	Show;	//是否在列表中显示
	INT	Delay;		//接收后延时

	struct STRUCT_COMM_S{
		CString Data;
		CString Remark;
		UCHAR Symbol;
		CString Act;
		UCHAR Check;	//用来暂存校验字
		INT	Delay;		//发送后延时
	}Send[MAX_SEND_COUNT];
	UCHAR	SCount;	//SEND的数目

	CString Y[MAX_Y];	//用简单表达式
}COMM;

#define CHECK_TYPE_XOR				0			//
#define CHECK_TYPE_SUM				1
#define CHECK_TYPE_SUM_ANTI			2			//和取反
#define CHECK_TYPE_SUM_ANTI1		6			//和取反+1
#define CHECK_TYPE_2BYTESUM_HIGHLOW 3
#define CHECK_TYPE_2BYTESUM_LOWHIGH 4
#define CHECK_TYPE_J1850CRC8		5

extern	COMM	Comm[MAX_COMM_COUNT];
extern	UINT	CommCount;	//RTT
extern 	CString GConfigFile;


extern BYTE GFrameType;

extern HICON GHIcon;


extern CString GTitle;
extern CString GPort;
extern LONG	GSpeed;
extern BYTE	GParity;
extern BYTE	GStopbits;
extern BYTE	GBits;
extern UINT	GMinFrameSpace;

extern CString GLogFile;

extern unsigned long GAllSendDelay;
extern unsigned long GAllReceiveDelay;
extern UCHAR GDelayRealtime;
extern UINT GAutoReply100CPU;


extern CString GWizardXMLFileName;
extern INT GWizardDlgResult;
extern INT GWizardConfigID;
extern INT GWizardCommID;


extern DWORD GSleep;
extern INT	GThreadPriority;
extern BOOL GLogFileEnable;

extern CString GYHex[];


const char * ParityValue2Description(INT parity);
const char * StopBitsValue2Description(INT stopbits);

double GetTickCountEx();
BOOL IsFileExist(LPCTSTR lpFileName);

INT PrioritySliderValue2APIValue(INT slider);
LPCSTR PrioritySliderValue2Description(INT slider);
BOOL GetPathFromFileName(CString & outpath, const char * filename);
void TrimToPureHexString(CString & str);
BOOL ConvertOldConfigFile(CString file);
BOOL ConvertKiaConfigFile(CString file1, CString file2);

CHAR CalcYResult(CString & outYX, CString exstr, BYTE * rcvbuf, INT rcvbufcnt);
void ProcessRefrenceYX(CString & str);

VOID AddA5A5ForFileFmt(CString & hexstr);
VOID RemoveA5A5ForFileFmt(CString & hexstr);


typedef struct STRUCT_MSG_FORLIST{
	INT img_id;
	CString str0;
	CString str1;
	CString str2;
	CString str3;
}MSG_FORLIST;
#define MAX_MSG_CACHE_COUNT 255


extern CDTComm GComm;

extern REV_THREAD_STATE GRTS;	//接收线程运行状态
//extern BYTE GCommBreakFlag;


class CVirtualCarWnd : public CDialog
{
// Construction
public:
	CVirtualCarWnd(CWnd* pParent = NULL);	// standard constructor

//Dengting's Methods
	HANDLE ProgramMutex;

	BOOL m_LogTime;
	

	VOID AddMsg(INT img_id, char * str0, char * str1, char * str2, char *str3);

	VOID WatchSerialPortState();
	VOID WatchCurConfigFileChange();

	//自动Load
	BOOL m_AutoLoad;
	CFile m_fileForAutoLoad;
	LONG m_cfglastsize;
	CTime m_cfglasttime;


	CMenu Menu;

    void OnMini();

	//for log file
	HANDLE HLogFile;
	char LogToFile(char diag0car1, char * str);

	//char ResetPos();
	//BOOL GetLastConfigFile(CString & file);
	//BOOL SetLastConfigFile(CHAR * file);
	//char MatchCondition(char * str);

	char ExecuteAct(CString & act);

	//Control the input buffer textbox...
	char InputBufferAdd(BYTE b);
	char InputBufferSet(PBYTE p, UINT c);
	char InputBufferClean();

	char LoadConfig(LPCSTR filename,INT configid, CHAR reopencomm, CHAR reloadcomm);
	char LoadComm(LPCSTR filename);

	char RemoveCommDataFrameHead();	//去除C中的02 01

	char ProcessFrame(BYTE * buffer, UINT len);
	char ResponseFrame(UINT id);
	void ReplaceCheckOut(BYTE * bytebuf, INT bytebuflen);

	void SendTempData(CString & str);
	LPCSTR GetThreadPriorityDescription(int priority);

	CImageList m_images;

	HICON	HNote;
	HICON	HCar;
	HICON	HDiagnotor;
	HICON	HErr;

	//char		ThreadEndFlag;
	HANDLE		HThread;
	ULONG		ThreadID;


	//CString FrameHead;
	//CString FrameTail;

	

	BYTE m_ReplacePosNum1;
	BYTE m_ReplacePosNum2;

	BYTE m_ReplaceToNum1;
	BYTE m_ReplaceToNum2;


	//For Icon
	CMenu TaskBarIconMenu;

// Dialog Data
	//{{AFX_DATA(CVirtualCarWnd)
	enum { IDD = IDD_VIRTUALCAR_DIALOG };
	CProgressCtrl	m_ProgressOutput;
	CProgressCtrl	m_ProgressInput;
	CSliderCtrl	m_SliderPriority;
	CSliderCtrl	m_SliderSleep;
	CMyListCtrl	m_list;
	CString	m_PortState;
	BOOL	m_ShowCommErr;
	BOOL	m_CheckReplaceSendData;
	CString	m_ReplacePos1;
	CString	m_ReplacePos2;
	CString	m_ReplaceTo1;
	CString	m_ReplaceTo2;
	UINT	m_ReplaceCheckSymbol;
	UINT	m_ReplaceSymbol;
	BOOL	m_ReplaceCheck;
	int		m_ReplaceCheckType;
	int		m_CheckOutBegin;
	int		m_CheckOutEnd;
	int		m_CheckOutTarget;
	BOOL	m_CheckReplaceSendData2;
	UINT	m_ReplaceSymbol2;
	CString	m_SleepCaption;
	CString	m_PriorityCaption;
	int		m_FrameType;
	UINT	m_DisableRepaySymbol;
	BOOL	m_CheckDisableRepay;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVirtualCarWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVirtualCarWnd)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnMenuitemClose();
	afx_msg void OnMenuitemAbout();
	afx_msg void OnMenuitemEditConfig();
	afx_msg void OnMenuitemViewLog();
	afx_msg void OnMenuitemClearList();
	afx_msg void OnMenuitemLoadConfig();
	afx_msg void OnCheckWatchList();
	afx_msg void OnCheckLogFile();
	afx_msg void OnMenuitemReloadConfig();
	afx_msg void OnCheckRemoveSelfSend();
	afx_msg void OnButtonSend();
	afx_msg void OnCheckAutoReplyByTemp();
	afx_msg void OnRadioTimerFile();
	afx_msg void OnRadioTimerTemp();
	afx_msg void OnMenuitemCleanLog();
	afx_msg void OnMenuitemLogTime();
	afx_msg void OnOutofmemorySliderSleep(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderSleep(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderPriority(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonCleanList();
	afx_msg void OnButtonCleanLog();
	afx_msg void OnMenuitemLogopen();
	afx_msg void OnMenuitemWatchList();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonReload();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCheckShowCommErr();
	afx_msg void OnCheckReplaceSendData();
	afx_msg void OnOutofmemorySpinPos1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinPos1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinPos2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinTo1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinTo2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckTimerReply();
	afx_msg void OnEditchangeComboReplaceCheckType();
	afx_msg void OnSelchangeComboReplaceCheckType();
	afx_msg void OnChangeEditReplaceSymbol();
	afx_msg void OnChangeEditReplaceCheckSymbol();
	afx_msg void OnCheckReplaceCheck();
	afx_msg void OnButtonTao();
	afx_msg void OnMenuitemAutoload();
	afx_msg void OnMenuitemToolTao();
	afx_msg void OnMenuitemToolUltraeditor();
	afx_msg void OnMenuitemNewConfig();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnCheckReplaceSendData2();
	afx_msg void OnMenuitemParas();
	afx_msg void OnMenuitemFilterSelfSent();
	afx_msg void OnKillfocusEditAutoReplySpace();
	afx_msg void OnMenuitemModifyAppConfig();
	afx_msg void OnMenuitemImportComm();
	afx_msg void OnMenuitemInsertCfg();
	afx_msg void OnMenuitemConvertVirtualcar1();
	afx_msg void OnMenuitemConvertKia();
	afx_msg void OnKillfocusEditCheckoutBegin();
	afx_msg void OnKillfocusEditCheckoutEnd();
	afx_msg void OnKillfocusEditCheckoutTarget();
	afx_msg void OnKillfocusEditReplaceCheckSymbol();
	afx_msg void OnKillfocusEditAutoReplyId();
	afx_msg void OnKillfocusEditReplacePos1();
	afx_msg void OnKillfocusEditReplaceTo1();
	afx_msg void OnKillfocusEditReplaceTo2();
	afx_msg void OnKillfocusEditReplacePos2();
	afx_msg void OnMenuitemAdda5a5();
	afx_msg void OnMenuitemRemovea5a5();
	afx_msg void OnEditchangeComboFrameType();
	afx_msg void OnSelchangeComboFrameType();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuitemPause();
	afx_msg void OnCheckPause();
	afx_msg void OnButtonClearSend();
	afx_msg void OnMenuitemClearInbuffer();
	afx_msg void OnMenuitemClearOutbuffer();
	afx_msg void OnStaticInput();
	afx_msg void OnStaticOutput();
	afx_msg void OnMenuitemCopyall();
	afx_msg void OnButtonSendComm();
	afx_msg void OnCloseTao() ;
	//}}AFX_MSG
	afx_msg LRESULT OnNc(WPARAM wParam,LPARAM IParam);  // 最小化窗口的参数
	DECLARE_MESSAGE_MAP()
};


extern CVirtualCarWnd * PVirtualCarWnd;

VOID ReceiveForMiniFrameSpace(CVirtualCarWnd * win);
VOID ReceiveForVCXFrame(CVirtualCarWnd * win);
VOID ReceiveForKT600Frame(CVirtualCarWnd * win);
VOID ReceiveForVCXPlusFrame(CVirtualCarWnd* win) ;
VOID CheckForAutoReply(CVirtualCarWnd * win);

INT KT600Pack(PBYTE frame, INT length, PBYTE pbuffer, INT buffersize);
INT VCXPack(PBYTE frame, INT length, PBYTE pbuffer, INT buffersize);
INT VCXPlusPack(PBYTE frame, INT length, PBYTE pbuffer, INT buffersize) ;
DWORD CalcSum(BYTE * buffer, DWORD size);


VOID CopyTextToClipboard(HWND owner, LPCSTR str);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRTUALCARWND_H__4EE5B99D_B233_49D5_AAE8_BA7EDDD9EEC1__INCLUDED_)
