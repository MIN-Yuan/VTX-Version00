#if !defined(AFX_DLGWIZARDTITLE_H__257AD325_481A_4507_B8D4_CBEBCEA7FF53__INCLUDED_)
#define AFX_DLGWIZARDTITLE_H__257AD325_481A_4507_B8D4_CBEBCEA7FF53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWizardTitle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardTitle dialog

class CDlgWizardTitle : public CDialog
{
// Construction
public:
	CDlgWizardTitle(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWizardTitle)
	enum { IDD = IDD_DIALOG_WIZARD_TITLE };
	CString	m_LogFile;
	CString	m_Title;
	BOOL	m_LogFileEnable;
	BOOL	m_FIlterSelfSent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWizardTitle)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWizardTitle)
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonNext();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditTitle();
	afx_msg void OnChangeEditLogfile();
	afx_msg void OnCheckLogenable();
	afx_msg void OnCheckFilterSelfSent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWIZARDTITLE_H__257AD325_481A_4507_B8D4_CBEBCEA7FF53__INCLUDED_)
