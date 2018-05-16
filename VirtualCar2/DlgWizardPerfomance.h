#if !defined(AFX_DLGWIZARDPERFOMANCE_H__3E15E31D_E403_4423_845E_8E2830E62A63__INCLUDED_)
#define AFX_DLGWIZARDPERFOMANCE_H__3E15E31D_E403_4423_845E_8E2830E62A63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWizardPerfomance.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardPerfomance dialog

class CDlgWizardPerfomance : public CDialog
{
// Construction
public:
	CDlgWizardPerfomance(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWizardPerfomance)
	enum { IDD = IDD_DIALOG_WIZARD_PERFORMANCE };
	CSliderCtrl	m_SliderSleep;
	CSliderCtrl	m_SliderPriority;
	BOOL	m_WatchList;
	CString	m_EditPriority;
	CString	m_EditSleep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWizardPerfomance)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWizardPerfomance)
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureSliderPriority(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderSleep(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonNext();
	afx_msg void OnCheckWatchList();
	afx_msg void OnButtonIgnore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWIZARDPERFOMANCE_H__3E15E31D_E403_4423_845E_8E2830E62A63__INCLUDED_)
