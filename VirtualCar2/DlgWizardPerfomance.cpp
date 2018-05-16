// DlgWizardPerfomance.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "DlgWizardPerfomance.h"
#include "VirtualCarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardPerfomance dialog


CDlgWizardPerfomance::CDlgWizardPerfomance(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizardPerfomance::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWizardPerfomance)
	m_WatchList = TRUE;
	m_EditPriority = _T("");
	m_EditSleep = _T("");
	//}}AFX_DATA_INIT
}


void CDlgWizardPerfomance::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWizardPerfomance)
	DDX_Control(pDX, IDC_SLIDER_SLEEP, m_SliderSleep);
	DDX_Control(pDX, IDC_SLIDER_PRIORITY, m_SliderPriority);
	DDX_Check(pDX, IDC_CHECK_WATCH_LIST, m_WatchList);
	DDX_Text(pDX, IDC_EDIT_PRIORITY, m_EditPriority);
	DDX_Text(pDX, IDC_EDIT_SLEEP, m_EditSleep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWizardPerfomance, CDialog)
	//{{AFX_MSG_MAP(CDlgWizardPerfomance)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PRIORITY, OnReleasedcaptureSliderPriority)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SLEEP, OnReleasedcaptureSliderSleep)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_CHECK_WATCH_LIST, OnCheckWatchList)
	ON_BN_CLICKED(IDC_BUTTON_IGNORE, OnButtonIgnore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardPerfomance message handlers

BOOL CDlgWizardPerfomance::OnInitDialog() 
{
	CDialog::OnInitDialog();

	
	m_SliderPriority.SetRange(-3,3);
	m_SliderPriority.SetPos(-1);
	m_SliderPriority.SetPos(0);
	m_EditPriority=PrioritySliderValue2Description(0);
	
	m_SliderSleep.SetRange(1,32);
	m_SliderSleep.SetPos(1);
	m_EditSleep="1";

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWizardPerfomance::OnReleasedcaptureSliderPriority(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	m_EditPriority=PrioritySliderValue2Description(m_SliderPriority.GetPos());

	UpdateData(FALSE);
	*pResult = 0;
}

void CDlgWizardPerfomance::OnReleasedcaptureSliderSleep(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_EditSleep.Format("%d",m_SliderSleep.GetPos());
	UpdateData(FALSE);

	*pResult = 0;
}

void CDlgWizardPerfomance::OnButtonNext() 
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

	xml.SetInteger(STR_SLEEP,m_SliderSleep.GetPos());
	xml.SetInteger(STR_THREADPRIORITY,m_SliderPriority.GetPos());
	xml.SetInteger(STR_WATCHLIST,m_WatchList);

	xml.Save();
	xml.Close();

	GWizardDlgResult=ID_WIZARD_NEXT;

	OnOK();

}

void CDlgWizardPerfomance::OnCheckWatchList() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CDlgWizardPerfomance::OnButtonIgnore() 
{
	GWizardDlgResult=ID_WIZARD_IGNORE;
	OnOK();
}

BOOL CDlgWizardPerfomance::PreTranslateMessage(MSG* pMsg) 
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
