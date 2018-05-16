#if !defined(AFX_DLGWIZARDTIMER_H__7BC36D2F_17E6_4289_B740_25730AE3EAC7__INCLUDED_)
#define AFX_DLGWIZARDTIMER_H__7BC36D2F_17E6_4289_B740_25730AE3EAC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWizardTimer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardTimer dialog

class CDlgWizardTimer : public CDialog
{
// Construction
public:
	CDlgWizardTimer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWizardTimer)
	enum { IDD = IDD_DIALOG_WIZARD_TIMER };
	BOOL	m_ReplyByManual;
	int		m_AutoReplyEnable;
	UINT	m_ID;
	CString	m_ManualData;
	UINT	m_Space;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWizardTimer)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWizardTimer)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonIgnore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWIZARDTIMER_H__7BC36D2F_17E6_4289_B740_25730AE3EAC7__INCLUDED_)
