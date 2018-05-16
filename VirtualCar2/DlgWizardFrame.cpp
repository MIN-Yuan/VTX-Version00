// DlgWizardFrame.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "DlgWizardFrame.h"
#include "VirtualCarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardFrame dialog


CDlgWizardFrame::CDlgWizardFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizardFrame::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWizardFrame)
	m_DelayRealtime = FALSE;
	m_SendDelay = 0;
	m_ReceiveDelay = 0;
	m_MiniFrameSpace = 0;
	m_FrameType = -1;
	//}}AFX_DATA_INIT
}


void CDlgWizardFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWizardFrame)
	DDX_Check(pDX, IDC_CHECK_DELAYREALTIME, m_DelayRealtime);
	DDX_Text(pDX, IDC_EDIT_SENDDELAY, m_SendDelay);
	DDX_Text(pDX, IDC_EDIT_RECEIVEDELAY, m_ReceiveDelay);
	DDX_Text(pDX, IDC_EDIT_MINIFRAMESPACE, m_MiniFrameSpace);
	DDX_CBIndex(pDX, IDC_COMBO_FRAMETYPE, m_FrameType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWizardFrame, CDialog)
	//{{AFX_MSG_MAP(CDlgWizardFrame)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_IGNORE, OnButtonIgnore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardFrame message handlers

BOOL CDlgWizardFrame::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_FrameType=0;
	m_MiniFrameSpace=10;
	m_ReceiveDelay=0;
	m_SendDelay=0;
	m_DelayRealtime=0;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWizardFrame::OnButtonNext() 
{
	CDTXML xml;

	UpdateData(TRUE);

	if(!xml.Open(GWizardXMLFileName)){
		MessageBox("打开文件失败");
	}

	if(!xml.Enter(STR_CONTENT)){
		xml.AddChild(STR_CONTENT);
		xml.Enter(STR_CONTENT);
	}

	if(!xml.Enter(STR_CONFIG,GWizardConfigID)){
		xml.AddChild(STR_CONFIG,GWizardConfigID);
		xml.Enter(STR_CONFIG,GWizardConfigID);
	}

	xml.SetInteger(STR_FRAMETYPE,m_FrameType);
	xml.SetInteger(STR_MINFRAMESPACE ,m_MiniFrameSpace);
	xml.SetInteger(STR_RECEIVEDELAY ,m_ReceiveDelay);
	xml.SetInteger(STR_SENDDELAY,m_SendDelay);
	xml.SetInteger(STR_DELAYREALTIME,m_DelayRealtime);

	xml.Save();
	xml.Close();

	GWizardDlgResult=ID_WIZARD_NEXT;

	OnOK();
}

void CDlgWizardFrame::OnButtonIgnore() 
{
	GWizardDlgResult=ID_WIZARD_IGNORE;
	OnOK();
}

BOOL CDlgWizardFrame::PreTranslateMessage(MSG* pMsg) 
{
    if(pMsg->message == WM_KEYDOWN){
        if(pMsg->wParam == VK_ESCAPE){
			return TRUE;
        }

		if(pMsg->wParam == VK_RETURN){
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
