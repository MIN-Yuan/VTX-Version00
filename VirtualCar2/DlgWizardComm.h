#if !defined(AFX_DLGWIZARDCOMM_H__C7E5F6B6_61D3_4606_93DC_6BDCAAF1E209__INCLUDED_)
#define AFX_DLGWIZARDCOMM_H__C7E5F6B6_61D3_4606_93DC_6BDCAAF1E209__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWizardComm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardComm dialog

class CDlgWizardComm : public CDialog
{
// Construction
public:
	CDlgWizardComm(CWnd* pParent = NULL);   // standard constructor
	void OnOK();
// Dialog Data
	//{{AFX_DATA(CDlgWizardComm)
	enum { IDD = IDD_DIALOG_WIZARD_COMM };
	CString	m_Data;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWizardComm)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWizardComm)
	afx_msg void OnButtonNext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWIZARDCOMM_H__C7E5F6B6_61D3_4606_93DC_6BDCAAF1E209__INCLUDED_)
