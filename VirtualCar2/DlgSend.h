#if !defined(AFX_DLGSEND_H__45ABD290_DA89_4901_9E07_83366D1D3DB7__INCLUDED_)
#define AFX_DLGSEND_H__45ABD290_DA89_4901_9E07_83366D1D3DB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSend.h : header file
//
#include "MyListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSend dialog

class CDlgSend : public CDialog
{
// Construction
public:
	CDlgSend(CWnd* pParent = NULL);   // standard constructor

	CString str_result;

// Dialog Data
	//{{AFX_DATA(CDlgSend)
	enum { IDD = IDD_DIALOG_SEND };
	CMyListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSend)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSend)
	afx_msg void OnClickListSend(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListSend(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSEND_H__45ABD290_DA89_4901_9E07_83366D1D3DB7__INCLUDED_)
