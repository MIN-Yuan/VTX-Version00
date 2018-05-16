// InputByteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "InputByteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputByteDlg dialog


CInputByteDlg::CInputByteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputByteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputByteDlg)
	m_Edit = _T("");
	//}}AFX_DATA_INIT
}


void CInputByteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputByteDlg)
	DDX_Text(pDX, IDC_EDIT, m_Edit);
	DDV_MaxChars(pDX, m_Edit, 2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputByteDlg, CDialog)
	//{{AFX_MSG_MAP(CInputByteDlg)
	ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputByteDlg message handlers

void CInputByteDlg::OnChangeEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}


void CInputByteDlg::OnOK()
{
	// TODO: Add extra validation here
	CDialog::OnOK();
}
