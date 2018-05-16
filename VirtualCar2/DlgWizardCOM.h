#if !defined(AFX_DLGWIZARDCOM_H__5C19A446_A336_4756_94CD_3FE29020B170__INCLUDED_)
#define AFX_DLGWIZARDCOM_H__5C19A446_A336_4756_94CD_3FE29020B170__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWizardCOM.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardCOM dialog

class CDlgWizardCOM : public CDialog
{
// Construction
public:
	CDlgWizardCOM(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWizardCOM)
	enum { IDD = IDD_DIALOG_WIZARD_COM };
	CString	m_Port;
	CString	m_SpeedStr;
	CString	m_Bits;
	int		m_Parity;
	int		m_StopBits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWizardCOM)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWizardCOM)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonIgnore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWIZARDCOM_H__5C19A446_A336_4756_94CD_3FE29020B170__INCLUDED_)
