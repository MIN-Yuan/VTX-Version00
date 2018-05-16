#if !defined(AFX_INPUTBYTEDLG_H__6AB89AC6_53ED_41DE_B82C_189EF71AAC79__INCLUDED_)
#define AFX_INPUTBYTEDLG_H__6AB89AC6_53ED_41DE_B82C_189EF71AAC79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputByteDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputByteDlg dialog

class CInputByteDlg : public CDialog
{
// Construction
public:
	CInputByteDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputByteDlg)
	enum { IDD = IDD_DIALOG_INPUT_BYTE };
	CString	m_Edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputByteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputByteDlg)
	afx_msg void OnChangeEdit();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTBYTEDLG_H__6AB89AC6_53ED_41DE_B82C_189EF71AAC79__INCLUDED_)
