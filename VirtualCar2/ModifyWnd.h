#if !defined(AFX_MODIFYWND_H__5CF28C14_1813_40A9_992F_AE807CFD4D5E__INCLUDED_)
#define AFX_MODIFYWND_H__5CF28C14_1813_40A9_992F_AE807CFD4D5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyWnd.h : header file
//

#include "GridCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CModifyWnd dialog

#define MAX_ROW 1024
#define MAX_COL 259
#define GRID_FIRST_DATA_COL 3


#define GRID_ROW_HEIGHT 16
#define GRID_TOP 26
#define GRID_FIX_COL_WIDTH 30
#define GRID_DATA_COL_WIDTH 20
#define ID_GRID	123

#define TITLE_BG_COLOR	RGB(200,200,200)
#define TITLE_FG_COLOR  RGB(0,0,0)
#define DATA_BG_COLOR	RGB(255,255,255)
#define DATA_BG_RED_COLOR RGB(0xF5,0xD5,0xC3)
#define DATA_BG_BLUE_COLOR RGB(0xd5,0xc3,0xf5)

#define DATA_FG_COLOR	RGB(20,20,20)

#define TITLE_FG_R_COLOR RGB(90,90,90)
#define TITLE_FG_S_COLOR RGB(110,110,110)

#define TITLE_BG_R_COLOR RGB(150,150,150)
#define DATA_BG_R_COLOR RGB(220,220,220)
#define DATA_BG_S_COLOR	RGB(255,255,255)

typedef struct STRUCT_ROW_MAP_COMM_INFO{
	BYTE TypeR0S1;
	UINT RID;
	BYTE SID;
	BYTE Cnt;

}ROW_MAP_COMM_INFO;

extern ROW_MAP_COMM_INFO RowInfo[];
extern UINT RowInfoCnt;
extern UINT RcvIndex;


extern CMutex GModifyWndMutex;
extern CMutex GLogMutex;


#define MODIFY_TYPE_ADD_01H 1
#define MODIFY_TYPE_SUB_01H	2
#define MODIFY_TYPE_ADD_10H 3
#define MODIFY_TYPE_SUB_10H 4
#define MODIFY_TYPE_MOVE_LEFT 5
#define MODIFY_TYPE_MOVE_RIGHT 6
#define MODIFY_TYPE_ADD_ANY 7
#define MODIFY_TYPE_SUB_ANY 8


#define MAX_MODIFY	512

typedef struct STRUCT_MODIFY{
	BYTE Type;
	BYTE Row;
	BYTE Col;
	BYTE Var;

	BYTE RID;
	BYTE SID;
	BYTE Pos;
}MODIFY;




class CModifyWnd : public CDialog
{
// Construction
public:
	CModifyWnd(CWnd* pParent = NULL);   // standard constructor

	CGridCtrl m_grid;

	void OnOK();

	void AddModifyReplace(int row, int col, char * hexstr);
	void FreeModify(int row, int col);
	void SaveRow(int row, BOOL redraw);
	void AddModifyAlways(int row, int col, unsigned char type, BYTE var);

	void ProcessModify(UINT rid, unsigned char sid);
	void RefreshSendOrder(UINT rid, unsigned char sid);
	void UpdateGridXNumber();

	//pupup menu handle
	CMenu menu;
	CMenu *pPopup;

	INT GridRowCount;

// Dialog Data
	//{{AFX_DATA(CModifyWnd)
	enum { IDD = IDD_DIALOG_MODIFY };
	BOOL	m_CheckModifyAlways;
	BOOL	m_GridXHex0Dec1;
	int		m_GridXOffset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void DrawCommToGrid(int row);


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModifyWnd)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonReset();
	afx_msg void OnButton00();
	afx_msg void OnButton7f();
	afx_msg void OnButton80();
	afx_msg void OnButtonFf();
	afx_msg void OnButton01();
	afx_msg void OnButtonFree();
	afx_msg void OnButtonDoubleRandom();
	afx_msg void OnMenuitemModifyReplace00();
	afx_msg void OnMenuitemModifyReplace01();
	afx_msg void OnMenuitemModifyReplace7f();
	afx_msg void OnMenuitemFree();
	afx_msg void OnMenuitemModifyReplace80();
	afx_msg void OnMenuitemModifyReplaceDran();
	afx_msg void OnMenuitemModifyReplaceFf();
	afx_msg void OnMenuitemSaveRow();
	afx_msg void OnCheckAllways();
	afx_msg void OnButtonSaveall();
	afx_msg void OnButtonModifyAdd1();
	afx_msg void OnButtonModifySub01();
	afx_msg void OnButtonModifyAdd10();
	afx_msg void OnButtonModifyMoveleft();
	afx_msg void OnButtonModifyMoveright();
	afx_msg void OnButtonModifySub10();
	afx_msg void OnMenuitemAdd01();
	afx_msg void OnMenuitemSub01();
	afx_msg void OnMenuitemAdd10();
	afx_msg void OnMenuitemSub10();
	afx_msg void OnMenuitemMoveleft();
	afx_msg void OnMenuitemMoveright();
	afx_msg void OnMenuitemAlwaysAdd01();
	afx_msg void OnMenuitemAlwaysSub01();
	afx_msg void OnMenuitemAlwaysAdd10();
	afx_msg void OnMenuitemAlwaysSub10();
	afx_msg void OnMenuitemAlwaysMoveleft();
	afx_msg void OnMenuitemAlwaysMoveright();
	afx_msg void OnButtonModifyAddAny();
	afx_msg void OnButtonModifySubAny();
	afx_msg void OnMenuitemAlwaysAddany();
	afx_msg void OnMenuitemAlwaysSubany();
	afx_msg void OnButtonSaveRow();
	afx_msg void OnMenuitemModifyReplaceStar();
	afx_msg void OnButtonStar();
	afx_msg void OnCheckHex0dec1();
	afx_msg void OnKillfocusEditGridxOffset();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



extern CModifyWnd * PModifyWnd;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYWND_H__5CF28C14_1813_40A9_992F_AE807CFD4D5E__INCLUDED_)
