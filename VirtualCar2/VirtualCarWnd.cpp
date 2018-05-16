#include "stdafx.h"
#include "VirtualCar.h"
#include "VirtualCarWnd.h"
#include "base.h"
#include "DlgAbout.h"
#include "resource.h"
#include "stdio.h"
#include <io.h>
#include <atlbase.h>
#include "ModifyWnd.h"
#include "DlgParas.h"

#include "DlgWizardNewFile.h"
#include "DlgWizardTitle.h"
#include "DlgWizardPerfomance.h"
#include "DlgWizardCOM.h"
#include "DlgWizardFrame.h"
#include "DlgWizardCheck.h"
#include "DlgWizardTimer.h"
#include "DlgWizardComm.h"

#include "DlgSend.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDTComm GComm;

#define VCX_FRAME_MARK		0xbb /** 帧标记 */   
#define VCX_FRAME_XOR       0xff /** 转义转换时异或字节 */
#define VCX_FRAME_TRANS2    0xdd /** 转义字节 */
#define VCX_FRAME_FMT		0x80
///////////////////////////////////////////////////////////////////////////////////

const char * STR_APP_TITLE ="Virtual Car II";
const char * STR_APP_CONFIG_FILE="VirtualCar.xml";
const char * STR_XML_FILE_INFO="<?xml version=\"1.0\" encoding=\"gb2312\" ?>\r\n";


const char * STR_CONTENT="CONTENT";
const char * STR_COMMS="COMMS";
const char * STR_DATA="DATA";
const char * STR_COMM="COMM";
const char * STR_CONFIG="CONFIG";
const char * STR_TITLE="TITLE";
const char * STR_LOGFILE="LOGFILE";
const char * STR_LOGENABLE="LOGENABLE";
const char * STR_FILTERSELFSENT="FILTERSELFSENT";
const char * STR_THREADPRIORITY="THREADPRIORITY";
const char * STR_SLEEP="SLEEP";
const char * STR_WATCHLIST="WATCHLIST";
const char * STR_PORT="PORT";
const char * STR_SPEED="SPEED";
const char * STR_PARITY="PARITY";
const char * STR_BITS="BITS";
const char * STR_STOPBITS="STOPBITS";
const char * STR_FRAMETYPE="FRAMETYPE";
const char * STR_MINFRAMESPACE="MINFRAMESPACE";
const char * STR_DELAYREALTIME="DELAYREALTIME";
const char * STR_SENDDELAY="SENDDELAY";
const char * STR_RECEIVEDELAY="RECEIVEDELAY";
const char * STR_CHECKENABLE="CHECKENABLE";
const char * STR_CHECKTYPE="CHECKTYPE";
const char * STR_CHECKBEGIN="CHECKBEGIN";
const char * STR_CHECKEND="CHECKEND";
const char * STR_CHECKTARGET="CHECKTARGET";
const char * STR_CHECKSYMBOL="CHECKSYMBOL";

const char * STR_AUTOREPLYENABLE="AUTOREPLYENABLE";
const char * STR_AUTOREPLYSPACE="AUTOREPLYSPACE";
const char * STR_AUTOREPLYID="AUTOREPLYID";
const char * STR_MANUALDATA="MANUALDATA";
const char * STR_REPLYBYMANUAL="REPLYBYMANUAL";

const char * STR_SEND="SEND";
const char * STR_RECEIVE="RECEIVE";
const char * STR_TYPE="TYPE";	//RTT/20081023
const char * STR_BREAK="BREAK";
const char * STR_COUNT="COUNT";
const char * STR_POS="POS";
const char * STR_YX="Y%d";
const char * STR_REMARK="REMARK";
const char * STR_SYMBOL="SYMBOL";
const char * STR_ACT="ACT";
const char * STR_SHOW="SHOW";
const char * STR_ENABLE="ENABLE";
const char * STR_00="00";

const char * STR_DELAY="DELAY";
const char * STR_DISABLESEND="DISABLESEND";
const char * STR_DISABLESENDSYMBOL="DISABLESENDSYMBOL";

CVirtualCarWnd * PVirtualCarWnd=0;
BOOL GModifyInitAlways=0;	//Modify窗口的Allways钩上.



REV_THREAD_STATE GRTS;	//接收线程运行状态
//BYTE GCommBreakFlag=0;


COMM	Comm[MAX_COMM_COUNT];
UINT	CommCount;		//RTT


// 最小化窗口的参数
#define  WM_NC  1001
#define  IDC_NC 1002


#define MAX_IN_SIZE 8192
BYTE GInBuffer[MAX_IN_SIZE+16];
UINT GInBufferCount=0;


HICON GHIcon;


CString GConfigFile;


UCHAR GAutoReplyEnable;
UCHAR GAutoReplyID;
UINT  GAutoReplySpace;


BYTE GFrameType;
UCHAR GFilterSelfSent;
UCHAR GAutoReplyByTemp;
UCHAR GWatchList;
BYTE GPause=FALSE;

double GLastReceiveByteTime=0;	//用于接收帧
double GAutoreplyLastTime=-720000;	//用于处理定时发送


unsigned long GAllSendDelay=0;
unsigned long GAllReceiveDelay=0;
UCHAR GDelayRealtime=0;


CDTExpEx GEx;


//DWORD lastframetime;	//用于记录最后帧时间

double GLastProcessFrameTimetime;	//用于显示时间列
INT GMaxListRowCount;

CString GTitle;
CString GPort;
LONG	GSpeed;
BYTE	GParity;
BYTE	GStopbits;
BYTE	GBits;
UINT	GMinFrameSpace;

CString GLogFile;

LONGLONG GFrequency;
LARGE_INTEGER Gli;
double GTempTickcount;	//用于暂存count值

UINT GAutoReply100CPU;

DWORD GSleep;
INT	GThreadPriority;

BOOL GLogFileEnable;


CString GWizardXMLFileName;	//用于在各个DLG中共享文件名。
INT GWizardDlgResult;
INT GWizardConfigID;	//向导中，操作哪一个<CONFIG>
//INT GWizardCommID;		//向导中，操作哪一个<COMM>

CString GYHex[MAX_Y];	//Like "00" "80"


CString GUltraEditorPath;

double GetTickCountEx()
{
	QueryPerformanceCounter(&Gli);
	return (double)1000*Gli.QuadPart/GFrequency;
}


INT PrioritySliderValue2APIValue(INT slider)
{
	switch(slider){
	case -3:
		return THREAD_PRIORITY_TIME_CRITICAL;
	case -2:
		return THREAD_PRIORITY_HIGHEST;
	case -1:
		return THREAD_PRIORITY_ABOVE_NORMAL;
	case 0:
		return THREAD_PRIORITY_NORMAL;
	case 1:
		return THREAD_PRIORITY_BELOW_NORMAL;
	case 2:
		return THREAD_PRIORITY_LOWEST;
	case 3:
		return THREAD_PRIORITY_IDLE;
	default:
		return THREAD_PRIORITY_NORMAL;
	}

}

LPCSTR PrioritySliderValue2Description(INT slider)
{
	switch(slider){
	case -3:
		return "实时";
	case -2:
		return "最高";
	case -1:
		return "较高";
	case 0:
		return "普通";
	case 1:
		return "较低";
	case 2:
		return "最低";
	case 3:
		return "空闲";
	default:
		return "错误";
	}

}

BOOL GetPathFromFileName(CString & outpath, const char * filename)
{
	int pos;
	CString str;

	str=filename;
	pos=str.ReverseFind('\\');
	if(pos==-1){
		return FALSE;
	}
	
	outpath=str.Left(pos+1);

	return TRUE;
}

const char * ParityValue2Description(INT parity)
{
	switch(parity){
	case 0:
		return "无校验";
	case 1:
		return "奇校验";
	case 2:
		return "偶校验";
	case 3:
		return "标志";
	case 4:
		return "空间隔";
	default:
		return "校错误";
	}
}

const char * StopBitsValue2Description(INT stopbits)
{
	switch(stopbits){
	case 0:
		return "1";
	case 1:
		return "1.5";
	case 2:
		return "2";
	default:
		return "?";
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVirtualCarWnd dialog

CVirtualCarWnd::CVirtualCarWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CVirtualCarWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVirtualCarWnd)
	m_PortState = _T("");
	m_ShowCommErr = TRUE;
	m_CheckReplaceSendData = FALSE;
	m_ReplacePos1 = _T("");
	m_ReplacePos2 = _T("");
	m_ReplaceTo1 = _T("");
	m_ReplaceTo2 = _T("");
	m_ReplaceCheckSymbol = 0;
	m_ReplaceSymbol = 0;
	m_ReplaceCheck = FALSE;
	m_ReplaceCheckType = CHECK_TYPE_SUM;
	m_CheckOutBegin = 0;
	m_CheckOutEnd = 0;
	m_CheckOutTarget = 0;
	m_CheckReplaceSendData2 = FALSE;
	m_ReplaceSymbol2 = 0;
	m_SleepCaption = _T("");
	m_PriorityCaption = _T("");
	m_FrameType = -1;
	m_DisableRepaySymbol = 0;
	m_CheckDisableRepay = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	GHIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_LogTime=1;

	m_ReplaceToNum1=0x01;
	m_ReplaceToNum2=0x02;
	m_ReplaceTo1.Format("%02x",m_ReplaceToNum1);
	m_ReplaceTo2.Format("%02x",m_ReplaceToNum2);

	m_ReplacePosNum1=0x07;
	m_ReplacePosNum2=0x08;
	m_ReplacePos1.Format("%02x",m_ReplacePosNum1);
	m_ReplacePos2.Format("%02x",m_ReplacePosNum2);


	m_cfglastsize=0;
	HLogFile=INVALID_HANDLE_VALUE;
	HThread=NULL;
	TaskBarIconMenu.LoadMenu(IDR_MENU_POPUP);

}

void TrimToPureHexString(CString &str)
{
	str.MakeUpper();

	int i,len;

	for(i=1; i<256; i++){
		if((i>='A' && i<='F') || i>='0' && i<='9'){

		}else{
			str.Remove((TCHAR)i);
		}
	}


	//如果是奇数,加一个0
	if(str.GetLength()%2){
		str+="0";
	}

	//插入空格
	len=str.GetLength();
	for(i=len-2; i>0; i-=2){
		str.Insert(i, " ");
	}

}

char GetUltraEditorPath(char *buf, int bufsize)
{
	CRegKey reg;
	DWORD len=bufsize;

	buf[0]=0;


	if(GUltraEditorPath.GetLength()>4){
		if(IsFileExist(GUltraEditorPath)){
			if(GUltraEditorPath.GetLength()<bufsize-1){
				strcpy(buf,GUltraEditorPath.GetBuffer(MAX_PATH));
				return 1;
			}
		}
	}

	

	reg.Open(HKEY_CLASSES_ROOT,"CLSID\\{b5eedee0-c06e-11cf-8c56-444553540000}\\InProcServer32");
	reg.QueryValue(buf,"",&len);
	reg.Close();

	if(strlen(buf)<=13){
		return 0;
	}
	
	buf[len-13]=0;

	strcat(buf,"Uedit32.exe");

	return 1;

}

void CVirtualCarWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CVirtualCarWnd)
	DDX_Control(pDX, IDC_PROGRESS_OUTPUT, m_ProgressOutput);
	DDX_Control(pDX, IDC_PROGRESS_INPUT, m_ProgressInput);
	DDX_Control(pDX, IDC_SLIDER_PRIORITY, m_SliderPriority);
	DDX_Control(pDX, IDC_SLIDER_SLEEP, m_SliderSleep);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_EDIT_PORT_STATE, m_PortState);
	DDX_Check(pDX, IDC_CHECK_SHOW_COMM_ERR, m_ShowCommErr);
	DDX_Check(pDX, IDC_CHECK_REPLACE_SEND_DATA, m_CheckReplaceSendData);
	DDX_Text(pDX, IDC_EDIT_REPLACE_POS1, m_ReplacePos1);
	DDV_MaxChars(pDX, m_ReplacePos1, 2);
	DDX_Text(pDX, IDC_EDIT_REPLACE_POS2, m_ReplacePos2);
	DDV_MaxChars(pDX, m_ReplacePos2, 2);
	DDX_Text(pDX, IDC_EDIT_REPLACE_TO1, m_ReplaceTo1);
	DDV_MaxChars(pDX, m_ReplaceTo1, 2);
	DDX_Text(pDX, IDC_EDIT_REPLACE_TO2, m_ReplaceTo2);
	DDV_MaxChars(pDX, m_ReplaceTo2, 2);
	DDX_Text(pDX, IDC_EDIT_REPLACE_CHECK_SYMBOL, m_ReplaceCheckSymbol);
	DDX_Text(pDX, IDC_EDIT_REPLACE_SYMBOL, m_ReplaceSymbol);
	DDX_Check(pDX, IDC_CHECK_REPLACE_CHECK, m_ReplaceCheck);
	DDX_CBIndex(pDX, IDC_COMBO_REPLACE_CHECK_TYPE, m_ReplaceCheckType);
	DDX_Text(pDX, IDC_EDIT_CHECKOUT_BEGIN, m_CheckOutBegin);
	DDX_Text(pDX, IDC_EDIT_CHECKOUT_END, m_CheckOutEnd);
	DDX_Text(pDX, IDC_EDIT_CHECKOUT_TARGET, m_CheckOutTarget);
	DDX_Check(pDX, IDC_CHECK_REPLACE_SEND_DATA2, m_CheckReplaceSendData2);
	DDX_Text(pDX, IDC_EDIT_REPLACE_SYMBOL2, m_ReplaceSymbol2);
	DDX_Text(pDX, IDC_EDIT_SLEEP, m_SleepCaption);
	DDX_Text(pDX, IDC_EDIT_PRIORITY, m_PriorityCaption);
	DDX_CBIndex(pDX, IDC_COMBO_FRAME_TYPE, m_FrameType);
	DDX_Text(pDX, IDC_EDIT_DISABLE_REPAY_SYMBOL, m_DisableRepaySymbol);
	DDX_Check(pDX, IDC_CHECK_DISABLE_REPAY, m_CheckDisableRepay);
	//}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CVirtualCarWnd, CDialog)
	//{{AFX_MSG_MAP(CVirtualCarWnd)
	ON_WM_SYSCOMMAND( )
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_KEYUP()
	ON_WM_DESTROY()
	ON_COMMAND(ID_MENUITEM_CLOSE, OnMenuitemClose)
	ON_COMMAND(ID_MENUITEM_ABOUT, OnMenuitemAbout)
	ON_COMMAND(ID_MENUITEM_EDIT_CONFIG, OnMenuitemEditConfig)
	ON_COMMAND(ID_MENUITEM_VIEW_LOG, OnMenuitemViewLog)
	ON_COMMAND(ID_MENUITEM_CLEAR_LIST, OnMenuitemClearList)
	ON_COMMAND(ID_MENUITEM_LOAD_CONFIG, OnMenuitemLoadConfig)
	ON_BN_CLICKED(IDC_CHECK_WATCH_LIST, OnCheckWatchList)
	ON_BN_CLICKED(IDC_CHECK_LOG_FILE, OnCheckLogFile)
	ON_COMMAND(ID_MENUITEM_RELOAD_CONFIG, OnMenuitemReloadConfig)
	ON_BN_CLICKED(IDC_CHECK_REMOVE_SELF_SEND, OnCheckRemoveSelfSend)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_BN_CLICKED(IDC_CHECK_AUTO_REPLY_BY_TEMP, OnCheckAutoReplyByTemp)
	ON_BN_CLICKED(IDC_RADIO_TIMER_FILE, OnRadioTimerFile)
	ON_BN_CLICKED(IDC_RADIO_TIMER_TEMP, OnRadioTimerTemp)
	ON_COMMAND(ID_MENUITEM_CLEAN_LOG, OnMenuitemCleanLog)
	ON_COMMAND(ID_MENUITEM_LOG_TIME, OnMenuitemLogTime)
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SLIDER_SLEEP, OnOutofmemorySliderSleep)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SLEEP, OnReleasedcaptureSliderSleep)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PRIORITY, OnReleasedcaptureSliderPriority)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN_LIST, OnButtonCleanList)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN_LOG, OnButtonCleanLog)
	ON_COMMAND(ID_MENUITEM_LOGOPEN, OnMenuitemLogopen)
	ON_COMMAND(ID_MENUITEM_WATCH_LIST, OnMenuitemWatchList)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, OnButtonReload)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_SHOW_COMM_ERR, OnCheckShowCommErr)
	ON_BN_CLICKED(IDC_CHECK_REPLACE_SEND_DATA, OnCheckReplaceSendData)
	ON_NOTIFY(NM_OUTOFMEMORY, IDC_SPIN_POS1, OnOutofmemorySpinPos1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POS1, OnDeltaposSpinPos1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POS2, OnDeltaposSpinPos2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TO1, OnDeltaposSpinTo1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TO2, OnDeltaposSpinTo2)
	ON_BN_CLICKED(IDC_CHECK_TIMER_REPLY, OnCheckTimerReply)
	ON_CBN_EDITCHANGE(IDC_COMBO_REPLACE_CHECK_TYPE, OnEditchangeComboReplaceCheckType)
	ON_CBN_SELCHANGE(IDC_COMBO_REPLACE_CHECK_TYPE, OnSelchangeComboReplaceCheckType)
	ON_EN_CHANGE(IDC_EDIT_REPLACE_SYMBOL, OnChangeEditReplaceSymbol)
	ON_BN_CLICKED(IDC_CHECK_REPLACE_CHECK, OnCheckReplaceCheck)
	ON_BN_CLICKED(IDC_BUTTON_TAO, OnButtonTao)
	ON_COMMAND(ID_MENUITEM_AUTOLOAD, OnMenuitemAutoload)
	ON_COMMAND(ID_MENUITEM_TOOL_TAO, OnMenuitemToolTao)
	ON_COMMAND(ID_MENUITEM_TOOL_ULTRAEDITOR, OnMenuitemToolUltraeditor)
	ON_COMMAND(ID_MENUITEM_NEW_CONFIG, OnMenuitemNewConfig)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_CHECK_REPLACE_SEND_DATA2, OnCheckReplaceSendData2)
	ON_COMMAND(ID_MENUITEM_PARAS, OnMenuitemParas)
	ON_COMMAND(ID_MENUITEM_FILTER_SELF_SENT, OnMenuitemFilterSelfSent)
	ON_EN_KILLFOCUS(IDC_EDIT_AUTO_REPLY_SPACE, OnKillfocusEditAutoReplySpace)
	ON_COMMAND(ID_MENUITEM_MODIFY_APP_CONFIG, OnMenuitemModifyAppConfig)
	ON_COMMAND(ID_MENUITEM_IMPORT_COMM, OnMenuitemImportComm)
	ON_COMMAND(ID_MENUITEM_INSERT_CFG, OnMenuitemInsertCfg)
	ON_COMMAND(ID_MENUITEM_CONVERT_VIRTUALCAR1, OnMenuitemConvertVirtualcar1)
	ON_COMMAND(ID_MENUITEM_CONVERT_KIA, OnMenuitemConvertKia)
	ON_EN_KILLFOCUS(IDC_EDIT_CHECKOUT_BEGIN, OnKillfocusEditCheckoutBegin)
	ON_EN_KILLFOCUS(IDC_EDIT_CHECKOUT_END, OnKillfocusEditCheckoutEnd)
	ON_EN_KILLFOCUS(IDC_EDIT_CHECKOUT_TARGET, OnKillfocusEditCheckoutTarget)
	ON_EN_KILLFOCUS(IDC_EDIT_REPLACE_CHECK_SYMBOL, OnKillfocusEditReplaceCheckSymbol)
	ON_EN_KILLFOCUS(IDC_EDIT_AUTO_REPLY_ID, OnKillfocusEditAutoReplyId)
	ON_EN_KILLFOCUS(IDC_EDIT_REPLACE_POS1, OnKillfocusEditReplacePos1)
	ON_EN_KILLFOCUS(IDC_EDIT_REPLACE_TO1, OnKillfocusEditReplaceTo1)
	ON_EN_KILLFOCUS(IDC_EDIT_REPLACE_TO2, OnKillfocusEditReplaceTo2)
	ON_EN_KILLFOCUS(IDC_EDIT_REPLACE_POS2, OnKillfocusEditReplacePos2)
	ON_COMMAND(ID_MENUITEM_ADDA5A5, OnMenuitemAdda5a5)
	ON_COMMAND(ID_MENUITEM_REMOVEA5A5, OnMenuitemRemovea5a5)
	ON_CBN_EDITCHANGE(IDC_COMBO_FRAME_TYPE, OnEditchangeComboFrameType)
	ON_CBN_SELCHANGE(IDC_COMBO_FRAME_TYPE, OnSelchangeComboFrameType)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_COMMAND(ID_MENUITEM_PAUSE, OnMenuitemPause)
	ON_BN_CLICKED(IDC_CHECK_PAUSE, OnCheckPause)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_SEND, OnButtonClearSend)
	ON_COMMAND(ID_MENUITEM_CLEAR_INBUFFER, OnMenuitemClearInbuffer)
	ON_COMMAND(ID_MENUITEM_CLEAR_OUTBUFFER, OnMenuitemClearOutbuffer)
	ON_BN_CLICKED(IDC_STATIC_INPUT, OnStaticInput)
	ON_BN_CLICKED(IDC_STATIC_OUTPUT, OnStaticOutput)
	ON_COMMAND(ID_MENUITEM_COPYALL, OnMenuitemCopyall)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BUTTON_SEND_COMM, OnButtonSendComm)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NC,OnNc)  //有这句才能响应鼠标消息
END_MESSAGE_MAP()


void CVirtualCarWnd::OnMini()
{
	NOTIFYICONDATA nc;
	nc.cbSize=sizeof(NOTIFYICONDATA);
	nc.hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	nc.hWnd=m_hWnd;
	lstrcpy(nc.szTip,"Virtual Car");
	nc.uCallbackMessage=WM_NC;
	nc.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nc.uID=IDC_NC;
	Shell_NotifyIcon(NIM_ADD,&nc);
	ShowWindow(false);	
    SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | WS_EX_TOPMOST);
}

void CVirtualCarWnd::OnSysCommand(UINT nID, LPARAM lParam)
{	
	CDialog::OnSysCommand(nID, lParam);
	
	if(nID == SC_MINIMIZE)  //如果鼠标点击最小化按钮
	{
        OnMini();
	} 
}

BOOL CVirtualCarWnd::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(GHIcon, TRUE);
	SetIcon(GHIcon, FALSE);
	
	// TODO: Add extra initialization here


	//阻止程序反复驻留内存
	ProgramMutex=CreateMutex(NULL, FALSE, "VirtualCar");
	if(GetLastError()==ERROR_ALREADY_EXISTS){
		MessageBox("Hey! Program already be running.");
		//OnOK();
	}

	
	//初始化GYHex;
	int i;
	for(i=0; i<MAX_Y; i++){
		GYHex[i]=STR_00;
	}


	//在线程启动前，把ModifyWnd上锁
	GModifyWndMutex.Lock();

	
	//Load App Config file
	CDTXML cfg;
	CString path;
	CString modulepath;
	::GetModuleFileName(NULL,modulepath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	modulepath.ReleaseBuffer();
	GetPathFromFileName(path,modulepath.GetBuffer(MAX_PATH));
	path+=STR_APP_CONFIG_FILE;
	if(cfg.Open(path)){
		cfg.Enter("TOP");
		cfg.Enter("MAINWND");
		
		INT MAXIMIZED;
		INT x,y,width,height;
		RECT rect;
		
		MAXIMIZED=cfg.GetInteger("MAXIMIZED",0);
		x=cfg.GetInteger("X",20);
		y=cfg.GetInteger("Y",20);
		width=cfg.GetInteger("WIDTH",940);
		height=cfg.GetInteger("HEIGHT",675);
		GetDesktopWindow()->GetWindowRect(&rect);
		
		//纠正位置
		if(x<0) x=0;
		if(y<0) y=0;
		if(x>rect.right-rect.left) x=rect.right-rect.left-20;
		if(y>rect.bottom-rect.top) y=rect.bottom-rect.top;
		
		if(MAXIMIZED){
			WINDOWPLACEMENT p;
			GetWindowPlacement(&p);
			p.showCmd=SW_SHOWMAXIMIZED;
			SetWindowPlacement(&p);
		}else{
			MoveWindow(x,y,width,height,FALSE);
		}
		
		cfg.Back();
		GMaxListRowCount=cfg.GetInteger("LISTMAXROW",4096);
		GAutoReply100CPU=cfg.GetInteger("AUTOREPLY100CUP",100);
		GConfigFile=cfg.GetString("LASTCONFIGFILE","");
		GModifyInitAlways=cfg.GetInteger("MODIFYALWAYS",0);
		GUltraEditorPath=cfg.GetString("UEPATH","");
		cfg.Close();
	}else{
		CString msg;
		msg.Format("没有找到%s",STR_APP_CONFIG_FILE);
		MessageBox(msg);
		OnOK();
		return 0;
	}




	//Init default log filename
	GLogFile="log.txt";
	HLogFile=INVALID_HANDLE_VALUE;



	//为什么要这里初始化（而不在XML.GetInterger时）？因为重复装载XML里，如果<CONFIG>里缺省时，不改变原来的值。
	GPort="COM2";
	GSpeed=CBR_9600;
	GParity=NOPARITY;
	GStopbits=ONESTOPBIT;
	GBits=8;
	GMinFrameSpace=20;
	GTitle=STR_APP_TITLE;


		
	//Listview设置全行选定
	//m_list.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	GWatchList=TRUE;

	
	//Inti watchlist
	GWatchList=1;
	

	//Init antiselfsent
	GFilterSelfSent=0;

	
	//Init for autorepay
	GAutoReplyEnable=0;
	GAutoReplyID=0;
	GAutoReplySpace=1000;


	//Load main window menu res
	Menu.LoadMenu(MAKEINTRESOURCE(IDR_MENU));
	SetMenu(&Menu);

	//Init AutoLoad
	m_AutoLoad=1;
	if(m_AutoLoad){
		Menu.CheckMenuItem(ID_MENUITEM_AUTOLOAD, MF_CHECKED);
	}else{
		Menu.CheckMenuItem(ID_MENUITEM_AUTOLOAD, MF_UNCHECKED);
	}



	//Init Listview images
	m_images.Create(16,16,ILC_COLORDDB,0,1);
	
	HNote=(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_NOTE),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	m_images.Add(HNote);

	HCar=(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CAR),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	m_images.Add(HCar);

	HDiagnotor=(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_DIAGNOTOR),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	m_images.Add(HDiagnotor);

	HErr=(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ERR),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	m_images.Add(HErr);

	m_list.SetImageList(&m_images,LVSIL_SMALL);


	//Init Listview col titles
	m_list.InsertColumn(COLUMN_DATA,"Data",LVCFMT_LEFT,500,0);
	m_list.InsertColumn(COLUMN_DESCRIPTION,"Remark",LVCFMT_LEFT,80,0);
	m_list.InsertColumn(COLUMN_TYPE,"Type",LVCFMT_LEFT,60,0);
	m_list.InsertColumn(COLUMN_TIME,"Time(ms)",LVCFMT_LEFT,80,0);


	//Init performance GUI
	GSleep=3;	
	m_SliderSleep.SetRange(1,32,TRUE);
	m_SliderSleep.SetPos(GSleep);
	m_SleepCaption.Format("%d",GSleep);

	//Thred Priority
	GThreadPriority=0;	//Normal
	m_SliderPriority.SetRange(-3,3,TRUE);
	m_SliderPriority.SetPos(GThreadPriority);
	m_PriorityCaption=PrioritySliderValue2Description(GThreadPriority);


	//COM in/out buffer ProgressBar
	m_ProgressInput.SetRange32(0,/*-COMM_INPUT_BUFFER_SIZE*0.02,*/COMM_DEFAULT_INPUT_BUFFER_SIZE);
	m_ProgressOutput.SetRange32(0,/*-COMM_OUTPUT_BUFFER_SIZE*0.02,*/COMM_DEFAULT_OUTPUT_BUFFER_SIZE);

	//Init MainWnd title
	SetWindowText(STR_APP_TITLE);


	//初始化非MFC变量的控件
	if(GLogFileEnable){
		CheckMenuItem(Menu,ID_MENUITEM_LOGOPEN,MF_CHECKED);
		CheckDlgButton(IDC_CHECK_LOG_FILE,1);
	}else{
		CheckMenuItem(Menu,ID_MENUITEM_LOGOPEN,MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_LOG_FILE,0);
	}
	if(GWatchList){
		CheckMenuItem(Menu,ID_MENUITEM_WATCH_LIST,MF_CHECKED);
		CheckDlgButton(IDC_CHECK_WATCH_LIST,1);
	}else{
		CheckMenuItem(Menu,ID_MENUITEM_WATCH_LIST,MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_WATCH_LIST,0);
	}
	if(GFilterSelfSent){
		CheckMenuItem(Menu,ID_MENUITEM_FILTER_SELF_SENT,MF_CHECKED);
		CheckDlgButton(IDC_CHECK_REMOVE_SELF_SEND,1);
	}else{
		CheckMenuItem(Menu,ID_MENUITEM_FILTER_SELF_SENT,MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_REMOVE_SELF_SEND,0);
	}



	CString str;
	if(GConfigFile.GetLength()>4){
		if(!LoadConfig(GConfigFile.GetBuffer(255),0,1,1)){
			str.Format("Load %s Failed",GConfigFile);
			AddMsg(ID_IMG_ERR,str.GetBuffer(128),NULL,NULL,NULL);
		}
	}else{
		OnMenuitemLoadConfig();
	}


	//ThreadEndFlag=0;
	GRTS=RTS_NONE;
	HThread=CreateThread(0,0,(LPTHREAD_START_ROUTINE)ThreadCheck,this,0,&ThreadID);
	SetThreadPriority(HThread,PrioritySliderValue2APIValue(GThreadPriority));
	
	UpdateData(FALSE);

	//SetWindowsTimer
	SetTimer(ID_TIMER_WATCH_CFGFILECHANGE,1500,NULL);

	AddMsg(ID_IMG_NOTE,"提示:用鼠标左键LEFT双击单元格，或用鼠标右键RIGHT单击单元格，可复制文本到剪贴板",NULL,NULL,NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVirtualCarWnd::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, GHIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVirtualCarWnd::OnQueryDragIcon()
{
	return (HCURSOR) GHIcon;
}


inline char GenRandHexChar()
{
	switch(rand()%16){
	case 0:
		return '0';
		break;
	case 1:
		return '1';
		break;
	case 2:
		return '2';
		break;
	case 3:
		return '3';
		break;
	case 4:
		return '4';
		break;
	case 5:
		return '5';
		break;
	case 6:
		return '6';
		break;
	case 7:
		return '7';
		break;
	case 8:
		return '8';
		break;
	case 9:
		return '9';
		break;
	case 10:
		return 'A';
		break;
	case 11:
		return 'B';
		break;
	case 12:
		return 'C';
		break;
	case 13:
		return 'D';
		break;
	case 14:
		return 'E';
		break;
	case 15: default:
		return 'F';
		break;
	}
}



char CVirtualCarWnd::LoadConfig(LPCSTR filename, INT configid, CHAR reopencomm, CHAR reloadcomm)
{
	CDTXML Xml;
	char buf[MAX_PATH];
	long l;
	unsigned long outlen;

	if(!Xml.Open(filename)){
		return 0;
	}


	if(!Xml.CheckFile(buf)){
		AddMsg(ID_IMG_ERR,buf,NULL,NULL,NULL);
		return 0;
	}

	if(!Xml.Enter(STR_CONTENT)){
		AddMsg(ID_IMG_ERR,(char*)STR_CONTENT,NULL,NULL,NULL);
		return 0;
	}

	if(!Xml.Enter(STR_CONFIG,configid)){
		AddMsg(ID_IMG_ERR,(char*)STR_CONFIG,NULL,NULL,NULL);
		return 0;
	}

	if(Xml.GetInteger(&l,STR_THREADPRIORITY)){
		m_SliderPriority.SetPos((int)l);
		OnReleasedcaptureSliderPriority(NULL, NULL);
	}


	if(Xml.GetInteger(&l,STR_SLEEP)){
		GSleep=l;
		if(GSleep>32) GSleep=32;
		if(GSleep<1) GSleep=1;
		m_SliderSleep.SetPos(GSleep);
		
		m_SleepCaption.Format("%d",m_SliderSleep.GetPos());
	}
	
	if(Xml.GetInteger(&l,STR_LOGENABLE)){
		GLogFileEnable=l;
		if(GLogFileEnable){
			CheckMenuItem(Menu,ID_MENUITEM_LOGOPEN,MF_CHECKED);
			CheckDlgButton(IDC_CHECK_LOG_FILE,1);
		}else{
			CheckMenuItem(Menu,ID_MENUITEM_LOGOPEN,MF_UNCHECKED);
			CheckDlgButton(IDC_CHECK_LOG_FILE,0);
		}
	}


	if(Xml.GetInteger(&l,STR_WATCHLIST)){
		GWatchList=(UCHAR)l;
		if(GWatchList){
			CheckMenuItem(Menu,ID_MENUITEM_WATCH_LIST,MF_CHECKED);
			CheckDlgButton(IDC_CHECK_WATCH_LIST,1);
		}else{
			CheckMenuItem(Menu,ID_MENUITEM_WATCH_LIST,MF_UNCHECKED);
			CheckDlgButton(IDC_CHECK_WATCH_LIST,0);
		}
	}
	
	
	if(Xml.GetInteger(&l,STR_FILTERSELFSENT)){
		GFilterSelfSent=(UCHAR)l;
		if(GFilterSelfSent){
			CheckMenuItem(Menu,ID_MENUITEM_FILTER_SELF_SENT,MF_CHECKED);
			CheckDlgButton(IDC_CHECK_REMOVE_SELF_SEND,1);
		}else{
			CheckMenuItem(Menu,ID_MENUITEM_FILTER_SELF_SENT,MF_UNCHECKED);
			CheckDlgButton(IDC_CHECK_REMOVE_SELF_SEND,0);
		}
	}
	
	
	//禁止<SEND>
	if(Xml.GetInteger(&l,STR_DISABLESEND)){
		m_CheckDisableRepay=(BOOL)l;
	}
	if(Xml.GetInteger(&l,STR_DISABLESENDSYMBOL)){
		m_DisableRepaySymbol=(UINT)l;
	}
	
	//替换校验字
	if(Xml.GetInteger(&l,STR_CHECKTYPE)){
		m_ReplaceCheckType=(int)l;
	}
	if(Xml.GetInteger(&l,STR_CHECKBEGIN)){
		m_CheckOutBegin=(int)l;
	}
	if(Xml.GetInteger(&l,STR_CHECKEND)){
		m_CheckOutEnd=(int)l;
	}
	if(Xml.GetInteger(&l,STR_CHECKTARGET)){
		m_CheckOutTarget=(int)l;
	}
	if(Xml.GetInteger(&l,STR_CHECKSYMBOL)){
		m_ReplaceCheckSymbol=(unsigned int)l;
	}
	if(Xml.GetInteger(&l,STR_CHECKENABLE)){
		m_ReplaceCheck=(BOOL)l;
		if(m_ReplaceCheck){
			GetDlgItem(IDC_EDIT_REPLACE_CHECK_SYMBOL)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_REPLACE_CHECK_TYPE)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_CHECKOUT_BEGIN)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_CHECKOUT_END)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_CHECKOUT_TARGET)->EnableWindow(TRUE);
			
		}else{
			GetDlgItem(IDC_EDIT_REPLACE_CHECK_SYMBOL)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_REPLACE_CHECK_TYPE)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_CHECKOUT_BEGIN)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_CHECKOUT_END)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_CHECKOUT_TARGET)->EnableWindow(FALSE);
		}
	}
	
	
	if(Xml.GetInteger(&l,STR_RECEIVEDELAY)){
		GAllReceiveDelay=(unsigned long)l;
	}
	
	if(Xml.GetInteger(&l,STR_SENDDELAY)){
		GAllSendDelay=(unsigned long)l;
	}
	
	if(Xml.GetInteger(&l,STR_DELAYREALTIME)){
		GDelayRealtime=(unsigned char)l;
	}
	
	if(Xml.GetInteger(&l,STR_FRAMETYPE)){
		GFrameType=(unsigned char)l;
	}
	
	m_FrameType=GFrameType;
	
	if(Xml.GetString(STR_LOGFILE,MAX_PATH,buf,outlen)){
		GLogFile=buf;
		
		//Reopen Log handle for Log file work
		GLogMutex.Lock();
		if(HLogFile!=INVALID_HANDLE_VALUE){
			CloseHandle(HLogFile);
			HLogFile=INVALID_HANDLE_VALUE;
		}
		CString logpath;
		GetPathFromFileName(logpath,filename);
		logpath+=GLogFile;
		HLogFile=CreateFile(logpath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,NULL,NULL);
		if(HLogFile==INVALID_HANDLE_VALUE){
			MessageBox("Can not open log file\r\n", NULL, MB_ICONERROR);
			//return 0;
		}
		GLogMutex.Unlock();
		
	}
	
	
	if(Xml.GetString(STR_PORT,MAX_PATH,buf,outlen)){
		GPort=buf;
	}
	if(Xml.GetInteger(&l,STR_SPEED)){
		GSpeed=l;
	}
	if(Xml.GetInteger(&l,STR_PARITY)){
		GParity=(UCHAR)l;
	}
	if(Xml.GetInteger(&l,STR_STOPBITS)){
		GStopbits=(UCHAR)l;
	}
	if(Xml.GetInteger(&l,STR_BITS)){
		GBits=(UCHAR)l;
	}
	
	
	if(Xml.GetInteger(&l,STR_MINFRAMESPACE)){
		GMinFrameSpace=l;
	}


	if(Xml.GetString(STR_TITLE,MAX_PATH,buf,outlen)){
		GTitle=buf;
	}
	
	
	//Manual Data
	GetDlgItem(IDC_INPUT_TEMP)->SetWindowText(Xml.GetString(STR_MANUALDATA,""));
	l=Xml.GetInteger(STR_REPLYBYMANUAL,0);
	CheckDlgButton(IDC_CHECK_AUTO_REPLY_BY_TEMP,(UINT)l);
	GAutoReplyByTemp=(unsigned char)l;
	
	
	
	//AutoReply Configuration
	if(Xml.GetInteger(&l,STR_AUTOREPLYENABLE)){
		GAutoReplyEnable=(unsigned char)l;
	}
	switch(GAutoReplyEnable){
	default:
	case 0:
		CheckDlgButton(IDC_CHECK_TIMER_REPLY,0);
		CheckDlgButton(IDC_RADIO_TIMER_FILE,1);
		CheckDlgButton(IDC_RADIO_TIMER_TEMP,0);
		
		GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->EnableWindow(FALSE);
		
		GetDlgItem(IDC_RADIO_TIMER_FILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_TIMER_TEMP)->EnableWindow(FALSE);
		break;
	case 1:
		CheckDlgButton(IDC_CHECK_TIMER_REPLY,1);
		CheckDlgButton(IDC_RADIO_TIMER_FILE,1);
		CheckDlgButton(IDC_RADIO_TIMER_TEMP,0);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->EnableWindow(TRUE);
		
		GetDlgItem(IDC_RADIO_TIMER_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TIMER_TEMP)->EnableWindow(TRUE);
		break;
	case 2:
		CheckDlgButton(IDC_CHECK_TIMER_REPLY,1);
		CheckDlgButton(IDC_RADIO_TIMER_FILE,0);
		CheckDlgButton(IDC_RADIO_TIMER_TEMP,1);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->EnableWindow(TRUE);
		
		GetDlgItem(IDC_RADIO_TIMER_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TIMER_TEMP)->EnableWindow(TRUE);
		break;
	}
	
	if(Xml.GetInteger(&l,STR_AUTOREPLYID)){
		GAutoReplyID=(unsigned char)l;
	}
	itoa(GAutoReplyID,buf,10);
	GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->SetWindowText(buf);
	
	if(Xml.GetInteger(&l,STR_AUTOREPLYSPACE)){
		GAutoReplySpace=l;
	}
	itoa(GAutoReplySpace,buf,10);
	GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->SetWindowText(buf);
	


	///////////////////////////////////////////////读取完毕

	//Update main window title	
	if(GTitle.GetLength()>0){
		SetWindowText(GTitle);
	}else{
		SetWindowText(STR_APP_TITLE);
	}

	
	GConfigFile=filename;


	
	if(GConfigFile.GetLength()>=43){
		sprintf(buf,"%s...%s <CONFIG> %d loaded.",GConfigFile.Left(14).GetBuffer(14),GConfigFile.Right(26).GetBuffer(26),configid);
	}else{
		sprintf(buf,"%s <CONFIG> %d loaded.",GConfigFile.GetBuffer(256),configid);
	}
	AddMsg(ID_IMG_NOTE,buf,NULL,NULL,NULL);


	if(reopencomm){	
		GComm.CloseComm();
		if(!GComm.OpenComm(GPort.GetBuffer(128),GSpeed,GParity,GBits,GStopbits)){
			AddMsg(ID_IMG_ERR,"Open Serial Port Failed.",NULL,NULL,NULL);
		}
	}else{
		GComm.SetCommPara(GSpeed,GParity,GBits,GStopbits);
	}
	
	if(GComm.IsOpen()){
		GComm.PurgeComm(PURGE_RXCLEAR);
	}
	

	Xml.Close();
	
	if(reloadcomm){
		LoadComm(filename);
	}


	m_PortState.Format("%s %ld %s %s停止位 %d位",
		GComm.IsOpen()?GPort.GetBuffer(128):"串口失败",
		GSpeed,
		ParityValue2Description(GParity),
		StopBitsValue2Description(GStopbits),
		GBits
		);


	
	//在这里重新打开XML文件句柄以便检测文件大小
	if(m_fileForAutoLoad.m_hFile!=m_fileForAutoLoad.hFileNull){
		m_fileForAutoLoad.Close();
	}
	m_fileForAutoLoad.Open(GConfigFile,CFile::modeRead|CFile::shareDenyNone|CFile::typeBinary);



	//Update for
	QueryPerformanceFrequency(&Gli);
	GFrequency=Gli.QuadPart;


	UpdateData(FALSE);

	return 1;
}

char CVirtualCarWnd::LoadComm(LPCSTR filename)
{
	CDTXML Xml;
	CString temp;
	//CString temp1,temp2;
	int i,j;

	if(!Xml.Open(GConfigFile.GetBuffer(128))){
		return 0;
	}

	if(!Xml.Enter(STR_CONTENT)){
		return 0;
	}

	if(!Xml.Enter(STR_COMMS)){
		return 0;
	}

	for(i=0; i<MAX_COMM_COUNT; i++){
		if(Xml.Enter(STR_COMM,i)){
			Comm[i].Enable=(UCHAR)Xml.GetInteger(STR_ENABLE,(LONG)1);
// 			Comm[i].Count=(UCHAR)Xml.GetInteger(STR_COUNT,(LONG)1);
			Comm[i].Act=Xml.GetString(STR_ACT,"");
			Comm[i].Pos=(UCHAR)Xml.GetInteger(STR_POS,(LONG)0);
			Comm[i].Receive=Xml.GetString(STR_RECEIVE,"");
			Comm[i].Receive.Remove(' ');
			Comm[i].Receive.Remove(',');
			Comm[i].Receive.Remove('h');
			Comm[i].Receive.Remove('H');
			Comm[i].Receive.Remove('\r');
			Comm[i].Receive.Remove('\n');
            Comm[i].Receive.Remove('\t');
			Comm[i].Receive.Replace("0x","");

			Comm[i].Remark=Xml.GetString(STR_REMARK,"");
			Comm[i].Type=(UCHAR)Xml.GetInteger(STR_TYPE,(LONG)SEND_TYPE_NORMAL);
			Comm[i].Break=(UCHAR)Xml.GetInteger(STR_BREAK,(LONG)SEND_BREAK_NORMAL);	//RTT/20081023
			Comm[i].Show=(UCHAR)Xml.GetInteger(STR_SHOW,(LONG)1);
			Comm[i].Delay=Xml.GetInteger(STR_DELAY,0);

			for(j=0; j<MAX_SEND_COUNT; j++){
				if(Xml.Enter(STR_SEND,j)){
					Comm[i].Send[j].Data=Xml.GetString(STR_DATA,"");
					Comm[i].Send[j].Data.Remove(' ');
					Comm[i].Send[j].Data.Remove(',');
					Comm[i].Send[j].Data.Remove('H');
					Comm[i].Send[j].Data.Remove('h');
					Comm[i].Send[j].Data.Remove('\r');
					Comm[i].Send[j].Data.Remove('\n');
                    Comm[i].Send[j].Data.Remove('\t');
					Comm[i].Send[j].Data.Replace("0x","");

					Comm[i].Send[j].Remark=Xml.GetString(STR_REMARK,"");
					Comm[i].Send[j].Symbol=(UCHAR)Xml.GetInteger(STR_SYMBOL,0);
					Comm[i].Send[j].Act=Xml.GetString(STR_ACT,"");
					Comm[i].Send[j].Delay=Xml.GetInteger(STR_DELAY,0);
					Xml.Back(1);
				}else{
					break;
				}
			}

			Comm[i].SCount=j;
			//add by RTT 10.03.11
			Comm[i].Count=(UCHAR)Xml.GetInteger(STR_COUNT,(LONG)j);



			for(j=0; j<MAX_Y; j++){
				temp.Format(STR_YX,j);
				Comm[i].Y[j]=Xml.GetString(temp.GetBuffer(255),"");
			}


			Xml.Back();
		}else{
			break;
		}
	}
	CommCount=i;


	return 1;
}


// DWORD MyReadComm(PBYTE pData, DWORD nLength)
// {
// 
// 	DWORD TotalRead=0;
// 	DWORD dwNumRead=0;	// 串口收到的数据长度
// 
// 	//3倍超时
// 	DWORD t=GetTickCount()+100+(1000/(GSpeed/10))*3;	//timeout
// 	//DWORD t=GetTickCount()+100+nLength;	//timeout
// 
// 	if(!GComm.IsOpen()){
// 		return 0;
// 	}
// 
// 	while(TotalRead<nLength && GRTS==RTS_RUNNING){
// 	
// 		dwNumRead=0;
// 		ReadFile(GHComm, pData+TotalRead, nLength-TotalRead, &dwNumRead, NULL);
// 		TotalRead+=dwNumRead;
// 
// 
// 		if(GetTickCount()>t){
// 			//MYDEBUG0("Sleep(10), ReadComm Timeout")
// 			break;
// 		}
// 	}
// 
// 	return TotalRead;
// }



DWORD WINAPI ThreadCheck(VOID * p)
{
	CVirtualCarWnd * win=(CVirtualCarWnd *)p;

	TRACE("Dengting: Thread begins\r\n");

	GRTS=RTS_RUNNING;
	while(GRTS==RTS_RUNNING){
		if(GFrameType==FRAME_TYPE_VCX){
			ReceiveForVCXFrame(win);
		}else if(GFrameType==FRAME_TYPE_KT600){
			ReceiveForKT600Frame(win);
		}else{
			ReceiveForMiniFrameSpace(win);
		}

		CheckForAutoReply(win);

	}//end while

	GRTS=RTS_NONE;

	TRACE("Dengting: Thread has been exited\r\n");
	return 0;
}

VOID CheckForAutoReply(CVirtualCarWnd * win)
{
	if(GComm.IsOpen()){
		GTempTickcount=GetTickCountEx();
		if(GTempTickcount>=GAutoreplyLastTime+GAutoReplySpace){
			if(GAutoReplyEnable==1){
				if(!win->ResponseFrame(GAutoReplyID)){
					if(GWatchList){
						win->AddMsg(ID_IMG_ERR, "Error, response frame failed", NULL,NULL,NULL);
					}
				}
			}else if(GAutoReplyEnable==2){
				CString str;
				win->GetDlgItem(IDC_INPUT_TEMP)->GetWindowText(str);
				win->SendTempData(str);
			}
			
			GAutoreplyLastTime=GTempTickcount;
		}
	}
}

VOID ReceiveForMiniFrameSpace(CVirtualCarWnd * win)
{
	if(GComm.ReadComm(GInBuffer+GInBufferCount,1)>0){
		if(GInBufferCount>=MAX_IN_SIZE){
			GLastReceiveByteTime=0;	//处理帧
		}else{
			GInBufferCount++;
			GLastReceiveByteTime=GetTickCountEx();
		}			
	}else{
		if(GInBufferCount==0){
			if(GSleep){
				if(!(GAutoReplyEnable && GAutoReplySpace<GAutoReply100CPU)){//开启自动发送时，不休眠
					Sleep(GSleep);
				}
			}
		}
	}

	GTempTickcount=GetTickCountEx();
	if(GTempTickcount>=GLastReceiveByteTime+GMinFrameSpace){
		if(GInBufferCount>0){
			win->ProcessFrame(GInBuffer,GInBufferCount);
			GInBufferCount=0;
		}
		GLastReceiveByteTime=GTempTickcount;
	}
}

VOID ReceiveForKT600Frame(CVirtualCarWnd * win)
{
	UINT len;

	if(GComm.ReadComm(GInBuffer+GInBufferCount,1)==1){
		GInBufferCount++;
		if(GComm.ReadComm(GInBuffer+GInBufferCount,1)==1){
			GInBufferCount++;
			if(memcmp(GInBuffer,(void*)"\xa5\xa5",2)==0){
				if(GComm.ReadComm(GInBuffer+GInBufferCount,2,NULL)==2){
					GInBufferCount+=2;
					len=GInBuffer[2]*256+GInBuffer[3]+1;
					if(len+4<MAX_IN_SIZE){
						if(GComm.ReadComm(GInBuffer+GInBufferCount,len,NULL)==len){
							GInBufferCount+=len;
							win->ProcessFrame(GInBuffer,GInBufferCount);
							GInBufferCount=0;
						}
					}
				}
			}
		}
		GInBufferCount=0;
	}else{
		GInBufferCount=0;
		if(GSleep){
			//开启自动发送时，不休眠
			if(!(GAutoReplyEnable && GAutoReplySpace<GAutoReply100CPU)){
				Sleep(GSleep);
			}
		}
	}
}

VOID ReceiveForVCXPlusFrame(CVirtualCarWnd* win)
{
	CString str;
	BYTE b=0;

	while(b!=VCX_FRAME_MARK){
		if(GRTS!=RTS_RUNNING){
			return;
		}
		if(GFrameType!=FRAME_TYPE_VCXPLUS){
			return;
		}

		CheckForAutoReply(win);

		if(GComm.ReadComm(&b,1)!=1){
			if(!(GAutoReplyEnable && GAutoReplySpace<GAutoReply100CPU)){
				Sleep(GSleep);
			}
			continue;
		}

		if(b!=VCX_FRAME_MARK){
			str.Format("Discard:%02x",b);
			win->AddMsg(ID_IMG_ERR,str.GetBuffer(0),NULL,NULL,NULL);
		}
	}

	//BEGIN:
	GInBufferCount=0;

	for(;;){
		if(GRTS!=RTS_RUNNING){
			return;
		}
		if(GFrameType!=FRAME_TYPE_VCX){
			return;
		}

		CheckForAutoReply(win);

		if(GComm.ReadComm(&b,1)!=1){
			// 			if(!(GAutoReplyEnable && GAutoReplySpace<GAutoReply100CPU)){
			// 				Sleep(GSleep);
			// 			}
			continue;
		}

		if(GInBufferCount>=MAX_IN_SIZE){
			win->AddMsg(ID_IMG_ERR,"Buffer for VCXPlus Frame is full",NULL,NULL,NULL);
			return;
		}

		if(b==VCX_FRAME_MARK){
			break;
		}

		GInBuffer[GInBufferCount]=b;
		GInBufferCount++;
	}

	//Trans
	DWORD i=0;
	DWORD outcount=0;
	for(i=0; i<GInBufferCount; i++){
		if(GInBuffer[i]==VCX_FRAME_TRANS2){
			i++;
			GInBuffer[outcount]=GInBuffer[i] ^ VCX_FRAME_XOR ;
			outcount++;
		}else{
			GInBuffer[outcount]=GInBuffer[i] ;
			outcount++;
		}
	}

	if(GRTS!=RTS_RUNNING){
		return;
	}
	if(GFrameType!=FRAME_TYPE_VCX){
		return;
	}

	if((BYTE)CalcSum(GInBuffer,outcount-1)!=GInBuffer[outcount-1]){
		win->AddMsg(ID_IMG_ERR,"Frame ckeck sum error",NULL,NULL,NULL);
		//return FALSE;
	}

	win->ProcessFrame(GInBuffer,outcount-1);
}



const BYTE FRAME_HEAD	=0xbb;
const BYTE FRAME_TAIL	=0xee;
const BYTE FRAME_TRANS	=0xaa;

const BYTE FRAME_TRANS_HEAD=0x0b;
const BYTE FRAME_TRANS_TAIL=0x0e;
const BYTE FRAME_TRANS_TRANS=0x0a;

const BYTE FRM_RCV_TYPE_DATA	=0x00;
const BYTE FRM_RCV_TYPE_DEBUGMSG=0x10;
const BYTE FRM_RCV_TYPE_ERRCODE	=0x20;

DWORD CalcSum(BYTE * buffer, DWORD size)
{
	DWORD sum=0;
	DWORD i;
	
	for(i=0; i<size; i++){
		sum+=buffer[i];
	}
	
	return sum;
}

VOID ReceiveForVCXFrame(CVirtualCarWnd * win)
{
	CString str;
	BYTE b=0;

	while(b!=FRAME_HEAD){
		if(GRTS!=RTS_RUNNING){
			return;
		}
		if(GFrameType!=FRAME_TYPE_VCX){
			return;
		}
		
		CheckForAutoReply(win);

		if(GComm.ReadComm(&b,1)!=1){
			if(!(GAutoReplyEnable && GAutoReplySpace<GAutoReply100CPU)){
				Sleep(GSleep);
			}
			continue;
		}

		if(b!=FRAME_HEAD){
			str.Format("Discard:%02x",b);
			win->AddMsg(ID_IMG_ERR,str.GetBuffer(0),NULL,NULL,NULL);
		}
	}

//BEGIN:
	GInBufferCount=0;

	for(;;)
	{
		if(GRTS!=RTS_RUNNING)
		{
			return;
		}
		if(GFrameType!=FRAME_TYPE_VCX)
		{
			return;
		}

		CheckForAutoReply(win);
		
		if(GComm.ReadComm(&b,1)!=1)
		{
 			//if(!(GAutoReplyEnable && GAutoReplySpace<GAutoReply100CPU))
			//{
 			//	Sleep(GSleep);
 			//}
			continue;
		}
		
		if(GInBufferCount>=MAX_IN_SIZE){
			win->AddMsg(ID_IMG_ERR,"Buffer for VCXFrame is full",NULL,NULL,NULL);
			return;
		}

		if(b==FRAME_TAIL){
			break;
		}else if(b==FRAME_HEAD){
			GInBufferCount=0;
			continue;
		}

		GInBuffer[GInBufferCount]=b;
		GInBufferCount++;
	}

	//Trans
	DWORD i;
	DWORD outcount=0;

	for(i=0; i<GInBufferCount; i++)
	{
		if(GInBuffer[i]==FRAME_TRANS){
			i++;
			switch(GInBuffer[i]){
			case FRAME_TRANS_HEAD:
				GInBuffer[outcount]=FRAME_HEAD;
				outcount++;
				break;
			case FRAME_TRANS_TAIL:
				GInBuffer[outcount]=FRAME_TAIL;
				outcount++;
				break;
			case FRAME_TRANS_TRANS:
				GInBuffer[outcount]=FRAME_TRANS;
				outcount++;
				break;
			default:
				win->AddMsg(ID_IMG_ERR, "Error trans BYTE",NULL,NULL,NULL);
				break;
			}
		}else{
			GInBuffer[outcount]=GInBuffer[i];
			outcount++;
		}
	}

	if(GRTS!=RTS_RUNNING){
		return;
	}
	if(GFrameType!=FRAME_TYPE_VCX){
		return;
	}

	if((BYTE)CalcSum(GInBuffer,outcount-1)!=GInBuffer[outcount-1]){
		win->AddMsg(ID_IMG_ERR,"Frame ckeck sum error",NULL,NULL,NULL);
		//return FALSE;
	}

	win->ProcessFrame(GInBuffer,outcount-1);

	//Sleep(1);

}




char CVirtualCarWnd::ProcessFrame(BYTE * buffer, UINT len)
{
	char msg[MAX_IN_SIZE*3+8];
	UINT i;
	UINT matchid=0xffff;
	CString str;
	CString temp;
	INT delay;

	if(len>MAX_IN_SIZE){
		return 0;
	}         

	if(!Buffer2HexString(buffer,len,msg,MAX_IN_SIZE*3)){
		return 0;
	}

	/*
	if(GEmulatePart==EMU_IOBOARD_ECU){
		//remove the frame head from hexstring
		strcpy(msg,msg+FrameHead.GetLength());

		//remove the frame tail from hexstring
		msg[strlen(msg)-FrameTail.GetLength()]=0;
	}
	*/


	if(len<=1024*8)	//如果太长的帧，不在文件中作通配比较。。。。太慢。。。
	{
		for(i=0; i<CommCount; i++)
		{
			//if(MatchCondition(Comm[i].C.GetBuffer(256))){
			if(Comm[i].Enable)
			{
				if(MatchingString(msg, Comm[i].Receive.GetBuffer(256),0))
				{
					matchid=i;
					break;
				}
			}
		}
	}

	if(matchid!=0xffff)
	{

		GTempTickcount=GetTickCountEx();
		if(Comm[matchid].Show){
			if(GWatchList || GLogFileEnable){
				Buffer2HexString(buffer,len,msg,MAX_IN_SIZE*3,""," ");
			}
			if(GWatchList){
				str=Comm[matchid].Remark;
				//ProcessArithY(matchid,str);
				temp.Format("%.3fms",GTempTickcount-GLastProcessFrameTimetime);
				AddMsg(ID_IMG_DIAGNOTOR,msg,str.GetBuffer(0),NULL,temp.GetBuffer(0));
			}
			if(GLogFileEnable){
				LogToFile(0,msg);
			}
		}
		GLastProcessFrameTimetime=GetTickCountEx();

		
		delay=GAllReceiveDelay+Comm[matchid].Delay;
		if(delay){
			if(GDelayRealtime){
				PC_DelayXms(delay);
			}else{
				Sleep(delay);
			}
		}


		//执行回应前ACT
		if(Comm[matchid].Act.GetLength()>0){
			ExecuteAct(Comm[matchid].Act);
		}


		//如果有Y,计算Y的值
		for(i=0; i<MAX_Y; i++){
			if(Comm[matchid].Y[i].GetLength()>0){
				CalcYResult(GYHex[i], Comm[matchid].Y[i], buffer, len);
			}
		}


		if(GPause){
			return TRUE;
		}

		if(!ResponseFrame(matchid)){
			if(GWatchList){
				AddMsg(ID_IMG_ERR,"Response frame failed",NULL,NULL,NULL);
			}
		}

	}else{

		if(GWatchList || GLogFileEnable){
			Buffer2HexString(buffer,len,msg,MAX_IN_SIZE*3,""," ");
		}

		if(GLogFileEnable){
			LogToFile(0,msg);
		}

		GTempTickcount=GetTickCountEx();
		if(GWatchList){
			temp.Format("%.3fms",GTempTickcount-GLastProcessFrameTimetime);
			AddMsg(ID_IMG_DIAGNOTOR,msg,"Undefined",NULL,temp.GetBuffer(0));
		}
		GLastProcessFrameTimetime=GetTickCountEx();


		if(GPause){
			return TRUE;
		}

		//自动用临时数据回复
		if(GAutoReplyByTemp){

			if(GAllReceiveDelay){
				if(GDelayRealtime){
					PC_DelayXms(GAllReceiveDelay);
				}else{
					Sleep(GAllReceiveDelay);
				}
			}

			GetDlgItem(IDC_INPUT_TEMP)->GetWindowText(str);
			SendTempData(str);

			if(GAllSendDelay){
				if(GDelayRealtime){
					PC_DelayXms(GAllSendDelay);
				}else{
					Sleep(GAllSendDelay);
				}
			}
		}
	}

	return 1;
}


CHAR CalcYResult(CString & outYX, CString exstr, BYTE * rcvbuf, INT rcvbufcnt)
{
	int b,e;
	int entersub;
	int len;
	int pos;
	CString str;

	//替换表达式中的B()函数
	while( (b=exstr.Find("B("))!=-1){
		
		//找到B()的End
		entersub=0;
		len=exstr.GetLength();
		for(e=b+2;e<len;e++){
			if(exstr.GetAt(e)==')'){
				if(entersub>0){
					entersub--;
				}else{
					break;
				}
			}else if(exstr.GetAt(e)=='('){
				entersub++;
			}
		}
		e++;	//找到)后，再移一字节。
	
	
		//找到B()的Begin和End后，取得B()中的
		pos=(int)atol(exstr.Mid(b+2,e-b-3));
		exstr.Delete(b,e-b);
		
		//将+/-位置转成 +数位置
		if(pos>=0){
			if(pos>=rcvbufcnt){
				pos=rcvbufcnt-1;
			}
		}else{
			pos=rcvbufcnt+pos;
		}
		
		str.Format("%d",(BYTE)rcvbuf[pos]);
		exstr.Insert(b,str);
	}

	outYX.Format("%02x",((unsigned char)GEx.Calculate(exstr)));

	return 1;
}

//
////计算出指COMM中的Y? 的结果
//BOOL GetReplaceYStr(UINT id, UINT yid, char * outbuf, int outbufsize)
//{
//	CString exstr;
//	LONG b,e;
//	INT pos;
//	CString str;
//	double f;
//	int entersub;
//	
//	if(Comm[id].Y[yid].GetLength()<2){
//		return 0;
//	}
//
//	exstr=Comm[id].Y[yid];
//
//	
//	//替换表达式中的B()函数
//	while( (b=exstr.Find("B("))!=-1){
//		
//		//找到B()的End
//		e=b+2;
//		entersub=0;
//		for(;e<exstr.GetLength();e++){
//			if(exstr.GetAt(e)==')'){
//				if(entersub>0){
//					entersub--;
//				}else{
//					break;
//				}
//			}else if(exstr.GetAt(e)=='('){
//				entersub++;
//			}
//		}
//		e++;	//找到)后，再移一字节。
//	
//	
//		//找到B()的Begin和End后，取得B()中的
//		pos=(unsigned char)atol(exstr.Mid(b+2,e-b-3));
//		exstr.Delete(b,e-b);
//		
//		//将+/-位置转成 +数位置
//		if(pos>0){
//			if(pos>=(INT)GInBufferCount){
//				pos=GInBufferCount-1;
//			}
//		}else{
//			pos=GInBufferCount+pos;
//		}
//		
//		str.Format("%d",(BYTE)GInBuffer[pos]);
//		exstr.Insert(b,str);
//	}
//
//	
//	f=GEx.Calculate(exstr);
//	sprintf(outbuf,"%02x",(unsigned char)f);
//
//	return 1;
//
//}


//将Str中的Y0替换为计算结果
void ProcessRefrenceYX(CString & str)
{
	int i;
	char strbuf[32];

	for(i=0; i<MAX_Y; i++){
		sprintf(strbuf,STR_YX,i);
		str.Replace(strbuf,GYHex[i]);
	}

	return;
}



unsigned char GJ1850CRCBitByBit(unsigned char* p, unsigned char len)
{
	
	unsigned char crchighbit=0x80;
	unsigned char crc=0x7E;
	unsigned char polynom=0x1D;
	unsigned char order=8;
	
	// bit by bit algorithm with augmented zero bytes.
	// does not use lookup table, suited for polynom orders between 1...32.
	
	unsigned char i, j, c, bit;
	
	
	for (i=0; i<len; i++) {
		
		//Dengting
		//c = (unsigned long)*p++;
		c = *p++;
		
		for (j=0x80; j; j>>=1) {
			
			bit = crc & crchighbit;
			crc<<= 1;
			if (c & j) crc|= 1;
			if (bit) crc^= polynom;
		}
	}
	
	for (i=0; i<order; i++) {
		
		bit = crc & crchighbit;
		crc<<= 1;
		if (bit) crc^= polynom;
	}
	
	crc^= 0xFF;;
	
	return(crc);
}

void CVirtualCarWnd::ReplaceCheckOut(BYTE * bytebuf, INT bytebuflen)
{
	int b,e,t; int n;
	unsigned char check;
	unsigned short U16check;


	if(bytebuflen<=0){
		return;
	}
	
	if(m_CheckOutBegin>=0){
		b=m_CheckOutBegin;
	}else{
		b=bytebuflen+m_CheckOutBegin;
	}

	if(m_CheckOutEnd>=0){
		e=m_CheckOutEnd;
	}else{
		e=bytebuflen+m_CheckOutEnd;
	}
	
	if(m_CheckOutTarget>=0){
		t=m_CheckOutTarget;
	}else{
		t=bytebuflen+m_CheckOutTarget;
	}


	if(b<0)	b=0;
	if(e<0) e=0;
	if(t<0) t=0;

	if(b>=bytebuflen) b=bytebuflen-1;
	if(e>=bytebuflen) e=bytebuflen-1;
	if(t>=bytebuflen) t=bytebuflen-1;

	switch(m_ReplaceCheckType){
	case CHECK_TYPE_XOR:
		check=0x00;
		for(n=b; n<=e; n++){
			check=check^bytebuf[n];
		}
		bytebuf[t]=check;
		break;
	case CHECK_TYPE_SUM:
		check=0x00;
		for(n=b; n<=e; n++){
			check=check+bytebuf[n];
		}
		bytebuf[t]=check;
		break;
	case CHECK_TYPE_SUM_ANTI:
		check=0x00;
		for(n=b; n<=e; n++){
			check=check+bytebuf[n];
		}
		check=~check;
		bytebuf[t]=check;
		break;
	case CHECK_TYPE_SUM_ANTI1:
		check=0x00;
		for(n=b; n<=e; n++){
			check=check+bytebuf[n];
		}
		check=~check;
		bytebuf[t]=check+1;
		break;
	case CHECK_TYPE_2BYTESUM_HIGHLOW:
		U16check=0x0000;
		for(n=b; n<=e; n++){
			U16check=U16check+bytebuf[n];
		}
		bytebuf[t]=(unsigned char)(U16check>>8);
		if(t+1<bytebuflen){
			bytebuf[t+1]=(unsigned char)U16check;
		}
		break;
	case CHECK_TYPE_2BYTESUM_LOWHIGH:
		U16check=0x0000;
		for(n=b; n<=e; n++){
			U16check=U16check+bytebuf[n];
		}
		bytebuf[t]=(unsigned char)U16check;
		if(t+1<bytebuflen){
			bytebuf[t+1]=(unsigned char)(U16check>>8);
		}
		break;
	case CHECK_TYPE_J1850CRC8:
		check=GJ1850CRCBitByBit(bytebuf+b,e-b+1);
		bytebuf[t]=check;
		break;
	}


}

char CVirtualCarWnd::ResponseFrame(UINT id)
{
	int i,j;	//begin, end,target;
	int total;
	char buffer[4096];
	// unsigned char bytebuf[512];
	// unsigned char bytebuf2[1028];//512*2+4
	// UINT bytebuflen;
	// INT bytebuflen2;
	// 修改ByteBuf及ByteBuf2大小为2048和4100 @2011.11.01 by Fanze
	unsigned char bytebuf[2048] ;
	unsigned char bytebuf2[4100] ;
	UINT bytebuflen;
	INT bytebuflen2;

	CString str;
	CString temp;
	int rt;
	int delay;

	//int n;
	//unsigned char check;

	//for Modify Window
	if(PModifyWnd){
		GModifyWndMutex.Lock();

		int row=0x1ff;
		//查找到RowInfo中对应行
		for(i=0; i<RowInfoCnt; i++){
			if(RowInfo[i].TypeR0S1==0 && RowInfo[i].RID==id){
				row=i;
				break;
			}
		}
		if(row!=0x1ff){
			RowInfo[row].Cnt++;
			str.Format("%d",RowInfo[row].Cnt);
			PModifyWnd->m_grid.SetItemText(row+1,1,str.GetBuffer(0));
			PModifyWnd->m_grid.RedrawCell(row+1,1);
		}

		//顺序显示
		RcvIndex++;
		str=PModifyWnd->m_grid.GetItemText(row+1,2);
		str+=" ";
		str+=itoa(RcvIndex,buffer,10);
		if(str.GetLength()>1024){
			str=str.Right(16);
		}
		PModifyWnd->m_grid.SetItemText(row+1,2, str);
		PModifyWnd->m_grid.RedrawCell(row+1,2);

		GModifyWndMutex.Unlock();
	}



	//如果没有定义回应帧
	//if(Comm[id].SCount<=0){
	//	return 0;
	//}


	switch(Comm[id].Type){
	case SEND_TYPE_NORMAL:

		j=Comm[id].Pos;
		total=Comm[id].Count;	//要发几条

		for(i=0; i<total; i++){

			//确定要发送的S号
			if(j>=Comm[id].SCount){
				j=0;
			}

			//禁止发送
			if(m_CheckDisableRepay){
				if(Comm[id].Send[j].Symbol==m_DisableRepaySymbol){
					j++;
					continue;
				}
			}

			//处理ModifyWnd
			if(PModifyWnd){
				GModifyWndMutex.Lock();
				PModifyWnd->ProcessModify(id,j);
				PModifyWnd->RefreshSendOrder(id,j);
				GModifyWndMutex.Unlock();
			}

			str=Comm[id].Send[j].Data;

			while((rt=str.Find("?",0))!=-1){
				str.SetAt(rt,GenRandHexChar());
			}

			ProcessRefrenceYX(str);

			HexString2Buffer(str.GetBuffer(0),bytebuf,2048,&bytebuflen);


			//替换控制字节一
			if(m_CheckReplaceSendData){
				if(Comm[id].Send[j].Symbol==m_ReplaceSymbol){
					if(bytebuflen>m_ReplacePosNum1){
						bytebuf[m_ReplacePosNum1]=m_ReplaceToNum1;
					}
				}
			}
			//替换控制字节二
			if(m_CheckReplaceSendData2){
				if(Comm[id].Send[j].Symbol==m_ReplaceSymbol2){
					if(bytebuflen>m_ReplacePosNum2){
						bytebuf[m_ReplacePosNum2]=m_ReplaceToNum2;
					}
				}
			}



			//替换校验字
			if(m_ReplaceCheck){
				if(Comm[id].Send[j].Symbol==m_ReplaceCheckSymbol){
					ReplaceCheckOut(bytebuf,bytebuflen);
				}
			}


			//打包/发送
			if(GFrameType==1){// KT600
				bytebuflen2=KT600Pack(bytebuf,bytebuflen,bytebuf2,1028);
				if(bytebuflen2<=0){
					ASSERT("VCXPack failed\r\n");
				}
				GComm.WriteComm(bytebuf2,bytebuflen2);				
			}else if(GFrameType==2){// VCX
				bytebuflen2=VCXPack(bytebuf,bytebuflen,bytebuf2,1028);
				if(bytebuflen2<=0){
					ASSERT("VCXPack failed\r\n");
				}
				GComm.WriteComm(bytebuf2,bytebuflen2);				
			}else if(GFrameType==3) // vcxplus
			{
				bytebuflen2=VCXPlusPack(bytebuf,bytebuflen,bytebuf2,1028);
				if(bytebuf2<=0){
					ASSERT("VCXPlus Pack failed");
				}
				GComm.WriteComm(bytebuf2,bytebuflen2) ;
			}
			else{//no pack
				GComm.WriteComm(bytebuf,bytebuflen);				
			}


			GTempTickcount=GetTickCountEx();
			if(Comm[id].Show){
				if(GWatchList || GLogFileEnable){
					Buffer2HexString(bytebuf,bytebuflen,buffer,4096,""," ");
				}

				if(GLogFileEnable){
					LogToFile(1,buffer);
				}

				if(GWatchList){
					str=Comm[id].Send[j].Remark;
					//ProcessArithY(id,str);
					temp.Format("%.3f", GTempTickcount-GLastProcessFrameTimetime);
					AddMsg(ID_IMG_CAR,buffer,str.GetBuffer(255),"普通",temp.GetBuffer(255));
				}
			}
			GLastProcessFrameTimetime=GTempTickcount;


			delay=GAllSendDelay+Comm[id].Send[j].Delay;
			if(delay){
				if(GDelayRealtime){
					PC_DelayXms(delay);
				}else{
					Sleep(delay);
				}
			}


			if(Comm[id].Send[j].Act.GetLength()>0){
				ExecuteAct(Comm[id].Send[j].Act);
			}


			j++;
		}

		break;
	case SEND_TYPE_ORDINAL:
		j=Comm[id].Pos;
		total=Comm[id].Count;	//要发几条

		for(i=0; i<total; i++){

			if(Comm[id].Break==SEND_BREAK_BREAK){	//收到数据后要打断发送 RTT/20081023 
				COMSTAT comstat;
				GComm.ClearCommError(NULL, &comstat);
				if(comstat.cbInQue){				//有收到数据
					
					str.Format("BREAK");
					if(GWatchList)	AddMsg(ID_IMG_NOTE,str.GetBuffer(255),NULL,NULL,NULL);

					return 1;	
				}

			}
			if(j>=Comm[id].SCount){
				j=0;
			}

			//禁止发送
			if(m_CheckDisableRepay){
				if(Comm[id].Send[j].Symbol==m_DisableRepaySymbol){
					j++;
					continue;
				}
			}


			//处理ModifyWnd
			if(PModifyWnd){
				GModifyWndMutex.Lock();
				PModifyWnd->ProcessModify(id,j);
				PModifyWnd->RefreshSendOrder(id,j);
				GModifyWndMutex.Unlock();
			}


			str=Comm[id].Send[j].Data;

			while((rt=str.Find("?",0))!=-1){
				str.SetAt(rt,GenRandHexChar());
			}
			
			ProcessRefrenceYX(str);

			HexString2Buffer(str.GetBuffer(1024),bytebuf,512,&bytebuflen);

			
			//替换控制字节一
			if(m_CheckReplaceSendData){
				if(Comm[id].Send[j].Symbol==m_ReplaceSymbol){
					if(bytebuflen>m_ReplacePosNum1){
						bytebuf[m_ReplacePosNum1]=m_ReplaceToNum1;
					}
				}
			}
			//替换控制字节二
			if(m_CheckReplaceSendData2){
				if(Comm[id].Send[j].Symbol==m_ReplaceSymbol2){
					if(bytebuflen>m_ReplacePosNum2){
						bytebuf[m_ReplacePosNum2]=m_ReplaceToNum2;
					}
				}
			}


			
			//替换校验字
			if(m_ReplaceCheck){
				if(Comm[id].Send[j].Symbol==m_ReplaceCheckSymbol){
					ReplaceCheckOut(bytebuf,bytebuflen);
				}
			}


			//打包/发送
			if(GFrameType==1){//VCX
				bytebuflen2=KT600Pack(bytebuf,bytebuflen,bytebuf2,1028);
				if(bytebuflen2<=0){
					ASSERT("VCXPack failed\r\n");
				}
				GComm.WriteComm(bytebuf2,bytebuflen2);				
			}else if(GFrameType==2){//KT600	
				bytebuflen2=VCXPack(bytebuf,bytebuflen,bytebuf2,1028);
				if(bytebuflen2<=0){
					ASSERT("VCXPack failed\r\n");
				}
				GComm.WriteComm(bytebuf2,bytebuflen2);				
			}else if(GFrameType==3) // vcxplus
			{
				bytebuflen2=VCXPlusPack(bytebuf,bytebuflen,bytebuf2,1028);
				if(bytebuf2<=0){
					ASSERT("VCXPlus Pack failed");
				}
				GComm.WriteComm(bytebuf2,bytebuflen2) ;
			}else{//no pack
				GComm.WriteComm(bytebuf,bytebuflen);				
			}


			GTempTickcount=GetTickCountEx();
			if(Comm[id].Show){
				if(GWatchList || GLogFileEnable){
					Buffer2HexString(bytebuf,bytebuflen,buffer,4096,""," ");
				}

				if(GLogFileEnable){
					LogToFile(1,buffer);
				}

				if(GWatchList){
					str=Comm[id].Send[j].Remark;
					//ProcessArithY(id,str);
					temp.Format("%.3f", GTempTickcount-GLastProcessFrameTimetime);
					AddMsg(ID_IMG_CAR,buffer,str.GetBuffer(255),"顺序",temp.GetBuffer(255));
				}
			}
			GLastProcessFrameTimetime=GTempTickcount;


			delay=GAllSendDelay+Comm[id].Send[j].Delay;
			if(delay){
				if(GDelayRealtime){
					PC_DelayXms(delay);
				}else{
					Sleep(delay);
				}
			}


			if(Comm[id].Send[j].Act.GetLength()>0){
				ExecuteAct(Comm[id].Send[j].Act);
			}


			j++;
		}

		Comm[id].Pos=j;

		break;
	case SEND_TYPE_RANDOM:
		for(i=0; i<Comm[id].Count; i++){
			j=rand()%Comm[id].SCount;

			if(j>=Comm[id].SCount){	
				return 0;
			}

			//禁止发送
			if(m_CheckDisableRepay){
				if(Comm[id].Send[j].Symbol==m_DisableRepaySymbol){
					continue;
				}
			}

			//处理ModifyWnd
			if(PModifyWnd){
				GModifyWndMutex.Lock();
				PModifyWnd->ProcessModify(id,j);
				PModifyWnd->RefreshSendOrder(id,j);
				GModifyWndMutex.Unlock();
			}
			
			str=Comm[id].Send[j].Data;

			while((rt=str.Find("?",0))!=-1){
				str.SetAt(rt,GenRandHexChar());
			}

			ProcessRefrenceYX(str);

			HexString2Buffer(str.GetBuffer(1024),bytebuf,512,&bytebuflen);

			
			//替换控制字节一
			if(m_CheckReplaceSendData){
				if(Comm[id].Send[j].Symbol==m_ReplaceSymbol){
					if(bytebuflen>m_ReplacePosNum1){
						bytebuf[m_ReplacePosNum1]=m_ReplaceToNum1;
					}
				}
			}
			//替换控制字节二
			if(m_CheckReplaceSendData2){
				if(Comm[id].Send[j].Symbol==m_ReplaceSymbol2){
					if(bytebuflen>m_ReplacePosNum2){
						bytebuf[m_ReplacePosNum2]=m_ReplaceToNum2;
					}
				}
			}

			//替换校验字
			if(m_ReplaceCheck){
				if(Comm[id].Send[j].Symbol==m_ReplaceCheckSymbol){
					ReplaceCheckOut(bytebuf,bytebuflen);
				}
			}


			//打包/发送
			if(GFrameType==1){//KT600
				bytebuflen2=KT600Pack(bytebuf,bytebuflen,bytebuf2,1028);
				if(bytebuflen2<=0){
					ASSERT("VCXPack failed\r\n");
				}
				GComm.WriteComm(bytebuf2,bytebuflen2);				
			}else if(GFrameType==2){//VCX	
				bytebuflen2=VCXPack(bytebuf,bytebuflen,bytebuf2,1028);
				if(bytebuflen2<=0){
					ASSERT("VCXPack failed\r\n");
				}
				GComm.WriteComm(bytebuf2,bytebuflen2);				
			}else if(GFrameType==3) // vcxplus
			{
				bytebuflen2=VCXPlusPack(bytebuf,bytebuflen,bytebuf2,1028);
				if(bytebuf2<=0){
					ASSERT("VCXPlus Pack failed");
				}
				GComm.WriteComm(bytebuf2,bytebuflen2) ;
			}else{//no pack
				GComm.WriteComm(bytebuf,bytebuflen);				
			}



			GTempTickcount=GetTickCountEx();
			if(Comm[id].Show){
				if(GWatchList || GLogFileEnable){
					Buffer2HexString(bytebuf,bytebuflen,buffer,4096,""," ");
				}

				if(GLogFileEnable){
					LogToFile(1,buffer);
				}

				if(GWatchList){
					str=Comm[id].Send[j].Remark;
					//ProcessArithY(id,str);
					temp.Format("%.3f", GTempTickcount-GLastProcessFrameTimetime);
					AddMsg(ID_IMG_CAR,buffer,str.GetBuffer(255),"随机",temp.GetBuffer(255));

				}
			}
			GLastProcessFrameTimetime=GTempTickcount;


			delay=GAllSendDelay+Comm[id].Send[j].Delay;
			if(delay){
				if(GDelayRealtime){
					PC_DelayXms(delay);
				}else{
					Sleep(delay);
				}
			}


			if(Comm[id].Send[j].Act.GetLength()>0){
				ExecuteAct(Comm[id].Send[j].Act);
			}

		}
		break;
	}




	//收掉自发自收的字节.
	if(GFilterSelfSent){
		GComm.ReadComm(bytebuf2, bytebuflen2,NULL);		//加了减i防止多收..
	}


	
	return 1;
}



void CVirtualCarWnd::OnClose() 
{
	OnMenuitemClose();
	//CDialog::OnClose();
}

void CVirtualCarWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

BOOL CVirtualCarWnd::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message  ==  WM_LBUTTONDBLCLK){
		if(IsWindow(pMsg->hwnd)){
			if(m_hWnd!=pMsg->hwnd){
				char classname[128];
				if(GetClassName(pMsg->hwnd,classname,126)){
					if(strstr(classname,"Edit")){
						char buf[8192];
						if(::GetWindowText(pMsg->hwnd,buf,8192)){
							CopyTextToClipboard(m_hWnd,buf);
						}
					}
				}			
			}
		}
	}

    
	if(pMsg->message == WM_KEYDOWN){
        if(pMsg->wParam == VK_ESCAPE){
			return TRUE;
        }

		if(pMsg->wParam == VK_RETURN){

			//If focus on the TempData, Send it out.
			CWnd * pfocuswnd;
			CString str;

			pfocuswnd=GetFocus();

			if(GetDlgItem(IDC_INPUT_TEMP)->m_hWnd==pfocuswnd->m_hWnd){
				GetDlgItem(IDC_INPUT_TEMP)->GetWindowText(str);
				SendTempData(str);				
			}
			
			if(GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->m_hWnd==pfocuswnd->m_hWnd){
				OnKillfocusEditAutoReplySpace();
			}

			UpdateData(TRUE);
			
			return TRUE;
		}
	}



	return CDialog::PreTranslateMessage(pMsg);
}



char CVirtualCarWnd::LogToFile(char diag0car1, char * str)
{
	DWORD written;

	if(HLogFile==INVALID_HANDLE_VALUE){
		return 0;
	}

	GLogMutex.Lock();

	if(diag0car1){
		WriteFile(HLogFile,(LPCVOID)"CAR:",5,&written,NULL);
	}else{
		WriteFile(HLogFile,(LPCVOID)"DIA:",5,&written,NULL);
	}

	if(m_LogTime){//记录时间戳
		CString temp;
		SYSTEMTIME t;
		GetLocalTime(&t);
		//GetSystemTime(&t);

		temp.Format("(%2d:%2d:%2d:%3d)", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
		temp.Replace(' ','0');

		WriteFile(HLogFile,(LPCVOID)temp.GetBuffer(32),temp.GetLength(),&written,NULL);
	}
	
	WriteFile(HLogFile,(LPCVOID)str,strlen(str),&written,NULL);
	WriteFile(HLogFile,(LPCVOID)"\r\n",2,&written,NULL);

	GLogMutex.Unlock();

	return TRUE;
}


char CVirtualCarWnd::ExecuteAct(CString & act)
{
	int i;
	CString str2;

	int len;
	int id;

	len=act.GetLength();
	for(i=0; i<len; i++){
		switch(act.GetAt(i)){
		case 'C':
			id=atol(act.Mid(i+1,3));
			if(!LoadConfig( GConfigFile.GetBuffer(128), id,0,0)){
				str2="Load ";
				str2+=GConfigFile.GetBuffer(128);
				str2+=" Failed.";
				AddMsg(ID_IMG_ERR,str2.GetBuffer(255),NULL,NULL,NULL);
			}
			break;
		case 'E':
			id=atol(act.Mid(i+1,3));
			Comm[id].Enable=1;
			str2.Format("Enabled <COMM> %d",id);
			if(GWatchList)	AddMsg(ID_IMG_NOTE,str2.GetBuffer(255),NULL,NULL,NULL);
			break;
		case 'D':
			id=atol(act.Mid(i+1,3));
			Comm[id].Enable=0;
			str2.Format("Disabled <COMM> %d",id);
			if(GWatchList)	AddMsg(ID_IMG_NOTE,str2.GetBuffer(255),NULL,NULL,NULL);
			break;
		}
	}

	return 1;
}

//BOOL CVirtualCarWnd::GetLastConfigFile(CString & file)
//{
//	CHAR buf[255];
//	HKEY hkey;
//	DWORD size=255;
//	DWORD type=REG_SZ;
//
//	if(RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Kingtec\\VirtualCar",&hkey) == ERROR_SUCCESS) {
//		RegQueryValueEx(hkey, "LastConfigFile", NULL, &type, (LPBYTE)buf, &size);
//		RegCloseKey(hkey);
//
//		file=buf;
//		return TRUE;
//	}
//	return FALSE;
//}

//BOOL CVirtualCarWnd::SetLastConfigFile(CHAR * file)
//{
//	HKEY hkey;
//	
//	//在这里重新打开XML文件句柄以便检测文件大小
//	if(m_fileForAutoLoad.m_hFile!=m_fileForAutoLoad.hFileNull){
//		m_fileForAutoLoad.Close();
//	}
//	m_fileForAutoLoad.Open(file,CFile::modeRead|CFile::shareDenyNone|CFile::typeBinary);
//	
//
//
//	if(RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Kingtec\\VirtualCar",&hkey) == ERROR_SUCCESS){
//	//if(RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE",&hkey) == ERROR_SUCCESS) {
//		RegSetValueEx(hkey, "LastConfigFile", 0, REG_SZ, (BYTE *)file, strlen(file));
//		RegCloseKey(hkey);
//
//		return TRUE;
//	}
//
//	return FALSE;
//}

VOID GDoEvents(int nCount)
{
	MSG msg; 
	for (int i = 0; i < nCount; i++){
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
	}
}

BOOL CVirtualCarWnd::DestroyWindow() 
{
	//Close file for check autoload
	if(m_fileForAutoLoad.m_hFile!=m_fileForAutoLoad.hFileNull){
		m_fileForAutoLoad.Close();
	}


	//Let thread exit...
	DWORD t=GetTickCount()+6000;
	GRTS=RTS_EXSITING;
	while(GRTS==RTS_EXSITING){
		GDoEvents(1);
		if(t<GetTickCount()){
			if(HThread){
				TerminateThread(HThread,0);
				TRACE("Error, Forcibliy terminal rev thread\r\n");
			}
			
			TRACE("Error, close thread timeout.\r\n");
			break;
		}
	}




	DestroyIcon(HNote);
	DestroyIcon(HCar);
	DestroyIcon(HDiagnotor);
	DestroyIcon(HErr);


	if(HLogFile!=INVALID_HANDLE_VALUE){
		CloseHandle(HLogFile);
	}

	GComm.CloseComm();



	
	//Release for Taskbar icon
	NOTIFYICONDATA nc;
	nc.cbSize=sizeof(NOTIFYICONDATA);
	nc.hWnd=m_hWnd;
	nc.uCallbackMessage=WM_NC;
	nc.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nc.uID=IDC_NC;
	Shell_NotifyIcon(NIM_DELETE,&nc);		


	//Release Menu Resource
	Menu.DestroyMenu();
	TaskBarIconMenu.DestroyMenu();

	//Release Mutex
	ReleaseMutex(ProgramMutex);






	return CDialog::DestroyWindow();
}

void CVirtualCarWnd::OnDestroy() 
{
	CDialog::OnDestroy();
}

LRESULT CVirtualCarWnd::OnNc(WPARAM wParam,LPARAM IParam)
{   
	if(IParam==WM_LBUTTONDOWN)
    {
		ShowWindow(SW_SHOWNORMAL);
		NOTIFYICONDATA nc;
		nc.cbSize=sizeof(NOTIFYICONDATA);
		nc.hWnd=m_hWnd;
		nc.uCallbackMessage=WM_NC;
		nc.uFlags=NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nc.uID=IDC_NC;
		Shell_NotifyIcon(NIM_DELETE,&nc);
		::SetWindowPos(GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetForegroundWindow();
	}
	if(IParam==WM_RBUTTONDOWN)
	{
		//CMenu* menu;
		//menu = new CMenu();
		//menu->LoadMenu(IDR_MENU_POPUP);
		//CMenu* pPopup=menu->GetSubMenu(0);
		
		CMenu * pPopup=TaskBarIconMenu.GetSubMenu(0);
		::SetMenuDefaultItem(pPopup->m_hMenu,0,TRUE); //设置粗体字
		CPoint Point;
		GetCursorPos(&Point);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN,Point.x,Point.y,AfxGetMainWnd(),NULL);
	}

	return 1 ;
}

void CVirtualCarWnd::OnMenuitemClose() 
{
	//Save window width/height to config file
	CDTXML cfg;
	CString path;
	CString modulepath;
	::GetModuleFileName(NULL,modulepath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	modulepath.ReleaseBuffer();
	GetPathFromFileName(path,modulepath.GetBuffer(MAX_PATH));
	path+=STR_APP_CONFIG_FILE;
	if(cfg.Open(path)){
		RECT rc;
		GetWindowRect(&rc);
		
		cfg.Enter("TOP");
		cfg.Enter("MAINWND");
		
		WINDOWPLACEMENT p;
		GetWindowPlacement(&p);
		if(p.showCmd==SW_SHOWMAXIMIZED){
			cfg.SetInteger("MAXIMIZED",1);
		}else if(p.showCmd==SW_SHOWNORMAL){
			cfg.SetInteger("MAXIMIZED",0);
			cfg.SetInteger("WIDTH",rc.right-rc.left);
			cfg.SetInteger("HEIGHT",rc.bottom-rc.top);
			cfg.SetInteger("X",rc.left);
			cfg.SetInteger("Y",rc.top);
		}
		cfg.Back();
		
		cfg.SetString("LASTCONFIGFILE",GConfigFile);
		cfg.Save();
		cfg.Close();
	}

	OnOK();

}

void CVirtualCarWnd::OnMenuitemAbout() 
{
	CDlgAbout dlg;
	dlg.DoModal();

}

void CVirtualCarWnd::OnMenuitemEditConfig() 
{

	char buf[512];

	if(GetUltraEditorPath(buf,512)){
		strcat(buf," ");
		strcat(buf,GConfigFile.GetBuffer(MAX_PATH));
		if(WinExec(buf,1)>31){
			return;
		}
	}


	sprintf(buf,"notepad.exe %s", GConfigFile.GetBuffer(MAX_PATH));
	if(WinExec(buf,1)>31){
		return;
	}

	
	ShellExecute(NULL,"edit",GConfigFile,NULL,NULL,SW_SHOWNORMAL);	
}

void CVirtualCarWnd::OnMenuitemViewLog() 
{
	char buf[512];

	CString logpath;
	GetPathFromFileName(logpath,GConfigFile);
	logpath+=GLogFile;


	if(GetUltraEditorPath(buf,512)){
		strcat(buf," ");
		strcat(buf,logpath.GetBuffer(MAX_PATH));
		if(WinExec(buf,1)>31){
			return;
		}
	}


	sprintf(buf,"notepad.exe %s",logpath.GetBuffer(MAX_PATH));
	if(WinExec(buf,1)>31){
		return;
	}
	
	ShellExecute(NULL,"edit",logpath,NULL,NULL,SW_SHOWNORMAL);	
}


void CVirtualCarWnd::OnMenuitemClearList() 
{
	m_list.DeleteAllItems();
	GInBufferCount=0;
}


void CVirtualCarWnd::OnMenuitemLoadConfig() 
{
	CString str;
	CFileDialog dlg(1,NULL,NULL,NULL,NULL,NULL);


	CString strPath;
	
	::GetModuleFileName(NULL,strPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos=strPath.ReverseFind('\\');
	strPath=strPath.Left(nPos);
	//CString strFileName = strPath.Right(strPath.GetLenth() - nPos -1);


	dlg.m_ofn.lpstrInitialDir=strPath;
	dlg.m_ofn.lpstrFilter="虚拟汽车配置文件(*.xml)\0*.xml\0";
	dlg.DoModal();
	
	if(strlen(dlg.m_ofn.lpstrFile)<2){
		return;
	}


	//设置m_cfglastsize=0，则自动检测更新时不会得新装文件.
	m_cfglastsize=0;

	if(!LoadConfig(dlg.m_ofn.lpstrFile,0,1,1)){
		str.Format("Load %s failed",dlg.m_ofn.lpstrFile);
		AddMsg(ID_IMG_ERR,str.GetBuffer(255),NULL,NULL,NULL);
	}

	//线程中的计时清零,使得收取新的帧
	GLastReceiveByteTime=0;
	GAutoreplyLastTime=(-((double)GAutoReplySpace));	//清零后,使得在手动装载config后可立即发送一帧.

}


void CVirtualCarWnd::OnCheckWatchList() 
{
	GWatchList=!GWatchList;
	if(GWatchList){
		CheckMenuItem(Menu,ID_MENUITEM_WATCH_LIST,MF_CHECKED);
		CheckDlgButton(IDC_CHECK_WATCH_LIST,1);
	}else{
		CheckMenuItem(Menu,ID_MENUITEM_WATCH_LIST,MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_WATCH_LIST,0);
	}
	
}

void CVirtualCarWnd::OnCheckLogFile() 
{
	GLogFileEnable=!GLogFileEnable;
	if(GLogFileEnable){
		CheckMenuItem(Menu, ID_MENUITEM_LOGOPEN, MF_CHECKED);
		CheckDlgButton(IDC_CHECK_LOG_FILE,1);
	}else{
		CheckMenuItem(Menu, ID_MENUITEM_LOGOPEN, MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_LOG_FILE,0);
	}
}

/*
void CVirtualCarWnd::OnCheckRealtime() 
{
	UpdateData(TRUE);
}

void CVirtualCarWnd::OnChangeEditSleep() 
{
	UpdateData(TRUE);
}
*/

void CVirtualCarWnd::OnMenuitemReloadConfig() 
{
	CString str;

	//设置m_cfglastsize=0，则自动检测更新时不会得新装文件.
	m_cfglastsize=0;

	if(!LoadConfig(GConfigFile.GetBuffer(255),0,1,1)){
		str.Format("Load %s failed",GConfigFile);
		AddMsg(ID_IMG_ERR,str.GetBuffer(255),NULL,NULL,NULL);
	}

	//线程中的计时清零,使得收取新的帧
	GLastReceiveByteTime=0;
	GAutoreplyLastTime=(-((double)GAutoReplySpace));	//清零后,使得在手动装载config后可立即发送一帧.
}


void CVirtualCarWnd::OnCheckRemoveSelfSend()
{
	GFilterSelfSent=!GFilterSelfSent;
	if(GFilterSelfSent){
		CheckMenuItem(Menu,ID_MENUITEM_FILTER_SELF_SENT,MF_CHECKED);
		CheckDlgButton(IDC_CHECK_REMOVE_SELF_SEND,1);
	}else{
		CheckMenuItem(Menu,ID_MENUITEM_FILTER_SELF_SENT,MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_REMOVE_SELF_SEND,0);
	}
}


void CVirtualCarWnd::SendTempData(CString & str)
{
	char buffer[1024*16];
	unsigned char bytebuf[1024*8];
	UINT bytebuflen;
	unsigned char bytebuf2[1024*16];	//512*2+4
	INT bytebuflen2;
	int j;

	if(!GComm.IsOpen()){
		AddMsg(ID_IMG_ERR,"Serial port is not open","","","");
		return;
	}

	str.Remove(' ');	
	str.Remove(',');
	str.Remove('h');
	str.Remove('H');
	str.Replace("0x","");
	str.Remove('\r') ;
	str.Remove('\n') ;


	if(str.GetLength()<1){
		return;
	}

	//Gen rand data for ??
	while((j=str.Find("?",0))!=-1){
		str.SetAt(j,GenRandHexChar());
	}

	HexString2Buffer(str,bytebuf,1024*8,&bytebuflen);

	if(bytebuflen<=0){
		return;
	}



	//打包/发送
	if(GFrameType==1){//VCX
		bytebuflen2=KT600Pack(bytebuf,bytebuflen,bytebuf2,1024*16);
		if(bytebuflen2<=0){
			ASSERT("VCXPack failed\r\n");
		}
		GComm.WriteComm(bytebuf2,bytebuflen2);				
	}else if(GFrameType==2){//KT600	
		bytebuflen2=VCXPack(bytebuf,bytebuflen,bytebuf2,1024*16);
		if(bytebuflen2<=0){
			ASSERT("VCXPack failed\r\n");
		}
		GComm.WriteComm(bytebuf2,bytebuflen2);				
	}else if(GFrameType==3) // vcxplus
	{
		bytebuflen2=VCXPlusPack(bytebuf,bytebuflen,bytebuf2,1028);
		if(bytebuf2<=0){
			ASSERT("VCXPlus Pack failed");
		}
		GComm.WriteComm(bytebuf2,bytebuflen2) ;
	}else{//no pack
		GComm.WriteComm(bytebuf,bytebuflen);				
	}



	if(GWatchList || GLogFileEnable){
		Buffer2HexString(bytebuf,bytebuflen,buffer,1024*16,""," ");
	}

	//int ll=strlen(buffer);

	if(GLogFileEnable){
		LogToFile(1,buffer);
	}


	GTempTickcount=GetTickCountEx();
	if(GWatchList){
		str.Format("%.3f",GTempTickcount-GLastProcessFrameTimetime);
		AddMsg(ID_IMG_CAR,buffer,NULL,"临时", str.GetBuffer(255));
	}
	GLastProcessFrameTimetime=GTempTickcount;


	//收掉自发自收的字节.
	if(GFilterSelfSent){
		GComm.ReadComm(bytebuf2,bytebuflen2,NULL);
	}


}

void CVirtualCarWnd::OnButtonSend() 
{
	CString str;
	GetDlgItem(IDC_INPUT_TEMP)->GetWindowText(str);
	SendTempData(str);
}

void CVirtualCarWnd::OnCheckAutoReplyByTemp() 
{
	if(((CButton*)GetDlgItem(IDC_CHECK_AUTO_REPLY_BY_TEMP))->GetCheck()){
		GAutoReplyByTemp=1;
	}else{
		GAutoReplyByTemp=0;
	}
}

/*
void CVirtualCarWnd::OnRadioTimerSendClose() 
{
	if(((CButton*)GetDlgItem(IDC_RADIO_TIMER_SEND_CLOSE))->GetCheck()){
		GAutoReplyEnable=0;
		GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->EnableWindow(FALSE);
	}
	

}
*/

void CVirtualCarWnd::OnRadioTimerFile() 
{
	if(((CButton*)GetDlgItem(IDC_CHECK_TIMER_REPLY))->GetCheck()==0){
		return;
	}

	
	if(((CButton*)GetDlgItem(IDC_RADIO_TIMER_FILE ))->GetCheck()){
		GAutoReplyEnable=1;
		GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->EnableWindow(TRUE);
	}
}

void CVirtualCarWnd::OnRadioTimerTemp() 
{
	if(((CButton*)GetDlgItem(IDC_CHECK_TIMER_REPLY))->GetCheck()==0){
		return;
	}

	if(((CButton*)GetDlgItem(IDC_RADIO_TIMER_TEMP ))->GetCheck()){
		GAutoReplyEnable=2;
		GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->EnableWindow(TRUE);
	}
}



void CVirtualCarWnd::OnMenuitemCleanLog() 
{
	if(MessageBox("您想清除日志文件吗？","请确认",MB_OKCANCEL)==IDCANCEL){
		return;
	}

	GLogMutex.Lock();

	if(HLogFile!=INVALID_HANDLE_VALUE){
		CloseHandle(HLogFile);
		HLogFile=INVALID_HANDLE_VALUE;
	}


	CString logpath;
	GetPathFromFileName(logpath,GConfigFile);
	logpath+=GLogFile;
	HLogFile=CreateFile(logpath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,NULL,NULL);
	if(HLogFile==INVALID_HANDLE_VALUE){
		MessageBox("Can not clean log file\r\n");
		//return 0;
	}


	GLogMutex.Unlock();
}

void CVirtualCarWnd::OnMenuitemLogTime() 
{
	m_LogTime=!m_LogTime;

	if(m_LogTime){
		CheckMenuItem(Menu, ID_MENUITEM_LOG_TIME, MF_CHECKED);
	}else{
		CheckMenuItem(Menu, ID_MENUITEM_LOG_TIME, MF_UNCHECKED);
	}
}

void CVirtualCarWnd::OnOutofmemorySliderSleep(NMHDR* pNMHDR, LRESULT* pResult) 
{


	*pResult = 0;
}

void CVirtualCarWnd::OnReleasedcaptureSliderSleep(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	GSleep=m_SliderSleep.GetPos();
	m_SleepCaption.Format("%d",m_SliderSleep.GetPos());
	UpdateData(FALSE);
	*pResult = 0;
}

void CVirtualCarWnd::OnReleasedcaptureSliderPriority(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if(HThread==NULL){
		return;
	}
	
	INT slider=m_SliderPriority.GetPos();
	SetThreadPriority(HThread,PrioritySliderValue2APIValue(slider));
	m_PriorityCaption=PrioritySliderValue2Description(slider);

	UpdateData(FALSE);
}

void CVirtualCarWnd::OnButtonCleanList() 
{
	OnMenuitemClearList();
}

void CVirtualCarWnd::OnButtonCleanLog() 
{
	OnMenuitemCleanLog();	
}

void CVirtualCarWnd::OnMenuitemLogopen() 
{
	// TODO: Add your command handler code here
	GLogFileEnable=!GLogFileEnable;
	if(GLogFileEnable){
		CheckMenuItem(Menu, ID_MENUITEM_LOGOPEN, MF_CHECKED);
		CheckDlgButton(IDC_CHECK_LOG_FILE,1);
	}else{
		CheckMenuItem(Menu, ID_MENUITEM_LOGOPEN, MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_LOG_FILE,0);
	}
}

void CVirtualCarWnd::OnMenuitemWatchList() 
{
	GWatchList=!GWatchList;
	if(GWatchList){
		CheckMenuItem(Menu,ID_MENUITEM_WATCH_LIST,MF_CHECKED);
		CheckDlgButton(IDC_CHECK_WATCH_LIST,1);
	}else{
		CheckMenuItem(Menu,ID_MENUITEM_WATCH_LIST,MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_WATCH_LIST,0);
	}	
}

void CVirtualCarWnd::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CVirtualCarWnd::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW * pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
		
		//CMenu menu;
		//VERIFY( menu.LoadMenu( IDR_MENU_LIST_RCLICK ) );
		//CMenu* popup = menu.GetSubMenu(0);
		//ASSERT( popup != NULL );


		//复制到剪切板
		CString source;
		source=m_list.GetItemText(pNMListView->iItem,pNMListView->iSubItem);
		CopyTextToClipboard(m_hWnd,source.GetBuffer(0));



		//popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	} 

	*pResult = 0;
}

void CVirtualCarWnd::OnButtonReload() 
{
	// TODO: Add your control notification handler code here
	OnMenuitemReloadConfig();
}

void CVirtualCarWnd::WatchSerialPortState()
{
	if(!GComm.IsOpen()){
		return;
	}

	char * msg;
	COMSTAT comstat;
	DWORD	err;

	if(GComm.ClearCommError(&err,&comstat)){
		switch(err){
		case CE_BREAK:
			msg="CE_BREAK The hardware detected a break condition.";
			break;
		case CE_DNS:
			msg="CE_DNS Windows 95 and Windows 98: A parallel device is not selected.";
			break;
		case CE_FRAME:
			msg="CE_FRAME The hardware detected a framing error.";
			break;
		case CE_IOE:
			msg="CE_IOE An I/O error occurred during communications with the device.";
			break;
		case CE_MODE:
			msg="CE_MODEThe requested mode is not supported, or the hFile parameter is invalid. If this value is specified, it is the only valid error.";
			break;
		case CE_OOP:
			msg="CE_OOP Windows 95 and Windows 98: A parallel device signaled that it is out of paper.";
			break;
		case CE_OVERRUN:
			msg="CE_OVERRUN A character-buffer overrun has occurred. The next character is lost.";
			break;
		case CE_PTO:
			msg="CE_PTO Windows 95 and Windows 98: A time-out occurred on a parallel device.";
			break;
		case CE_RXOVER:
			msg="CE_RXOVER An input buffer overflow has occurred. There is either no room in the input buffer, or a character was received after the end-of-file (EOF) character.";
			//PurgeComm(GHComm,PURGE_RXCLEAR);
			//msg="清空,丢失一些数据";
			break;
		case CE_RXPARITY:
			msg="CE_RXPARITY The hardware detected a parity error.";
			break;
		case CE_TXFULL:
			msg="CE_TXFULL The application tried to transmit a character, but the output buffer was full.";
			break;
		default:
			msg = "Unknow error information." ;
			break ;
		}
		
		if(err){
			if(m_ShowCommErr && msg != NULL){
				AddMsg(ID_IMG_ERR,msg,NULL,NULL,NULL);
			}
		}
	}

	m_ProgressInput.SetPos(comstat.cbInQue);
	m_ProgressOutput.SetPos(comstat.cbOutQue);
}

void CVirtualCarWnd::WatchCurConfigFileChange()
{
	//自动装载
	static CFileStatus fs;

	if(m_AutoLoad){
		if(m_fileForAutoLoad.m_hFile!=m_fileForAutoLoad.hFileNull){
			if(m_cfglastsize==0){
				m_fileForAutoLoad.GetStatus(fs);
				m_cfglastsize=fs.m_size;
				m_cfglasttime=fs.m_mtime;
			}else{
				m_fileForAutoLoad.GetStatus(fs);
 				if(m_cfglastsize!= fs.m_size || m_cfglasttime!=fs.m_mtime){
					OnMenuitemReloadConfig();
					Beep(3200,50);
					Beep(2200,50);
				}
				m_cfglastsize=fs.m_size;
				m_cfglasttime=fs.m_mtime;
			}
		}else{
			
		}
	}
}


void CVirtualCarWnd::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==ID_TIMER_WATCH_CFGFILECHANGE){
		WatchSerialPortState();
		WatchCurConfigFileChange();
		return;
	}
	CDialog::OnTimer(nIDEvent);
}

void CVirtualCarWnd::OnCheckShowCommErr() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	
}

void CVirtualCarWnd::OnCheckReplaceSendData() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	
	if(m_CheckReplaceSendData){
		GetDlgItem(IDC_EDIT_REPLACE_POS1)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_REPLACE_TO1)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_REPLACE_SYMBOL)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_EDIT_REPLACE_POS1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_REPLACE_TO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_REPLACE_SYMBOL)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);
}






void CVirtualCarWnd::OnOutofmemorySpinPos1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CVirtualCarWnd::OnDeltaposSpinPos1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	*pResult = 0;

	m_ReplacePosNum1+=-pNMUpDown->iDelta;
	m_ReplacePos1.Format("%02x",m_ReplacePosNum1);
	UpdateData(FALSE);

}

void CVirtualCarWnd::OnDeltaposSpinPos2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;

	m_ReplacePosNum2+=-pNMUpDown->iDelta;
	m_ReplacePos2.Format("%02x",m_ReplacePosNum2);
	UpdateData(FALSE);


}

void CVirtualCarWnd::OnDeltaposSpinTo1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;

	m_ReplaceToNum1+=-pNMUpDown->iDelta;
	m_ReplaceTo1.Format("%02x",m_ReplaceToNum1);
	UpdateData(FALSE);

}

void CVirtualCarWnd::OnDeltaposSpinTo2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
	m_ReplaceToNum2+=-pNMUpDown->iDelta;
	m_ReplaceTo2.Format("%02x",m_ReplaceToNum2);
	UpdateData(FALSE);

}

void CVirtualCarWnd::OnCheckTimerReply() 
{
	// TODO: Add your control notification handler code here
	if(((CButton*)GetDlgItem(IDC_CHECK_TIMER_REPLY))->GetCheck()==0){
		
		GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_TIMER_FILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_TIMER_TEMP)->EnableWindow(FALSE);
		
		GAutoReplyEnable=0;
		
		return;
	}

	
	if(((CButton*)GetDlgItem(IDC_RADIO_TIMER_FILE ))->GetCheck()){
		GAutoReplyEnable=1;
		GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->EnableWindow(TRUE);

		GetDlgItem(IDC_RADIO_TIMER_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TIMER_TEMP)->EnableWindow(TRUE);
	}else if(((CButton*)GetDlgItem(IDC_RADIO_TIMER_TEMP ))->GetCheck()){
		GAutoReplyEnable=2;
		GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->EnableWindow(TRUE);

		GetDlgItem(IDC_RADIO_TIMER_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TIMER_TEMP)->EnableWindow(TRUE);
	}

}


void CVirtualCarWnd::OnEditchangeComboReplaceCheckType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

}

void CVirtualCarWnd::OnSelchangeComboReplaceCheckType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
}

void CVirtualCarWnd::OnChangeEditReplaceSymbol() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}


void CVirtualCarWnd::OnCheckReplaceCheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(m_ReplaceCheck){
		GetDlgItem(IDC_EDIT_REPLACE_CHECK_SYMBOL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_REPLACE_CHECK_TYPE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CHECKOUT_BEGIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CHECKOUT_END)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CHECKOUT_TARGET)->EnableWindow(TRUE);
	
	}else{
		GetDlgItem(IDC_EDIT_REPLACE_CHECK_SYMBOL)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_REPLACE_CHECK_TYPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CHECKOUT_BEGIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CHECKOUT_END)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CHECKOUT_TARGET)->EnableWindow(FALSE);
	}

}



void CVirtualCarWnd::OnButtonTao() 
{	
	OnMenuitemToolTao();
}

void CVirtualCarWnd::OnMenuitemAutoload()
{
	// TODO: Add your command handler code here
	m_AutoLoad=!m_AutoLoad;

	if(m_AutoLoad){
		Menu.CheckMenuItem(ID_MENUITEM_AUTOLOAD, MF_CHECKED);
	}else{
		Menu.CheckMenuItem(ID_MENUITEM_AUTOLOAD, MF_UNCHECKED);
	}
}

void CVirtualCarWnd::OnMenuitemToolTao() 
{
	// TODO: Add your control notification handler code here
	BYTE old_m_WatchList=GWatchList;
	BOOL old_m_AutoLoad=m_AutoLoad;

	CModifyWnd dlg;
	
	GWatchList=0;
	m_AutoLoad=0;

	this->ShowWindow(SW_MINIMIZE);
	
	dlg.DoModal();

	this->ShowWindow(SW_RESTORE);
	this->SetForegroundWindow();
	
	
	GWatchList=old_m_WatchList;
	m_AutoLoad=old_m_AutoLoad;
}



void CVirtualCarWnd::OnMenuitemToolUltraeditor()
{
	char buf[512];

	if(GetUltraEditorPath(buf,512)){
		WinExec(buf,1);
	}
}




void CVirtualCarWnd::OnSize(UINT nType, int cx, int cy) 
{	
	CDialog::OnSize(nType, cx, cy);

	if(nType==SIZE_MINIMIZED){
		return;
	}

	RECT rect;
	CWnd * pwnd;

	SetRect(&rect,0,0,cx-200,cy-60);
	pwnd=GetDlgItem(IDC_LIST);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,5,cy-55,cx-200,cy-5);
	pwnd=GetDlgItem(IDC_STATIC_TEMP_FRAME);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-385,cy-55,cx-210,cy-35);
	pwnd=GetDlgItem(IDC_CHECK_AUTO_REPLY_BY_TEMP);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
		pwnd->SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}




	SetRect(&rect,10,cy-33,50,cy-13);
	pwnd=GetDlgItem(IDC_BUTTON_CLEAR_SEND);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,55,cy-33,cx-330,cy-13);
	pwnd=GetDlgItem(IDC_INPUT_TEMP);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}

	SetRect(&rect,cx-325,cy-33,cx-270,cy-11);
	pwnd=GetDlgItem(IDC_BUTTON_SEND);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}

	SetRect(&rect,cx-265,cy-33,cx-210,cy-11);
	pwnd=GetDlgItem(IDC_BUTTON_SEND_COMM);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}



	//右边控件
	SetRect(&rect,cx-195,0,cx-100,30);
	pwnd=GetDlgItem(IDC_BUTTON_RELOAD);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}

	SetRect(&rect,cx-95,0,cx-60,30);
	pwnd=GetDlgItem(IDC_CHECK_PAUSE);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-55,0,cx-5,30);
	pwnd=GetDlgItem(IDC_BUTTON_TAO);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-190,40,cx-160,55);
	pwnd=GetDlgItem(IDC_STATIC_COM1);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,TRUE);
	}
	SetRect(&rect,cx-160,38,cx-5,55);
	pwnd=GetDlgItem(IDC_EDIT_PORT_STATE);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-195,57,cx-160,75);
	pwnd=GetDlgItem(IDC_STATIC_INPUT);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-160,58,cx-105,75);
	pwnd=GetDlgItem(IDC_PROGRESS_INPUT);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-100,57,cx-65,75);
	pwnd=GetDlgItem(IDC_STATIC_OUTPUT);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-65,58,cx-5,75);
	pwnd=GetDlgItem(IDC_PROGRESS_OUTPUT);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-190,83,cx-150,98);
	pwnd=GetDlgItem(IDC_STATIC_COM4);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-150,80,cx-5,95);
	pwnd=GetDlgItem(IDC_COMBO_FRAME_TYPE);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	
	SetRect(&rect,cx-190,105,cx-5,120);
	pwnd=GetDlgItem(IDC_CHECK_SHOW_COMM_ERR);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}

	SetRect(&rect,cx-190,130,cx-5,145);
	pwnd=GetDlgItem(IDC_CHECK_REMOVE_SELF_SEND);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-190,155,cx-90,170);
	pwnd=GetDlgItem(IDC_CHECK_WATCH_LIST);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}

	SetRect(&rect,cx-90,155,cx-5,175);
	pwnd=GetDlgItem(IDC_BUTTON_CLEAN_LIST);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	
	SetRect(&rect,cx-190,180,cx-90,195);
	pwnd=GetDlgItem(IDC_CHECK_LOG_FILE);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}

	SetRect(&rect,cx-90,180,cx-5,200);
	pwnd=GetDlgItem(IDC_BUTTON_CLEAN_LOG);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	

	SetRect(&rect,cx-190,205,cx-5,280);
	pwnd=GetDlgItem(IDC_STATIC_PREFORMANCE_FRAME);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-180,230,cx-130,245);
	pwnd=GetDlgItem(IDC_STATIC_CPU1);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-130,225,cx-40,250);
	pwnd=GetDlgItem(IDC_SLIDER_SLEEP);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-40,230,cx-15,245);
	pwnd=GetDlgItem(IDC_EDIT_SLEEP);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-180,255,cx-130,270);
	pwnd=GetDlgItem(IDC_STATIC_CPU2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-130,250,cx-40,275);
	pwnd=GetDlgItem(IDC_SLIDER_PRIORITY);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-40,255,cx-15,270);
	pwnd=GetDlgItem(IDC_EDIT_PRIORITY);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-190,290,cx-5,390);
	pwnd=GetDlgItem(IDC_STATIC_TIMER_FRAME);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-180,290,cx-100,305);
	pwnd=GetDlgItem(IDC_CHECK_TIMER_REPLY);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}

	SetRect(&rect,cx-180,315,cx-15,330);
	pwnd=GetDlgItem(IDC_RADIO_TIMER_FILE);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-180,340,cx-150,355);
	pwnd=GetDlgItem(IDC_STATIC_TIMER_ID);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-150,340,cx-125,355);
	pwnd=GetDlgItem(IDC_EDIT_AUTO_REPLY_ID);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-105,340,cx-80,355);
	pwnd=GetDlgItem(IDC_STATIC_TIMER_INTERVAL);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-75,340,cx-15,355);
	pwnd=GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	
	SetRect(&rect,cx-180,365,cx-15,380);
	pwnd=GetDlgItem(IDC_RADIO_TIMER_TEMP);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-190,400,cx-5,495);
	pwnd=GetDlgItem(IDC_STATIC_CHECK_FRAME);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-180,400,cx-85,415);
	pwnd=GetDlgItem(IDC_CHECK_REPLACE_CHECK);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-70,400,cx-45,415);
	pwnd=GetDlgItem(IDC_STATIC_CHECK_SYMBOL);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-40,400,cx-15,415);
	pwnd=GetDlgItem(IDC_EDIT_REPLACE_CHECK_SYMBOL);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}



	SetRect(&rect,cx-180,425,cx-130,440);
	pwnd=GetDlgItem(IDC_STATIC_CHECK_BEGINPOS);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-120,425,cx-70,440);
	pwnd=GetDlgItem(IDC_STATIC_CHECK_ENDPOS);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-55,425,cx-15,440);
	pwnd=GetDlgItem(IDC_STATIC_CHECK_TARGETPOS);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-180,425,cx-135,440);
	pwnd=GetDlgItem(IDC_STATIC_CHECK_BEGINPOS);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-120,425,cx-75,440);
	pwnd=GetDlgItem(IDC_STATIC_CHECK_ENDPOS);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-55,425,cx-15,440);
	pwnd=GetDlgItem(IDC_STATIC_CHECK_TARGETPOS);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}

	SetRect(&rect,cx-180,440,cx-135,455);
	pwnd=GetDlgItem(IDC_EDIT_CHECKOUT_BEGIN);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-120,440,cx-75,455);
	pwnd=GetDlgItem(IDC_EDIT_CHECKOUT_END);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-55,440,cx-15,455);
	pwnd=GetDlgItem(IDC_EDIT_CHECKOUT_TARGET);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-180,465,cx-150,480);
	pwnd=GetDlgItem(IDC_STATIC_CHECKTYPE);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-150,465,cx-15,480);
	pwnd=GetDlgItem(IDC_COMBO_REPLACE_CHECK_TYPE);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-190,505,cx-5,560);
	pwnd=GetDlgItem(IDC_STATIC_REPLACE_FRAME);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-180,505,cx-85,520);
	pwnd=GetDlgItem(IDC_CHECK_REPLACE_SEND_DATA);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-70,505,cx-45,520);
	pwnd=GetDlgItem(IDC_STATIC_REPLACE_SYMBOL);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-40,505,cx-15,520);
	pwnd=GetDlgItem(IDC_EDIT_REPLACE_SYMBOL);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-180,530,cx-150,545);
	pwnd=GetDlgItem(IDC_STATIC_RPOS1);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-150,530,cx-125,545);
	pwnd=GetDlgItem(IDC_EDIT_REPLACE_POS1);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-125,525,cx-105,550);
	pwnd=GetDlgItem(IDC_SPIN_POS1);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-90,530,cx-60,545);
	pwnd=GetDlgItem(IDC_STATIC_RAS1);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-60,530,cx-35,545);
	pwnd=GetDlgItem(IDC_EDIT_REPLACE_TO1);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-35,525,cx-15,550);
	pwnd=GetDlgItem(IDC_SPIN_TO1);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}




	SetRect(&rect,cx-190,570,cx-5,625);
	pwnd=GetDlgItem(IDC_STATIC_REPLACE_FRAME2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-180,570,cx-85,585);
	pwnd=GetDlgItem(IDC_CHECK_REPLACE_SEND_DATA2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-70,570,cx-45,585);
	pwnd=GetDlgItem(IDC_STATIC_REPLACE_SYMBOL2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-40,570,cx-15,585);
	pwnd=GetDlgItem(IDC_EDIT_REPLACE_SYMBOL2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}


	SetRect(&rect,cx-180,595,cx-150,610);
	pwnd=GetDlgItem(IDC_STATIC_RPOS2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-150,595,cx-125,610);
	pwnd=GetDlgItem(IDC_EDIT_REPLACE_POS2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-125,590,cx-105,615);
	pwnd=GetDlgItem(IDC_SPIN_POS2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}



	SetRect(&rect,cx-90,595,cx-60,610);
	pwnd=GetDlgItem(IDC_STATIC_RAS2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-60,595,cx-35,610);
	pwnd=GetDlgItem(IDC_EDIT_REPLACE_TO2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-35,590,cx-15,615);
	pwnd=GetDlgItem(IDC_SPIN_TO2);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}



	//IDC_CHECK_DISABLE_REPAY
	SetRect(&rect,cx-180,635,cx-85,650);
	pwnd=GetDlgItem(IDC_CHECK_DISABLE_REPAY);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-70,635,cx-45,650);
	pwnd=GetDlgItem(IDC_STATIC_DISABLE_REPAY_SYMBOL);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	SetRect(&rect,cx-40,635,cx-15,650);
	pwnd=GetDlgItem(IDC_EDIT_DISABLE_REPAY_SYMBOL);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
	

	
	SetRect(&rect,cx-190,650,cx-5,cy-5);
	pwnd=GetDlgItem(IDC_STATIC_MORE_FRAME);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}
		
	
	RedrawWindow(NULL);
}

void CVirtualCarWnd::OnSizing(UINT fwSide, LPRECT pRect) 
{
	
	CDialog::OnSizing(fwSide, pRect);

	if(pRect->right-pRect->left<490){ 
		pRect->right=pRect->left+490;
	}
	if(pRect->bottom-pRect->top<330){ 
		pRect->bottom=pRect->top+330;
	}
}

void CVirtualCarWnd::OnCheckReplaceSendData2() 
{
	UpdateData(TRUE);

	if(m_CheckReplaceSendData2){
		GetDlgItem(IDC_EDIT_REPLACE_POS2)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_REPLACE_TO2)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_REPLACE_SYMBOL2)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_EDIT_REPLACE_POS2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_REPLACE_TO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_REPLACE_SYMBOL2)->EnableWindow(FALSE);
	}	
}


VOID CVirtualCarWnd::AddMsg(INT img_id, char * str0, char * str1, char * str2, char *str3)
{
	int id;
	id=m_list.GetItemCount();

	if(id>GMaxListRowCount){
		m_list.DeleteAllItems();
		id=m_list.GetItemCount();
	}

	TRACE1("str0 = %s\r\n", str0);

	if(img_id){
		m_list.InsertItem(id,str0,img_id);
	}else{
		m_list.InsertItem(id,str0);
	}

	m_list.SetItemData(id,img_id);
	
	if(str1) m_list.SetItemText(id,1,str1);
	if(str2) m_list.SetItemText(id,2,str2);
	if(str3) m_list.SetItemText(id,3,str3);


	m_list.EnsureVisible(id,FALSE);
}


void CVirtualCarWnd::OnMenuitemParas() 
{
	// TODO: Add your command handler code here
	CDlgParas dlg;
	dlg.pmainwin=this;
	dlg.DoModal();
}

BOOL IsFileExist(LPCTSTR lpFileName)   
{   
	if(lpFileName==NULL)   
		return   FALSE;   
	BOOL   bExist   =   TRUE;   
	HANDLE   hFind;   
	WIN32_FIND_DATA   dataFind;   
	BOOL   bMoreFiles   =   TRUE;   
	hFind   =   FindFirstFile(lpFileName,&dataFind);   
	if(hFind==   INVALID_HANDLE_VALUE)
	{   
		bExist   =   FALSE;   
	}   
	FindClose(hFind);   
	return   bExist;   
}   

void CVirtualCarWnd::OnMenuitemFilterSelfSent() 
{
	GFilterSelfSent=!GFilterSelfSent;
	if(GFilterSelfSent){
		CheckMenuItem(Menu,ID_MENUITEM_FILTER_SELF_SENT,MF_CHECKED);
		CheckDlgButton(IDC_CHECK_REMOVE_SELF_SEND,1);
	}else{
		CheckMenuItem(Menu,ID_MENUITEM_FILTER_SELF_SENT,MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_REMOVE_SELF_SEND,0);
	}
}


void CVirtualCarWnd::OnMenuitemNewConfig()
{
	CDlgWizardNewFile * PDlgWizardNewFile=NULL;
	
	PDlgWizardNewFile=new CDlgWizardNewFile();
	if(PDlgWizardNewFile==NULL){
		return;
	}

	GWizardDlgResult=ID_WIZARD_CANCEL;
	PDlgWizardNewFile->DoModal();
	delete PDlgWizardNewFile;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}





	CDlgWizardTitle * PDlgWizardTitle=NULL;
	PDlgWizardTitle=new CDlgWizardTitle();
	if(PDlgWizardTitle==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	GWizardConfigID=0;
	PDlgWizardTitle->DoModal();
	delete PDlgWizardTitle;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}




	CDlgWizardPerfomance * PDlgWizardPerformance=NULL;
	PDlgWizardPerformance=new CDlgWizardPerfomance();
	if(PDlgWizardPerformance==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardPerformance->DoModal();
	delete PDlgWizardPerformance;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}
	



	CDlgWizardCOM * PDlgWizardCOM=NULL;
	PDlgWizardCOM=new CDlgWizardCOM();
	if(PDlgWizardCOM==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardCOM->DoModal();
	delete PDlgWizardCOM;
	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}




	CDlgWizardFrame * PDlgWizardFrame=NULL;
	PDlgWizardFrame=new CDlgWizardFrame();
	if(PDlgWizardFrame==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardFrame->DoModal();
	delete PDlgWizardFrame;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}


	
	CDlgWizardCheck * PDlgWizardCheck=NULL;
	PDlgWizardCheck=new CDlgWizardCheck();
	if(PDlgWizardCheck==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardCheck->DoModal();
	delete PDlgWizardCheck;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}


	
	CDlgWizardTimer * PDlgWizardTimer=NULL;
	PDlgWizardTimer=new CDlgWizardTimer();
	if(PDlgWizardTimer==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardTimer->DoModal();
	delete PDlgWizardTimer;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}


	
	CDlgWizardComm * PDlgWizardComm =NULL;
	PDlgWizardComm=new CDlgWizardComm();
	if(PDlgWizardComm==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardComm->DoModal();
	delete PDlgWizardComm;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}

	//设置m_cfglastsize=0，则自动检测更新时不会得新装文件.
	m_cfglastsize=0;

	if(!LoadConfig(GWizardXMLFileName,GWizardConfigID,1,1)){
		CString str;
		str.Format("Load %s failed",GWizardXMLFileName);
		AddMsg(ID_IMG_ERR,str.GetBuffer(255),NULL,NULL,NULL);
	}

	//线程中的计时清零,使得收取新的帧
	GLastReceiveByteTime=0;
	GAutoreplyLastTime=(-((double)GAutoReplySpace));	//清零后,使得在手动装载config后可立即发送一帧.

}




void CVirtualCarWnd::OnKillfocusEditAutoReplySpace() 
{
	CString str;
	GetDlgItem(IDC_EDIT_AUTO_REPLY_SPACE)->GetWindowText(str);
	GAutoReplySpace=atoi(str.GetBuffer(32));
	
}

void CVirtualCarWnd::OnMenuitemModifyAppConfig() 
{
	char buf[512];
	CString path;
	CString modulepath;
	::GetModuleFileName(NULL,modulepath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	modulepath.ReleaseBuffer();
	GetPathFromFileName(path,modulepath.GetBuffer(MAX_PATH));
	path+=STR_APP_CONFIG_FILE;

	if(GetUltraEditorPath(buf,512)){
		strcat(buf," ");
		strcat(buf,path.GetBuffer(MAX_PATH));
		if(WinExec(buf,1)>31){
			return;
		}
	}


	sprintf(buf,"notepad.exe %s", path.GetBuffer(MAX_PATH));
	if(WinExec(buf,1)>31){
		return;
	}

	
	ShellExecute(NULL,"edit",path,NULL,NULL,SW_SHOWNORMAL);		
}

void CVirtualCarWnd::OnMenuitemImportComm() 
{
	CDlgWizardComm * PDlgWizardComm =NULL;
	PDlgWizardComm=new CDlgWizardComm();
	if(PDlgWizardComm==NULL){
		return;
	}

	GWizardXMLFileName=GConfigFile;
	//GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardComm->DoModal();
	delete PDlgWizardComm;
}

void CVirtualCarWnd::OnMenuitemInsertCfg() 
{
	CString msg;

	if(!IsFileExist(GConfigFile)){
		msg.Format("您未打开配置文件。%s",GConfigFile);
		MessageBox(msg);
	}

	GWizardXMLFileName=GConfigFile;


	CDTXML xml;
	if(!xml.Open(GWizardXMLFileName)){
		return;
	}

	if(!xml.Enter(STR_CONTENT)){
		return;
	}

	GWizardConfigID=xml.GetChildCount(STR_CONFIG);

	CDlgWizardPerfomance * PDlgWizardPerformance=NULL;
	PDlgWizardPerformance=new CDlgWizardPerfomance();
	if(PDlgWizardPerformance==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardPerformance->DoModal();
	delete PDlgWizardPerformance;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}
	



	CDlgWizardCOM * PDlgWizardCOM=NULL;
	PDlgWizardCOM=new CDlgWizardCOM();
	if(PDlgWizardCOM==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardCOM->DoModal();
	delete PDlgWizardCOM;
	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}




	CDlgWizardFrame * PDlgWizardFrame=NULL;
	PDlgWizardFrame=new CDlgWizardFrame();
	if(PDlgWizardFrame==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardFrame->DoModal();
	delete PDlgWizardFrame;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}


	
	CDlgWizardCheck * PDlgWizardCheck=NULL;
	PDlgWizardCheck=new CDlgWizardCheck();
	if(PDlgWizardCheck==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardCheck->DoModal();
	delete PDlgWizardCheck;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}


	
	CDlgWizardTimer * PDlgWizardTimer=NULL;
	PDlgWizardTimer=new CDlgWizardTimer();
	if(PDlgWizardTimer==NULL){
		return;
	}
	GWizardDlgResult=ID_WIZARD_CANCEL;	
	PDlgWizardTimer->DoModal();
	delete PDlgWizardTimer;

	if(GWizardDlgResult==ID_WIZARD_CANCEL){
		return;
	}

	
}

BOOL ConvertOldConfigFile(CString file)
{
	CHAR temppath[MAX_PATH];
	const char * tempfile="VirtualCarTemp.xml";
	CDTXML xmlsrc;
	CDTXML xmltemp;
	INT i,j;
	HANDLE htempfile;
	DWORD written;
	char tag[64];
	CString tempstr;
	char buf4096[4096];
	unsigned long outlen;


	GetTempPath(MAX_PATH,temppath);
	strcat(temppath,tempfile);

	if(!xmlsrc.Open(file)){
		return 0;
	}

	if(!xmlsrc.Enter(STR_CONTENT)){
		return 0;
	}

	if(!xmlsrc.Enter("CONFIG0")){
		return 0;
	}

	//建立临时文件
	htempfile=INVALID_HANDLE_VALUE;
	htempfile=CreateFile(temppath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,NULL,NULL);
	if(htempfile==INVALID_HANDLE_VALUE){
		return 0;
	}
	WriteFile(htempfile,STR_XML_FILE_INFO,strlen(STR_XML_FILE_INFO),&written,NULL);
	CloseHandle(htempfile);

	if(!xmltemp.Open(temppath)){
		return 0;
	}

	
	if(!xmltemp.AddChild(STR_CONTENT)){
		return 0;
	}
	if(!xmltemp.Enter(STR_CONTENT)){
		return 0;
	}


	xmlsrc.Back(2);
	if(!xmlsrc.Enter(STR_CONTENT)){
		return 0;
	}



	//复制<CONFIG>
	for(i=0; i<8; i++){
		sprintf(tag,"CONFIG%d",i);
		if(xmlsrc.GetChildCount(tag)){
			tempstr=xmlsrc.GetString(tag,"");
			xmltemp.SetString(STR_CONFIG,tempstr.GetBuffer(4096),i);

			//列正REPAYBYMANUAL to REPLYBYMANUAL
			xmltemp.Enter(STR_CONFIG,i);
			xmltemp.SetInteger(STR_REPLYBYMANUAL,xmltemp.GetInteger("REPAYBYMANUAL",0));
			xmltemp.RemoveChild("REPAYBYMANUAL");
			xmltemp.Back();
			
		}else{
			break;
		}
	}

	
	if(!xmlsrc.Enter(STR_COMM)){
		return 0;
	}
	if(!xmltemp.AddChild(STR_COMMS)){
		return 0;
	}
	if(!xmltemp.Enter(STR_COMMS)){
		return 0;
	}
	for(i=0; i<1024; i++){
		sprintf(tag,"COMM%d",i);
		if(xmlsrc.Enter(tag)){
			xmltemp.AddChild(STR_COMM,i);
			xmltemp.Enter(STR_COMM,i);


			if(xmlsrc.GetString(STR_SHOW,4096,buf4096,outlen)){
				xmltemp.SetString(STR_SHOW,buf4096);
			}

			if(xmlsrc.GetString(STR_ENABLE,4096,buf4096,outlen)){
				xmltemp.SetString(STR_ENABLE,buf4096);
			}

			if(xmlsrc.GetString("R",4096,buf4096,outlen)){
				xmltemp.SetString(STR_RECEIVE,buf4096);
			}

			if(xmlsrc.GetString("RD",4096,buf4096,outlen)){
				xmltemp.SetString(STR_REMARK,buf4096);
			}

			if(xmlsrc.GetString(STR_TYPE,4096,buf4096,outlen)){
				xmltemp.SetString(STR_TYPE,buf4096);
			}

			if(xmlsrc.GetString(STR_COUNT,4096,buf4096,outlen)){
				xmltemp.SetString(STR_COUNT,buf4096);
			}

			if(xmlsrc.GetString(STR_POS,4096,buf4096,outlen)){
				xmltemp.SetString(STR_POS,buf4096);
			}

			if(xmlsrc.GetString("PREACT",4096,buf4096,outlen)){
				xmltemp.SetString(STR_ACT,buf4096);
			}

			
			for(j=0; j<MAX_Y; j++){
				sprintf(tag,STR_YX,j);
				if(xmlsrc.GetString(tag,4096,buf4096,outlen)){
					xmltemp.SetString(tag,buf4096);
				}
			}

			for(j=0; j<64; j++){
				sprintf(tag,"S%d",j);
				if(xmlsrc.GetChildCount(tag)){
					xmltemp.AddChild(STR_SEND,j);
					xmltemp.Enter(STR_SEND,j);
					
					if(xmlsrc.GetString(tag,4096,buf4096,outlen)){
						xmltemp.SetString(STR_DATA,buf4096);
					}

					sprintf(tag,"SD%d",j);
					if(xmlsrc.GetString(tag,4096,buf4096,outlen)){
						xmltemp.SetString(STR_REMARK,buf4096);
					}

					sprintf(tag,"ACT%d",j);
					if(xmlsrc.GetString(tag,4096,buf4096,outlen)){
						xmltemp.SetString(STR_ACT,buf4096);
					}

					sprintf(tag,"SS%d",j);
					if(xmlsrc.GetString(tag,4096,buf4096,outlen)){
						xmltemp.SetString(STR_SYMBOL,buf4096);
					}
					
					xmltemp.Back();
				}else{
					break;
				}
			}

			xmlsrc.Back();
			xmltemp.Back();
		}else{
			break;
		}
	}

	xmltemp.Save();

	xmlsrc.Close();
	xmltemp.Close();

	if(!CopyFile(temppath,file,FALSE)){
		return 0;
	}
	
	return 1;
}

//将KIA模拟配置的一帧数据处理成VirtualCar格式
CString ConvertLine2Xml(CString str)
{
	int len;
	int slen;
	int i;
	CString lenstr;

	//删除所有tab;
	while(str.Find("\t",0) >= 0){
		str.Replace("\t"," ");
	}
	//将连续空格替换为单个;
	while(str.Find("  ",0) >= 0){
		str.Replace("  "," ");
	}
	str.Replace("R:","");
	str.Replace("S:","");
	str.Replace("r:","");
	str.Replace("s:","");

	slen=str.GetLength()/3-1;

	lenstr=str.Mid(0,3);		//取长度字节
//	lenstr.Replace(" ","");
	len=_tcstoul(lenstr,0,16);	//求长度（按16进制）
	str.Delete(0,3);			//删除前三个字节长度
	str.Insert(0,"00 ");		//前三个字节替换为00 （VCX数据帧）
	if(len>slen){				//按长度补足数据
		for(i=0; i<len-slen; i++){
			str+=CString(" 00");
		}
	}
	//将连续空格替换为单个;
	while(str.Find("  ",0) >= 0){
		str.Replace("  "," ");
	}
	slen=str.GetLength();
	str.Delete((len+1)*3,slen-(len+1)*3);	//
	
	return str;
}

/*******************************************************************************
* FUNCNAME  :   ConvertKiaConfigFile
* DESC      :   转换TXT模拟文件到XML模拟文件.
* INPUT     :   file1,	待转换的原始TXT文件
				file2,	xml文件模板(使用其中的参数和VCX配置)
* RETURN    :   BOOL,	1, TRUE; 0, FLASE;
*******************************************************************************/
BOOL ConvertKiaConfigFile(CString file1, CString file2)
{
	CDTXML xmlsrc;
	CDTXML xmltemp;
	INT i,j;
	HANDLE hfilexml;
	HANDLE hfiletxt;
	DWORD written;
	char tag[64];
	CString tempstr;
	char buf16k[64*1024];	//最大支持转换64K的文本文件
	INT cnt;
	CString path1;

	//处理文件路径，在*.TXT源路径生成对应的*.XML
	path1=file1;
	int nPos=path1.ReverseFind('.');
	path1=path1.Left(nPos);
	path1=path1+CString(".xml");

	//建立待生成的目标XML文件
	hfilexml=INVALID_HANDLE_VALUE;
	hfilexml=CreateFile(path1, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,NULL,NULL);
	if(hfilexml==INVALID_HANDLE_VALUE){
		return 0;
	}
	WriteFile(hfilexml,STR_XML_FILE_INFO,strlen(STR_XML_FILE_INFO),&written,NULL);
	CloseHandle(hfilexml);
	
	//打开XML模板文件
	if(!xmlsrc.Open(file2)){
		return 0;
	}
	if(!xmlsrc.Enter(STR_CONTENT)){
		return 0;
	}

	//打开待生成的目标XML文件
	if(!xmltemp.Open(path1)){
		return 0;
	}
	if(!xmltemp.AddChild(STR_CONTENT)){
		return 0;
	}
	if(!xmltemp.Enter(STR_CONTENT)){
		return 0;
	}

	//复制n个<CONFIG>
	sprintf(tag,"CONFIG");
	cnt=xmlsrc.GetChildCount(tag);
	for(i=0; i<cnt; i++){
		tempstr=xmlsrc.GetString(tag,"",i);
		xmltemp.SetString(STR_CONFIG,tempstr.GetBuffer(4096),i);
	}
	//复制所有COMM
	sprintf(tag,"COMMS");
	tempstr=xmlsrc.GetString(tag,"",0);
	xmltemp.SetString("COMMS",tempstr.GetBuffer(4096),0);
	if(!xmltemp.Enter(STR_COMMS)){
		return 0;
	}

	//打开待转换的txt文件
	hfiletxt=INVALID_HANDLE_VALUE;
	hfiletxt=CreateFile(file1, GENERIC_READ, FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	if(hfiletxt==INVALID_HANDLE_VALUE){
		return 0;
	}
	memset(buf16k,0,sizeof(buf16k));	//清空缓冲区再读取txt文件内容
	cnt=GetFileSize(hfiletxt, NULL);
	ReadFile(hfiletxt,buf16k,cnt,&written,NULL);
	CloseHandle(hfiletxt);

	//转换老的KIA平台模拟配置文件为VirtualCar格式的xml配置文件
	//转换要求：
	// 1,实现将元征模拟配置文件转换为AUTOSCAN VirtualCar模拟配置文件
	// 2,给定一个目录,批量转换
	// 3,选定一个文件,单独转换
	// 3,每条命令第LenPos个字节是长度字,一般都是第0个字节,后面数据长度不足时,需要补数.
	// 4, 注释以";;"或";;;;"开始,注释掉的命令不要删除,也要转换
	// 5,大部分模拟数据采用和校验方式,最后一个字节为校验字.
		
	int len,b,e,l=0;
	int commid=1,comm_id=0,sendid=0,send_id=0;
	CString strtemp;
	CString strline;
	BOOL	R0S1=0,r0s1=0,flag=0;

	strtemp=buf16k;

	strtemp.Replace("Req","R");
	strtemp.Replace("Ans","S");


	//将连续;;替换为单个;
	while(strtemp.Find(";;",0) >= 0){
		strtemp.Replace(";;",";");
	}
	
	//删除所有tab;
	while(strtemp.Find("\t",0) >= 0){
		strtemp.Replace("\t"," ");
	}
	strtemp.Replace(";S","s");
	strtemp.Replace(";R","r");
	strtemp.Replace(";\n","");
	
	//统一格式化成\n
	strtemp.Replace("\n\r","\n");
	strtemp.Replace("\r\n","\n");
	strtemp.Replace("\r","\n");
	//删除所有连续换行
	while(strtemp.Find("\n\n",0) >= 0){
		strtemp.Replace("\n\n","\n");
	}
	strtemp+="\n\n";	//最后加一个\n结束

	//去除配置参数
	b=strtemp.Find("R:Row");
	if(b>=0){
		e=strtemp.Find(";\n",b);
		if(e>=0){
			if(e>b){
				strtemp.Delete(b,e-b);
			}
		}
	}
	b=strtemp.Find("S:Row");
	if(b>=0){
		e=strtemp.Find(";\n",b);
		if(e>=0){
			if(e>b){
				strtemp.Delete(b,e-b);
			}
		}
	}

	b=0; e=0;
	len=strtemp.GetLength();
	for(i=0; i<len; i++){

		//找出一行
		if(strtemp.GetAt(i)=='\n'){
			b=e+1;
			e=i;

			if(e-b>=2){
				strline=strtemp.Mid(b,e-b);
				if(strline.Find("R:")>=0){
					xmltemp.AddChild(STR_COMM,commid);
					xmltemp.Enter(STR_COMM,commid);
					xmltemp.SetString(STR_RECEIVE,ConvertLine2Xml(strline));
					commid++;
					flag=1;
					sendid=0;
					send_id=0;
					xmltemp.Back();
				}
				else if(strline.Find("S:")>=0){
					xmltemp.Enter(STR_COMM,commid-1);
					xmltemp.AddChild(STR_SEND,0);
					xmltemp.Enter(STR_SEND,0);
					xmltemp.SetString(STR_DATA,ConvertLine2Xml(strline));
					xmltemp.Back(2);
				}
				else if(strline.Find("r:")>=0){
					xmltemp.AddChild("COMM_",comm_id);
					xmltemp.Enter("COMM_",comm_id);
					xmltemp.SetString(STR_RECEIVE,ConvertLine2Xml(strline));
					comm_id++;  
					flag=0;
					sendid=0;
					send_id=0;
					xmltemp.Back();
				}
				else if(strline.Find("s:")>=0){
					if(flag){
						xmltemp.Enter("COMM",commid-1);
						xmltemp.AddChild("SEND_",send_id);
						xmltemp.Enter("SEND_",send_id);
						xmltemp.SetString(STR_DATA,ConvertLine2Xml(strline));
						send_id++;
						xmltemp.Back(2);
					}else{
						xmltemp.Enter("COMM_",comm_id-1);
						xmltemp.AddChild("SEND",sendid);
						xmltemp.Enter("SEND",sendid);
						xmltemp.SetString(STR_DATA,ConvertLine2Xml(strline));
						sendid++;
						xmltemp.Back(2);
					}
				}
			}
		}
	}
	
	xmlsrc.Close();
	xmltemp.Save();
	xmltemp.Close();

	return 1;
}

void CVirtualCarWnd::OnMenuitemConvertVirtualcar1() 
{
	CString str;
	CFileDialog dlg(1,NULL,NULL,NULL,NULL,NULL);
	CString strPath;
	
	::GetModuleFileName(NULL,strPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos=strPath.ReverseFind('\\');
	strPath=strPath.Left(nPos);
	//CString strFileName = strPath.Right(strPath.GetLenth() - nPos -1);


	dlg.m_ofn.lpstrInitialDir=strPath;
	dlg.m_ofn.lpstrFilter="虚拟汽车配置文件(*.xml)\0*.xml\0";
	dlg.DoModal();
	
	if(strlen(dlg.m_ofn.lpstrFile)<2){
		return;
	}

	if(ConvertOldConfigFile(dlg.m_ofn.lpstrFile)){
		MessageBox("转换成功");
	}else{
		MessageBox("转换失败");
	}
}

void CVirtualCarWnd::OnMenuitemConvertKia() 
{
	CString str;
	CFileDialog dlg1(1,NULL,NULL,NULL,NULL,NULL);
	CFileDialog dlg2(1,NULL,NULL,NULL,NULL,NULL);
	CString strPath;
	
	::GetModuleFileName(NULL,strPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos=strPath.ReverseFind('\\');
	strPath=strPath.Left(nPos);
	//CString strFileName = strPath.Right(strPath.GetLenth() - nPos -1);
	
	dlg1.m_ofn.lpstrTitle="选择要转换的KIA平台模拟文件";
	dlg1.m_ofn.lpstrInitialDir=strPath;
	dlg1.m_ofn.lpstrFilter="KIA平台模拟配置文件(*.txt)\0*.txt\0";
	dlg1.DoModal();
	
	if(strlen(dlg1.m_ofn.lpstrFile)<2){
		return;
	}
	
	dlg2.m_ofn.lpstrTitle="选择新的模拟配置文件模板";
	dlg2.m_ofn.lpstrInitialDir=strPath;
	dlg2.m_ofn.lpstrFilter="VirtualCar模拟配置文件(*.xml)\0*.xml\0";
	dlg2.DoModal();
	
	if(strlen(dlg2.m_ofn.lpstrFile)<2){
		return;
	}
	
	if(ConvertKiaConfigFile(dlg1.m_ofn.lpstrFile,dlg2.m_ofn.lpstrFile)){
		MessageBox("转换成功");
	}else{
		MessageBox("转换失败");
	}
}

void CVirtualCarWnd::OnKillfocusEditCheckoutBegin() 
{
	UpdateData(TRUE);	
}

void CVirtualCarWnd::OnKillfocusEditCheckoutEnd() 
{
	UpdateData(TRUE);		
}

void CVirtualCarWnd::OnKillfocusEditCheckoutTarget() 
{
	UpdateData(TRUE);		
}

void CVirtualCarWnd::OnKillfocusEditReplaceCheckSymbol() 
{
	UpdateData(TRUE);		
	
}

void CVirtualCarWnd::OnKillfocusEditAutoReplyId() 
{
	CString str;
	GetDlgItem(IDC_EDIT_AUTO_REPLY_ID)->GetWindowText(str);
	GAutoReplyID=atoi(str.GetBuffer(32));
	
}

void CVirtualCarWnd::OnKillfocusEditReplacePos1() 
{
	UpdateData(TRUE);

	char * endpos;
	m_ReplacePosNum1=(unsigned char)strtol(m_ReplacePos1.GetBuffer(32),&endpos,16);
	
}

void CVirtualCarWnd::OnKillfocusEditReplaceTo1() 
{
	UpdateData(TRUE);

	char * endpos;
	m_ReplaceToNum1=(unsigned char)strtol(m_ReplaceTo1.GetBuffer(32),&endpos,16);
	
}

void CVirtualCarWnd::OnKillfocusEditReplaceTo2() 
{
	UpdateData(TRUE);

	char * endpos;
	m_ReplaceToNum2=(unsigned char)strtol(m_ReplaceTo2.GetBuffer(32),&endpos,16);
	
}

void CVirtualCarWnd::OnKillfocusEditReplacePos2() 
{
	UpdateData(TRUE);

	char * endpos;
	m_ReplacePosNum2=(unsigned char)strtol(m_ReplacePos2.GetBuffer(32),&endpos,16);
	
}

VOID RemoveA5A5ForFileFmt(CString & hexstr)
{
	CString str;
	hexstr.Remove(' ');
	hexstr.Replace("0x","");
	hexstr.Remove('h');
	hexstr.Remove('H');
	hexstr.Remove(',');
	hexstr.Replace("*","* ");
	hexstr.MakeUpper();
	//UCHAR check;
	//unsigned char buf[1024];
	//UINT buflen;
	int i;
	int len;

	if(hexstr.Find("A5A5")==0){
		hexstr=hexstr.Mid(10,hexstr.GetLength()-12);
	}

	//加入空格
	len=hexstr.GetLength();
	for(i=len-2; i>=2; i-=2){
		hexstr.Insert(i," ");
	}

}

VOID AddA5A5ForFileFmt(CString & hexstr)
{
	CString str;
	hexstr.Remove(' ');
	hexstr.Replace("0x","");
	hexstr.Remove('h');
	hexstr.Remove('H');
	hexstr.Remove(',');
	hexstr.Replace("*","* ");
	hexstr.MakeUpper();
	UCHAR check;
	unsigned char buf[1024];
	UINT buflen;
	UINT i;
	int len;

	if(hexstr.Find("A5A5")!=0){
		if(hexstr.GetLength()%2==1){
			hexstr+='0';
		}
		//加33
		hexstr.Insert(0,"33");

		len=hexstr.GetLength()/2;


		str.Format("00%02x",len);
		hexstr.Insert(0,str);


		//校验和取反
		HexString2Buffer(hexstr,buf,1024,&buflen);
		check=0;
		for(i=0; i<buflen; i++){
			check+=buf[i];
		}
		check=~check;


		//加头与长度
		hexstr.Insert(0,"A5A5");

		//国校验和取反
		str.Format("%02x",check);
		hexstr+=str;	//校验和

	}

	//加入空格
	len=hexstr.GetLength();
	for(i=len-2; i>=2; i-=2){
		hexstr.Insert(i," ");
	}


}

void CVirtualCarWnd::OnMenuitemAdda5a5() 
{
	CString str;
	CFileDialog dlg(1,NULL,NULL,NULL,NULL,NULL);
	CString strPath;
	
	::GetModuleFileName(NULL,strPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos=strPath.ReverseFind('\\');
	strPath=strPath.Left(nPos);
	//CString strFileName = strPath.Right(strPath.GetLenth() - nPos -1);


	dlg.m_ofn.lpstrInitialDir=strPath;
	dlg.m_ofn.lpstrFilter="虚拟汽车配置文件(*.xml)\0*.xml\0";
	dlg.DoModal();
	
	if(strlen(dlg.m_ofn.lpstrFile)<2){
		return;
	}


	CDTXML xml;
	CString tag;
	int i,j;

	if(!xml.Open(dlg.m_ofn.lpstrFile)){
		return;
	}

	if(!xml.Enter(STR_CONTENT)){
		return;
	}

	if(!xml.Enter(STR_COMMS)){
		return;
	}

	for(i=0; i<255; i++){
		if(xml.Enter(STR_COMM,i)){

			str=xml.GetString(STR_RECEIVE,"");
			AddA5A5ForFileFmt(str);
			xml.SetString(STR_RECEIVE,str);


			
			for(j=0; j<64; j++){
				if(xml.Enter(STR_SEND,j)){
					
					str=xml.GetString(STR_DATA,"");
					AddA5A5ForFileFmt(str);
					xml.SetString(STR_DATA,str);

					xml.Back();
				}else{
					break;
				}
			}

			xml.Back();
		}else{
			break;
		}
	}

	xml.Save();

	MessageBox("添加A5A5成功");

}


void CVirtualCarWnd::OnMenuitemRemovea5a5() 
{
	CString str;
	CFileDialog dlg(1,NULL,NULL,NULL,NULL,NULL);
	CString strPath;
	
	::GetModuleFileName(NULL,strPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	strPath.ReleaseBuffer();
	int nPos=strPath.ReverseFind('\\');
	strPath=strPath.Left(nPos);
	//CString strFileName = strPath.Right(strPath.GetLenth() - nPos -1);


	dlg.m_ofn.lpstrInitialDir=strPath;
	dlg.m_ofn.lpstrFilter="虚拟汽车配置文件(*.xml)\0*.xml\0";
	dlg.DoModal();
	
	if(strlen(dlg.m_ofn.lpstrFile)<2){
		return;
	}


	CDTXML xml;
	CString tag;
	int i,j;

	if(!xml.Open(dlg.m_ofn.lpstrFile)){
		return;
	}

	if(!xml.Enter(STR_CONTENT)){
		return;
	}

	if(!xml.Enter(STR_COMMS)){
		return;
	}

	for(i=0; i<255; i++){
		if(xml.Enter(STR_COMM,i)){

			str=xml.GetString(STR_RECEIVE,"");
			RemoveA5A5ForFileFmt(str);
			xml.SetString(STR_RECEIVE,str);


			
			for(j=0; j<64; j++){
				if(xml.Enter(STR_SEND,j)){
					
					str=xml.GetString(STR_DATA,"");
					RemoveA5A5ForFileFmt(str);
					xml.SetString(STR_DATA,str);

					xml.Back();
				}else{
					break;
				}
			}

			xml.Back();
		}else{
			break;
		}
	}

	xml.Save();

	MessageBox("去除A5A5成功");
}

void CVirtualCarWnd::OnEditchangeComboFrameType() 
{
	// TODO: Add your control notification handler code here
}

void CVirtualCarWnd::OnSelchangeComboFrameType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	GFrameType=m_FrameType;
	
}

INT KT600Pack(PBYTE frame, INT length, PBYTE pbuffer, INT buffersize)
{
	INT count=0;

	if(buffersize<length+5){
		return 0;
	}

	pbuffer[count]=0xa5;
	count++;
	pbuffer[count]=0xa5;
	count++;

	pbuffer[count]=((unsigned short)length)&0xff00;
	count++;
	
	pbuffer[count]=((unsigned short)length)&0x00ff;
	count++;

	memcpy(pbuffer+count,(VOID*)frame,length);
	count+=length;

	pbuffer[count]=~((BYTE)CalcSum(frame,length));
	count++;	

	return count;
}

INT VCXPack(PBYTE frame, INT length, PBYTE pbuffer, INT buffersize)
{
	INT count=0;
	INT i;
	
	pbuffer[count]=FRAME_HEAD;
	count++;
	
	for(i=0; i<length; i++){
		
		if(buffersize<count+5){
			return 0;
		}
		
		switch(frame[i]){
		case FRAME_HEAD:
			pbuffer[count]=FRAME_TRANS;
			count++;
			pbuffer[count]=FRAME_TRANS_HEAD;
			count++;
			break;
		case FRAME_TAIL:
			pbuffer[count]=FRAME_TRANS;
			count++;
			pbuffer[count]=FRAME_TRANS_TAIL;
			count++;
			break;
		case FRAME_TRANS:
			pbuffer[count]=FRAME_TRANS;
			count++;
			pbuffer[count]=FRAME_TRANS_TRANS;
			count++;
			break;
		default:
			pbuffer[count]=frame[i];
			count++;
			break;
		}	
	}
	
	if(buffersize<count+3){
		return 0;
	}
	
	//for checksum
	BYTE checksum=(BYTE)CalcSum(frame,length);

	switch(checksum){
	case FRAME_HEAD:
		pbuffer[count]=FRAME_TRANS;
		count++;
		pbuffer[count]=FRAME_TRANS_HEAD;
		count++;
		break;
	case FRAME_TAIL:
		pbuffer[count]=FRAME_TRANS;
		count++;
		pbuffer[count]=FRAME_TRANS_TAIL;
		count++;
		break;
	case FRAME_TRANS:
		pbuffer[count]=FRAME_TRANS;
		count++;
		pbuffer[count]=FRAME_TRANS_TRANS;
		count++;
		break;
	default:
		pbuffer[count]=checksum;
		count++;
		break;
		}		
	
	
	pbuffer[count]=FRAME_TAIL;
	count++;
	
	return count;
}

/*
*/
INT VCXPlusPack(PBYTE frame, INT length, PBYTE pbuffer, INT buffersize)
{
	INT i=0 ;
	INT count=0;

	pbuffer[count]=VCX_FRAME_MARK;
	count++;
	for(i=0;i<length;i++){
		if(buffersize<count+5){
			return 0;
		}
		switch(frame[i])
		{
		case VCX_FRAME_MARK: // 0xbb
		case VCX_FRAME_TRANS2: // 0xdd
			pbuffer[count] = VCX_FRAME_TRANS2;
			count++ ;
			pbuffer[count] = frame[i] ^ VCX_FRAME_XOR ;
			count++ ;
			break ;
		default:
			pbuffer[count] = frame[i] ;
			count++;
			break ;
		}
	}

	if(buffersize<count+3){
		return 0;
	}
	BYTE checkSum=(BYTE)CalcSum(pbuffer,count);
	switch(checkSum){
		case VCX_FRAME_MARK: // 0xbb
		case VCX_FRAME_TRANS2: // 0xdd
			pbuffer[count] = VCX_FRAME_TRANS2;
			count++ ;
			pbuffer[count] = checkSum ^ VCX_FRAME_XOR ;
			count++ ;
			break ;
		default:
			pbuffer[count] = checkSum ;
			count++;
			break ;
	}
	pbuffer[count]=VCX_FRAME_MARK;
	count++;
	return count ;
}

//复制到剪切板
VOID CopyTextToClipboard(HWND owner, LPCSTR str)
{
	if(OpenClipboard(owner)){
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(str)+1);
		buffer = (char*)GlobalLock(clipbuffer);
		
		strcpy(buffer, LPCSTR(str));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
}

void CVirtualCarWnd::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW * pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );

		//复制到剪切板
		CString source;
		source=m_list.GetItemText(pNMListView->iItem,pNMListView->iSubItem);
		CopyTextToClipboard(m_hWnd,source.GetBuffer(0));

	} 
	
	*pResult = 0;
}

void CVirtualCarWnd::OnMenuitemPause() 
{
	GPause=!GPause;
	if(GPause){
		CheckMenuItem(Menu,ID_MENUITEM_PAUSE,MF_CHECKED);
		CheckDlgButton(IDC_CHECK_PAUSE,1);
	}else{
		CheckMenuItem(Menu,ID_MENUITEM_PAUSE,MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_PAUSE,0);
	}
}

void CVirtualCarWnd::OnCheckPause() 
{
	GPause=!GPause;
	if(GPause){
		CheckMenuItem(Menu,ID_MENUITEM_PAUSE,MF_CHECKED);
		CheckDlgButton(IDC_CHECK_PAUSE,1);
	}else{
		CheckMenuItem(Menu,ID_MENUITEM_PAUSE,MF_UNCHECKED);
		CheckDlgButton(IDC_CHECK_PAUSE,0);
		GComm.PurgeComm(PURGE_RXCLEAR);
	}
	UpdateData(FALSE);
}

void CVirtualCarWnd::OnButtonClearSend() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_INPUT_TEMP)->SetWindowText("");	
}

void CVirtualCarWnd::OnMenuitemClearInbuffer() 
{
	// TODO: Add your command handler code here
	if(GComm.IsOpen()){
		GComm.PurgeComm(PURGE_RXCLEAR);
		AddMsg(ID_IMG_NOTE,"输入缓冲区清除完毕。",NULL,NULL,NULL);
	}	
}

void CVirtualCarWnd::OnMenuitemClearOutbuffer() 
{
	// TODO: Add your command handler code here
	if(GComm.IsOpen()){
		GComm.PurgeComm(PURGE_TXCLEAR);
		AddMsg(ID_IMG_NOTE,"输出缓冲区清除完毕。",NULL,NULL,NULL);
	}
}

void CVirtualCarWnd::OnStaticInput() 
{
	// TODO: Add your control notification handler code here
	OnMenuitemClearInbuffer();
}

void CVirtualCarWnd::OnStaticOutput() 
{
	// TODO: Add your control notification handler code here
	OnMenuitemClearOutbuffer();	
}

void CVirtualCarWnd::OnMenuitemCopyall() 
{
	// TODO: Add your command handler code here
	int i;
	int cnt;

	const int textmax=409600;
	char * text=0;
	int pos=0;

	text=new char[textmax];
	if(text==0){
		return;
	}

	cnt=m_list.GetItemCount();

	for(i=0; i<cnt; i++){
		pos+=m_list.GetItemText(i,0,text+pos,textmax-pos);
		if(pos>=textmax-3){
			break;
		}

		strcpy(text+pos,"\r\n");
		pos+=2;
	}

	CopyTextToClipboard(m_hWnd, text);

	delete [] text;
}

void CVirtualCarWnd::OnButtonSendComm() 
{
	// TODO: Add your control notification handler code here
	CDlgSend dlg;

	dlg.DoModal();

	if(dlg.str_result.GetLength()){
		dlg.str_result.Replace("*","??");
		SendTempData(dlg.str_result);
	}
}
