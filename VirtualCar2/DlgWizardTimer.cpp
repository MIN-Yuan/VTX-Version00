// DlgWizardTimer.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "DlgWizardTimer.h"
#include "VirtualCarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardTimer dialog


CDlgWizardTimer::CDlgWizardTimer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizardTimer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWizardTimer)
	m_ReplyByManual = FALSE;
	m_AutoReplyEnable = -1;
	m_ID = 0;
	m_ManualData = _T("");
	m_Space = 0;
	//}}AFX_DATA_INIT
}


void CDlgWizardTimer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWizardTimer)
	DDX_Check(pDX, IDC_CHECK_REPLYBYMANUAL, m_ReplyByManual);
	DDX_CBIndex(pDX, IDC_COMBO_AUTOREPLYENABLE, m_AutoReplyEnable);
	DDX_Text(pDX, IDC_EDIT_ID, m_ID);
	DDX_Text(pDX, IDC_EDIT_MANUALDATA, m_ManualData);
	DDX_Text(pDX, IDC_EDIT_SPACE, m_Space);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWizardTimer, CDialog)
	//{{AFX_MSG_MAP(CDlgWizardTimer)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_IGNORE, OnButtonIgnore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardTimer message handlers

BOOL CDlgWizardTimer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_AutoReplyEnable=0;
	m_ID=0;
	m_Space=3600000;
	m_ManualData="";
	m_AutoReplyEnable=0;	

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWizardTimer::OnButtonNext() 
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

	xml.SetInteger(STR_AUTOREPLYENABLE,m_AutoReplyEnable);
	xml.SetInteger(STR_AUTOREPLYID,m_ID);
	xml.SetInteger(STR_AUTOREPLYSPACE,m_Space);
	xml.SetInteger(STR_REPLYBYMANUAL,m_ReplyByManual);
	
	TrimToPureHexString(m_ManualData);
	xml.SetString(STR_MANUALDATA,m_ManualData);

	xml.Save();
	xml.Close();

	GWizardDlgResult=ID_WIZARD_NEXT;

	OnOK();		
}

void CDlgWizardTimer::OnButtonIgnore() 
{
	GWizardDlgResult=ID_WIZARD_IGNORE;
	OnOK();
	
}

BOOL CDlgWizardTimer::PreTranslateMessage(MSG* pMsg) 
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
