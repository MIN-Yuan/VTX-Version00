// DlgWizardCheck.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "DlgWizardCheck.h"
#include "VirtualCarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardCheck dialog


CDlgWizardCheck::CDlgWizardCheck(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizardCheck::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWizardCheck)
	m_ReplaceCheck = FALSE;
	m_CheckType = -1;
	m_Begin = 0;
	m_End = 0;
	m_Symbol = 0;
	m_Target = 0;
	//}}AFX_DATA_INIT
}


void CDlgWizardCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWizardCheck)
	DDX_Check(pDX, IDC_CHECK_REPLACECHECK, m_ReplaceCheck);
	DDX_CBIndex(pDX, IDC_COMBO_CHECKTYPE, m_CheckType);
	DDX_Text(pDX, IDC_EDIT_BEGIN, m_Begin);
	DDX_Text(pDX, IDC_EDIT_END, m_End);
	DDX_Text(pDX, IDC_EDIT_SYMBOL, m_Symbol);
	DDX_Text(pDX, IDC_EDIT_TARGET, m_Target);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWizardCheck, CDialog)
	//{{AFX_MSG_MAP(CDlgWizardCheck)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_IGNORE, OnButtonIgnore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardCheck message handlers

BOOL CDlgWizardCheck::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ReplaceCheck=0;
	m_CheckType=0;
	m_Begin=0;
	m_End=-2;
	m_Target=-1;
	m_Symbol=0;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWizardCheck::OnButtonNext() 
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

	xml.SetInteger(STR_CHECKENABLE ,m_ReplaceCheck);
	xml.SetInteger(STR_CHECKBEGIN,m_Begin);
	xml.SetInteger(STR_CHECKEND,m_End);
	xml.SetInteger(STR_CHECKTARGET,m_Target);
	xml.SetInteger(STR_CHECKSYMBOL,m_Symbol);

	xml.Save();
	xml.Close();

	GWizardDlgResult=ID_WIZARD_NEXT;

	OnOK();	
}

void CDlgWizardCheck::OnButtonIgnore() 
{
	GWizardDlgResult=ID_WIZARD_IGNORE;
	OnOK();
}

BOOL CDlgWizardCheck::PreTranslateMessage(MSG* pMsg) 
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
