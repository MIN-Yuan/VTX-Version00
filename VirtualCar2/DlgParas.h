#if !defined(AFX_DLGPARAS_H__8B699EAC_125A_41BC_ABDD_AE9D821DC619__INCLUDED_)
#define AFX_DLGPARAS_H__8B699EAC_125A_41BC_ABDD_AE9D821DC619__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgParas.h : header file
//

#include "VirtualCarWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgParas dialog

class CDlgParas : public CDialog
{
// Construction
public:
	CDlgParas(CWnd* pParent = NULL);   // standard constructor

	CVirtualCarWnd * pmainwin;

// Dialog Data
	//{{AFX_DATA(CDlgParas)
	enum { IDD = IDD_DIALOG_PARAS };
	CListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgParas)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgParas)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPARAS_H__8B699EAC_125A_41BC_ABDD_AE9D821DC619__INCLUDED_)
