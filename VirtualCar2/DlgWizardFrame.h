#if !defined(AFX_DLGWIZARDFRAME_H__FABE04D0_22A7_42C5_9DD7_4CC0BA63A43D__INCLUDED_)
#define AFX_DLGWIZARDFRAME_H__FABE04D0_22A7_42C5_9DD7_4CC0BA63A43D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWizardFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardFrame dialog

class CDlgWizardFrame : public CDialog
{
// Construction
public:
	CDlgWizardFrame(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWizardFrame)
	enum { IDD = IDD_DIALOG_WIZARD_FRAME };
	BOOL	m_DelayRealtime;
	UINT	m_SendDelay;
	UINT	m_ReceiveDelay;
	UINT	m_MiniFrameSpace;
	int		m_FrameType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWizardFrame)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWizardFrame)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonIgnore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWIZARDFRAME_H__FABE04D0_22A7_42C5_9DD7_4CC0BA63A43D__INCLUDED_)
