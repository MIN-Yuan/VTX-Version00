#if !defined(AFX_DLGWIZARDNEWFILE_H__9C4C2DC5_2BC5_4E1D_9EA8_66E033261E4A__INCLUDED_)
#define AFX_DLGWIZARDNEWFILE_H__9C4C2DC5_2BC5_4E1D_9EA8_66E033261E4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWizardNewFile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardNewFile dialog

class CDlgWizardNewFile : public CDialog
{
// Construction
public:
	CDlgWizardNewFile(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWizardNewFile)
	enum { IDD = IDD_DIALOG_WIZARD_NEWFILE };
	CString	m_FilePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWizardNewFile)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWizardNewFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonNext();
	afx_msg void OnChangeEditFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWIZARDNEWFILE_H__9C4C2DC5_2BC5_4E1D_9EA8_66E033261E4A__INCLUDED_)
