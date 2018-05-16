// DlgWizardTitle.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "DlgWizardTitle.h"
#include "VirtualCarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardTitle dialog


CDlgWizardTitle::CDlgWizardTitle(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizardTitle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWizardTitle)
	m_LogFile = _T("log.txt");
	m_Title = _T("");
	m_LogFileEnable = FALSE;
	m_FIlterSelfSent = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgWizardTitle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWizardTitle)
	DDX_Text(pDX, IDC_EDIT_LOGFILE, m_LogFile);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_Title);
	DDX_Check(pDX, IDC_CHECK_LOGENABLE, m_LogFileEnable);
	DDX_Check(pDX, IDC_CHECK_FILTER_SELF_SENT, m_FIlterSelfSent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWizardTitle, CDialog)
	//{{AFX_MSG_MAP(CDlgWizardTitle)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_EN_CHANGE(IDC_EDIT_TITLE, OnChangeEditTitle)
	ON_EN_CHANGE(IDC_EDIT_LOGFILE, OnChangeEditLogfile)
	ON_BN_CLICKED(IDC_CHECK_LOGENABLE, OnCheckLogenable)
	ON_BN_CLICKED(IDC_CHECK_FILTER_SELF_SENT, OnCheckFilterSelfSent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardTitle message handlers


void CDlgWizardTitle::OnButtonNext() 
{
	CDTXML xml;


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

	xml.SetString(STR_TITLE,m_Title.GetBuffer(255));
	xml.SetString(STR_LOGFILE,m_LogFile);
	xml.SetInteger(STR_LOGENABLE,m_LogFileEnable);
	xml.SetInteger(STR_FILTERSELFSENT,m_FIlterSelfSent);

	xml.Save();
	xml.Close();

	GWizardDlgResult=ID_WIZARD_NEXT;

	OnOK();
}


BOOL CDlgWizardTitle::OnInitDialog() 
{
	CDialog::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWizardTitle::OnChangeEditTitle() 
{
	UpdateData(TRUE);	
}

void CDlgWizardTitle::OnChangeEditLogfile() 
{
	UpdateData(TRUE);		
}

void CDlgWizardTitle::OnCheckLogenable() 
{
	UpdateData(TRUE);	
	
}

void CDlgWizardTitle::OnCheckFilterSelfSent() 
{
	UpdateData(TRUE);	
	
}

BOOL CDlgWizardTitle::PreTranslateMessage(MSG* pMsg) 
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
