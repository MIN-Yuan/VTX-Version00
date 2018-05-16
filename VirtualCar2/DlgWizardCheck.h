#if !defined(AFX_DLGWIZARDCHECK_H__4BA337D6_352C_4EA7_98FA_492B0B294CE6__INCLUDED_)
#define AFX_DLGWIZARDCHECK_H__4BA337D6_352C_4EA7_98FA_492B0B294CE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWizardCheck.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardCheck dialog

class CDlgWizardCheck : public CDialog
{
// Construction
public:
	CDlgWizardCheck(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWizardCheck)
	enum { IDD = IDD_DIALOG_WIZARD_CHECK };
	BOOL	m_ReplaceCheck;
	int		m_CheckType;
	int		m_Begin;
	int		m_End;
	UINT	m_Symbol;
	int		m_Target;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWizardCheck)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWizardCheck)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonIgnore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWIZARDCHECK_H__4BA337D6_352C_4EA7_98FA_492B0B294CE6__INCLUDED_)
