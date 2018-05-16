// DlgParas.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "DlgParas.h"
#include "VirtualCarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgParas dialog


CDlgParas::CDlgParas(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgParas::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgParas)
	//}}AFX_DATA_INIT
	pmainwin=NULL;
}


void CDlgParas::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgParas)
	DDX_Control(pDX, IDC_LIST_PARAS, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgParas, CDialog)
	//{{AFX_MSG_MAP(CDlgParas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgParas message handlers


BOOL CDlgParas::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(pmainwin==NULL){
		return 0;
	}

	CString str;

	m_List.InsertColumn(0,"NAME",LVCFMT_LEFT,140);
	m_List.InsertColumn(1,"VALUE",LVCFMT_LEFT,270);

	m_List.InsertItem(m_List.GetItemCount(),"CUR_CFG_FILE");
	m_List.SetItemText(m_List.GetItemCount()-1,1,GConfigFile);


	m_List.InsertItem(m_List.GetItemCount(),STR_TITLE);
	m_List.SetItemText(m_List.GetItemCount()-1,1,GTitle);

	m_List.InsertItem(m_List.GetItemCount(),STR_LOGFILE);
	m_List.SetItemText(m_List.GetItemCount()-1,1,GLogFile);

	m_List.InsertItem(m_List.GetItemCount(),"LOGFILE_OPEN");
	m_List.SetItemText(m_List.GetItemCount()-1,1,GLogFile?"TRUE":"FALSE");

	m_List.InsertItem(m_List.GetItemCount(),STR_PORT);
	m_List.SetItemText(m_List.GetItemCount()-1,1,GPort);

	m_List.InsertItem(m_List.GetItemCount(),STR_SPEED);
	str.Format("%d",GSpeed);
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);
	
	m_List.InsertItem(m_List.GetItemCount(),STR_PARITY);
	str.Format("%d ",GParity);
	str+=ParityValue2Description(GParity);
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);

	m_List.InsertItem(m_List.GetItemCount(),STR_BITS);
	str.Format("%d",GBits);
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);

	m_List.InsertItem(m_List.GetItemCount(),STR_STOPBITS);
	str.Format("%s停止位",StopBitsValue2Description(GStopbits));
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);

	m_List.InsertItem(m_List.GetItemCount(),STR_FRAMETYPE);
	switch(GFrameType){
	case 0:
		str="0 按最小帧间隔时间区分帧";
		break;
	case 1:
		str="1 按采样板帧格式";
		break;
	case 2:
		str="2 按VCX帧格式";
	default:
		str.Format("%d",GFrameType);
		break;
	}
	m_List.SetItemText(m_List.GetItemCount()-1,1,(CString)str);

	m_List.InsertItem(m_List.GetItemCount(),STR_MINFRAMESPACE);
	str.Format("%d ms",GMinFrameSpace);
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);

	m_List.InsertItem(m_List.GetItemCount(),STR_DELAYREALTIME);
	m_List.SetItemText(m_List.GetItemCount()-1,1,GDelayRealtime?"1 实时延时(占CPU)":"0 Sleep延时");

	m_List.InsertItem(m_List.GetItemCount(),STR_SENDDELAY);
	str.Format("%d ms",GAllSendDelay);
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);

	m_List.InsertItem(m_List.GetItemCount(),STR_RECEIVEDELAY);
	str.Format("%d ms",GAllReceiveDelay);
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);


	m_List.InsertItem(m_List.GetItemCount(),STR_THREADPRIORITY);
	str.Format("%d",pmainwin->m_SliderPriority.GetPos());
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);

	m_List.InsertItem(m_List.GetItemCount(),STR_SLEEP);
	str.Format("%d",GSleep);
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);

	m_List.InsertItem(m_List.GetItemCount(),"COMM_COUNT");
	str.Format("%d",CommCount);
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);


	//Listview设置全行选定
	m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	UpdateData(FALSE);
	
	return TRUE;
}
