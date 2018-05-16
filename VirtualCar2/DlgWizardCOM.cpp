// DlgWizardCOM.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "DlgWizardCOM.h"
#include "VirtualCarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardCOM dialog


CDlgWizardCOM::CDlgWizardCOM(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizardCOM::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWizardCOM)
	m_Port = _T("");
	m_SpeedStr = _T("");
	m_Bits = _T("");
	m_Parity = -1;
	m_StopBits = -1;
	//}}AFX_DATA_INIT
}


void CDlgWizardCOM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWizardCOM)
	DDX_CBString(pDX, IDC_COMBO_PORT, m_Port);
	DDX_CBString(pDX, IDC_COMBO_SPEED, m_SpeedStr);
	DDX_CBString(pDX, IDC_COMBO_BITS, m_Bits);
	DDX_CBIndex(pDX, IDC_COMBO_PARITY, m_Parity);
	DDX_CBIndex(pDX, IDC_COMBO_STOPBITS, m_StopBits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWizardCOM, CDialog)
	//{{AFX_MSG_MAP(CDlgWizardCOM)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_IGNORE, OnButtonIgnore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardCOM message handlers

BOOL CDlgWizardCOM::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Port="COM1";
	m_SpeedStr="115200";
	m_StopBits=0;	//ONESTOPBITS
	m_Bits="8";
	m_Parity=0;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWizardCOM::OnButtonNext() 
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

	xml.SetString(STR_PORT,m_Port);
	xml.SetString(STR_SPEED,m_SpeedStr);
	xml.SetString(STR_BITS,m_Bits);
	xml.SetInteger(STR_PARITY,m_Parity);
	xml.SetInteger(STR_STOPBITS,m_StopBits);

	xml.Save();
	xml.Close();

	GWizardDlgResult=ID_WIZARD_NEXT;

	OnOK();
}



void CDlgWizardCOM::OnButtonIgnore() 
{
	GWizardDlgResult=ID_WIZARD_IGNORE;
	OnOK();
}

BOOL CDlgWizardCOM::PreTranslateMessage(MSG * pMsg) 
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
