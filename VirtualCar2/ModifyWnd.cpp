// ModifyWnd.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "ModifyWnd.h"
#include "VirtualCarWnd.h"
#include "DTXML.h"
#include "base.h"
#include "resource.h"
#include "InputByteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



ROW_MAP_COMM_INFO RowInfo[MAX_ROW];
UINT RowInfoCnt;
CModifyWnd * PModifyWnd=0;

UINT RcvIndex;

MODIFY Modify[MAX_MODIFY];
INT ModifyCount=0;

CMutex GModifyWndMutex;
CMutex GLogMutex;


extern BOOL GTaoDlgOpen ;
/////////////////////////////////////////////////////////////////////////////
// CModifyWnd dialog

CModifyWnd::CModifyWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyWnd)
	m_CheckModifyAlways = GModifyInitAlways;
	m_GridXHex0Dec1 = FALSE;
	m_GridXOffset = -1;
	//}}AFX_DATA_INIT


}


void CModifyWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyWnd)
	DDX_Check(pDX, IDC_CHECK_ALLWAYS, m_CheckModifyAlways);
	DDX_Check(pDX, IDC_CHECK_HEX0DEC1, m_GridXHex0Dec1);
	DDX_Text(pDX, IDC_EDIT_GRIDX_OFFSET, m_GridXOffset);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModifyWnd, CDialog)
	//{{AFX_MSG_MAP(CModifyWnd)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_00, OnButton00)
	ON_BN_CLICKED(IDC_BUTTON_7F, OnButton7f)
	ON_BN_CLICKED(IDC_BUTTON_80, OnButton80)
	ON_BN_CLICKED(IDC_BUTTON_FF, OnButtonFf)
	ON_BN_CLICKED(IDC_BUTTON_01, OnButton01)
	ON_BN_CLICKED(IDC_BUTTON_FREE, OnButtonFree)
	ON_BN_CLICKED(IDC_BUTTON_DOUBLE_RANDOM, OnButtonDoubleRandom)
	ON_COMMAND(ID_MENUITEM_MODIFY_REPLACE_00, OnMenuitemModifyReplace00)
	ON_COMMAND(ID_MENUITEM_MODIFY_REPLACE_01, OnMenuitemModifyReplace01)
	ON_COMMAND(ID_MENUITEM_MODIFY_REPLACE_7F, OnMenuitemModifyReplace7f)
	ON_COMMAND(ID_MENUITEM_FREE, OnMenuitemFree)
	ON_COMMAND(ID_MENUITEM_MODIFY_REPLACE_80, OnMenuitemModifyReplace80)
	ON_COMMAND(ID_MENUITEM_MODIFY_REPLACE_DRAN, OnMenuitemModifyReplaceDran)
	ON_COMMAND(ID_MENUITEM_MODIFY_REPLACE_FF, OnMenuitemModifyReplaceFf)
	ON_COMMAND(ID_MENUITEM_SAVE_ROW, OnMenuitemSaveRow)
	ON_BN_CLICKED(IDC_CHECK_ALLWAYS, OnCheckAllways)
	ON_BN_CLICKED(IDC_BUTTON_SAVEALL, OnButtonSaveall)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_ADD1, OnButtonModifyAdd1)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_SUB01, OnButtonModifySub01)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_ADD10, OnButtonModifyAdd10)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_MOVELEFT, OnButtonModifyMoveleft)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_MOVERIGHT, OnButtonModifyMoveright)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_SUB10, OnButtonModifySub10)
	ON_COMMAND(ID_MENUITEM_ADD01, OnMenuitemAdd01)
	ON_COMMAND(ID_MENUITEM_SUB01, OnMenuitemSub01)
	ON_COMMAND(ID_MENUITEM_ADD10, OnMenuitemAdd10)
	ON_COMMAND(ID_MENUITEM_SUB10, OnMenuitemSub10)
	ON_COMMAND(ID_MENUITEM_MOVELEFT, OnMenuitemMoveleft)
	ON_COMMAND(ID_MENUITEM_MOVERIGHT, OnMenuitemMoveright)
	ON_COMMAND(ID_MENUITEM_ALWAYS_ADD01, OnMenuitemAlwaysAdd01)
	ON_COMMAND(ID_MENUITEM_ALWAYS_SUB01, OnMenuitemAlwaysSub01)
	ON_COMMAND(ID_MENUITEM_ALWAYS_ADD10, OnMenuitemAlwaysAdd10)
	ON_COMMAND(ID_MENUITEM_ALWAYS_SUB10, OnMenuitemAlwaysSub10)
	ON_COMMAND(ID_MENUITEM_ALWAYS_MOVELEFT, OnMenuitemAlwaysMoveleft)
	ON_COMMAND(ID_MENUITEM_ALWAYS_MOVERIGHT, OnMenuitemAlwaysMoveright)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_ADD_ANY, OnButtonModifyAddAny)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_SUB_ANY, OnButtonModifySubAny)
	ON_COMMAND(ID_MENUITEM_ALWAYS_ADDANY, OnMenuitemAlwaysAddany)
	ON_COMMAND(ID_MENUITEM_ALWAYS_SUBANY, OnMenuitemAlwaysSubany)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_ROW, OnButtonSaveRow)
	ON_COMMAND(ID_MENUITEM_MODIFY_REPLACE_STAR, OnMenuitemModifyReplaceStar)
	ON_BN_CLICKED(IDC_BUTTON_STAR, OnButtonStar)
	ON_BN_CLICKED(IDC_CHECK_HEX0DEC1, OnCheckHex0dec1)
	ON_EN_KILLFOCUS(IDC_EDIT_GRIDX_OFFSET, OnKillfocusEditGridxOffset)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// CModifyWnd message handlers

BOOL CModifyWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here

	SetIcon(GHIcon,TRUE);
	SetIcon(GHIcon,FALSE);



	//Load App Config file
	CDTXML cfg;
	if(cfg.Open(STR_APP_CONFIG_FILE))
	{
		cfg.Enter("TOP");
		cfg.Enter("TAOWND");

		INT MAXIMIZED;
		INT x,y,width,height;
		RECT rect;

		MAXIMIZED=cfg.GetInteger("MAXIMIZED",0);
		x=cfg.GetInteger("X",20);
		y=cfg.GetInteger("Y",20);
		width=cfg.GetInteger("WIDTH",940);
		height=cfg.GetInteger("HEIGHT",675);
		GetDesktopWindow()->GetWindowRect(&rect);

		//纠正位置
		if(x<0) x=0;
		if(y<0) y=0;
		if(x>rect.right-rect.left) x=rect.right-rect.left-20;
		if(y>rect.bottom-rect.top) y=rect.bottom-rect.top;

		if(MAXIMIZED){
			WINDOWPLACEMENT p;
			GetWindowPlacement(&p);
			p.showCmd=SW_SHOWMAXIMIZED;
			SetWindowPlacement(&p);
		}else{
			MoveWindow(x,y,width,height,FALSE);
		}

		cfg.Close();
	}





	RECT rect;
	GetClientRect(&rect);

	rect.top+=GRID_TOP;


	m_grid.Create(rect,this,ID_GRID);


	m_grid.SetColumnResize(TRUE);
	m_grid.SetRowResize(FALSE);

	m_grid.SetColumnCount(MAX_COL);


	//算出要用多少行表格
	int i,j;
	GridRowCount=1; //顶部固定1行
	for(i=0; i<CommCount; i++)
	{
		GridRowCount++;
		for(j=0; j<Comm[i].SCount; j++)
		{
			GridRowCount++;
		}
	}
	m_grid.SetRowCount(GridRowCount);


	m_grid.SetFixedRowCount(1);
	m_grid.SetFixedColumnCount(2);

	
	//设置标题列宽度
	m_grid.SetColumnWidth(0,GRID_FIX_COL_WIDTH);
	m_grid.SetColumnWidth(1,GRID_FIX_COL_WIDTH);
	m_grid.SetColumnWidth(2,120);
	m_grid.SetColumnWidth(3,GRID_FIX_COL_WIDTH);


	//设置数据列宽度
	for(i=GRID_FIRST_DATA_COL; i<MAX_COL; i++)
	{
		m_grid.SetColumnWidth(i,GRID_DATA_COL_WIDTH);
	}

	for(i=0; i<GridRowCount; i++){
		m_grid.SetRowHeight(i,GRID_ROW_HEIGHT);
	}
// 
// 	//load menu resource and point to PopMenu
	menu.LoadMenu(IDR_MENU_MODIFY_POPUP);
	pPopup=menu.GetSubMenu(0);
	//::SetMenuDefaultItem(pPopup->m_hMenu,0,TRUE); //设置粗体字

	//m_grid.EnableSelection(FALSE);
// 
// 	
// 
// 	//GetDesktopWindow()->GetWindowRect(&rect);
// 
	OnButtonReset();

	PModifyWnd=this;
	GModifyWndMutex.Unlock();


	//this->SetFocus();
	this->SetForegroundWindow();
	
//	AfxMessageBox("init ok") ;
	return TRUE;  // return TRUE unless you set the focus to a control

}

BOOL CModifyWnd::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	// TODO: Add your specialized code here and/or call the base class
    if(pMsg -> message == WM_KEYDOWN)
	{
        if(pMsg -> wParam == VK_ESCAPE)
			return TRUE;
		//if(pMsg -> wParam == VK_RETURN)
		//	return TRUE;
	}	
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CModifyWnd::OnOK()
{

}

void CModifyWnd::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	//if(nType==SIZE_MAXIMIZED){
	//	return;
	//}

	if(!IsWindowVisible()){
		return;
	}

	RECT rect;
	CWnd * pwnd;


	GetClientRect(&rect);
	rect.top+=GRID_TOP;
	pwnd=GetDlgItem(ID_GRID);
	if(IsWindow(*pwnd)){
		pwnd->MoveWindow(&rect,FALSE);
	}

	RedrawWindow(NULL);
}


// void CModifyWnd::OnButton1() 
// {
// 	// TODO: Add your control notification handler code here
// 	m_grid.SetItemText(4,4,"255");
// 	m_grid.RedrawCell(4,4);
// 
// }


void CModifyWnd::OnButtonReset() 
{
	// TODO: Add your control notification handler code here
	int i,j,k;
	char buf[32];
	
	GV_ITEM Item;

	PVirtualCarWnd->LoadComm(GConfigFile);

	// fill rows/cols with text
	for (i=0; i<GridRowCount; i++){
		for(j=0; j<MAX_COL; j++){
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = i;
			Item.col = j;

			if(i<m_grid.GetFixedRowCount()){	//横标题
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE/*DT_WORDBREAK*/;
				m_grid.SetItemText(i,j,"");
				m_grid.SetItemBkColour(i,j,TITLE_BG_COLOR);
				m_grid.SetItemFgColour(i,j,TITLE_FG_COLOR);
			}else if(j<m_grid.GetFixedColumnCount()){
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE/*|DT_END_ELLIPSIS*/;
				m_grid.SetItemText(i,j,"");
				m_grid.SetItemBkColour(i,j,TITLE_BG_COLOR);
				m_grid.SetItemFgColour(i,j,TITLE_FG_COLOR);
			}else{
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE/*|DT_END_ELLIPSIS*/;
				m_grid.SetItemText(i,j,"");
				m_grid.SetItemBkColour(i,j,DATA_BG_COLOR);
				m_grid.SetItemFgColour(i,j,DATA_FG_COLOR);

				if(j==2){
					Item.nFormat=DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
				}
			}
			m_grid.SetItem(&Item);
			m_grid.GetCell(i,j)->SetMargin(1);
		}
	}
	

	UpdateGridXNumber();

	m_grid.SetItemText(0,0,"编号");
	m_grid.SetItemText(0,1,"计数");
	m_grid.SetItemText(0,2,"顺序(十进制)");


	//装载数据库中数据
	RowInfoCnt=0;
	for(i=0; i<CommCount; i++){
		sprintf(buf,"%d",i);
		m_grid.SetItemText(RowInfoCnt+1,0,buf);

		for(k=0; k<2; k++){
			m_grid.SetItemBkColour(RowInfoCnt+1,k,TITLE_BG_R_COLOR);
		}
		for(k=2; k<MAX_COL; k++){
			m_grid.SetItemBkColour(RowInfoCnt+1,k,DATA_BG_R_COLOR);
		}

		RowInfo[RowInfoCnt].TypeR0S1=0;
		RowInfo[RowInfoCnt].RID=i;
		RowInfoCnt++;
	

		for(j=0; j<Comm[i].SCount; j++){
			sprintf(buf,"%d",j);
			m_grid.SetItemText(RowInfoCnt+1,0,buf);
			m_grid.SetItemFgColour(RowInfoCnt+1,0,TITLE_FG_S_COLOR);
			RowInfo[RowInfoCnt].TypeR0S1=1;
			RowInfo[RowInfoCnt].RID=i;
			RowInfo[RowInfoCnt].SID=j;
			RowInfoCnt++;
		}
	}


	//从COMM刷新数据到表格
	for(i=0; i<RowInfoCnt; i++){
		DrawCommToGrid(i);
	}

	//清空所有Modify
	ModifyCount=0;


	//清空RcvIndex;
	RcvIndex=0;


	m_grid.InvalidateRect(NULL);

}


void CModifyWnd::SaveRow(int row, BOOL redraw)
{
	CString str;
	int i;
	CDTXML xml;
	//CString tag;

	if(row-1>=RowInfoCnt){
		return;
	}

	//把格中的HEX串提出来
	i=GRID_FIRST_DATA_COL;
	while(m_grid.GetItemText(row,i).GetLength()==2){
		str+=m_grid.GetItemText(row,i);
		str+=" ";
		i++;
	}



	//将提出来的串存到文件
	if(xml.Open(GConfigFile.GetBuffer(MAX_PATH))){
		if(xml.Enter(STR_CONTENT)){
			if(xml.Enter(STR_COMMS)){
				if(xml.Enter(STR_COMM,RowInfo[row-1].RID)){
					if(RowInfo[row-1].TypeR0S1==0){
						xml.SetString(STR_RECEIVE,str.GetBuffer(1024));
					}else{
						if(xml.Enter(STR_SEND,RowInfo[row-1].SID)){
							xml.SetString(STR_DATA,str.GetBuffer(1024));
							xml.Back();
						}
					}
				}
			}
		}
		
		xml.Save();
	}

	
	for(i=GRID_FIRST_DATA_COL; i<MAX_COL; i++){
		if(RowInfo[row-1].TypeR0S1==0){
			if(m_grid.GetItemBkColour(row,i)!=DATA_BG_R_COLOR){
				m_grid.SetItemBkColour(row,i,DATA_BG_R_COLOR);
				//m_grid.RedrawRow(row);
			}
		}else{
			if(m_grid.GetItemBkColour(row,i)==DATA_BG_RED_COLOR){
				m_grid.SetItemBkColour(row,i,DATA_BG_S_COLOR);
				//m_grid.RedrawRow(row);
			}
		}
	}
	
	if(redraw){
		m_grid.RedrawRow(row);
	}
}

BOOL CModifyWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	int row, col;
	CPoint Point;
	char c;
	int i;
	CString str;


	if (wParam == (WPARAM)m_grid.GetDlgCtrlID())
    {
        *pResult = 1;
        GV_DISPINFO *pDispInfo = (GV_DISPINFO*)lParam;
		switch(pDispInfo->hdr.code){
		case NM_RCLICK:
			row=m_grid.GetFocusCell().row;
			col=m_grid.GetFocusCell().col;
			if(col>=GRID_FIRST_DATA_COL && row>=1 && row-1<RowInfoCnt){

				if(RowInfo[row-1].TypeR0S1==0){
//					if(col-GRID_FIRST_DATA_COL >= Comm[RowInfo[row-1].RID.S[RowInfo[row-1].SID].GetLength/2 ){
//						break;
//					}

					//pPopup->EnableMenuItem(ID_MENUITEM_EXP_SWITCH,MF_GRAYED);
					//pPopup->EnableMenuItem(ID_MENUITEM_EXP_ARITH,MF_GRAYED);

				}else{
					//pPopup->EnableMenuItem(ID_MENUITEM_EXP_SWITCH,MF_ENABLED);
					//pPopup->EnableMenuItem(ID_MENUITEM_EXP_ARITH,MF_ENABLED);
				}

				GetCursorPos(&Point);
				pPopup->TrackPopupMenu(TPM_LEFTALIGN,Point.x,Point.y,this,NULL);
			}
			break;

		case GVN_ENDLABELEDIT:
			row=m_grid.GetFocusCell().row;
			col=m_grid.GetFocusCell().col;

			str=m_grid.GetItemText(row,col);
			
			if(str.GetLength()>2){
				str=str.Left(2);
			}

			if(str.GetLength()<=0){
				str="00";
			}


			if(RowInfo[row-1].TypeR0S1==0){

				if(str.GetLength()<2){
					str.Replace(" ","00");

					if(str.GetAt(0)=='*'){
						str.Insert(1,' ');
					}else if(str.GetAt(0)==' '){
						str="00";
					}else{
						str.Insert(0,'0');
					}
				}else{
					str.Replace("  ","00");
					str.Replace("**","* ");
				}

				for(i=0; i<2; i++){
					c=str.GetAt(i);
					if(!(
						(c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F') || c=='?' || c=='*' || c==' '
						)){
						str.SetAt(i,'0');
					}
				}
			}else{

				if(str.GetLength()<2){
					str.Insert(0,'0');
				}

				for(i=0; i<2; i++){
					c=str.GetAt(i);
					if(!(
						(c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F') || c=='?'
						)){
						str.SetAt(i,'0');
					}
				}

			}

			AddModifyReplace(row,col,str.GetBuffer(4));

			break;

		}
    }

	return CDialog::OnNotify(wParam, lParam, pResult);
}


void CModifyWnd::DrawCommToGrid(int row)
{
	int i;
	int j;
	int k;
	int len;
	CString str;

	if(row>=RowInfoCnt){
		return;
	}

	if(RowInfo[row].TypeR0S1==0)
	{
		j=RowInfo[row].RID;
		str=Comm[j].Receive;
		str.Replace("*","* ");
		len=str.GetLength();
		len=len/2;
		for(i=0; i<len; i++)
		{
		
			m_grid.SetItemText(row+1,GRID_FIRST_DATA_COL+i,str.Mid(i*2,2));
			
		}
	}else{
		j=RowInfo[row].RID;
		k=RowInfo[row].SID;
		str=Comm[j].Send[k].Data;
		str.Replace("*","* ");
		len=str.GetLength();
		len=len/2;
		for(i=0; i<len; i++)
		{	//0
			int a = i* 2 ;
			int b = 0;
			b+= a ;
			int c = Comm[j].Send[k].Data.GetLength() ;
			if(b<c)
			{
				m_grid.SetItemText(row+1,GRID_FIRST_DATA_COL+i,Comm[j].Send[k].Data.Mid(a,2));
			}
		}
	}
}


void CModifyWnd::AddModifyReplace(int row, int col, char * hexstr)
{
	int i;

	if((row-1)>=RowInfoCnt){
		return;
	}

	if(row<1){
		return;
	}

	if(col<GRID_FIRST_DATA_COL){
		return;
	}
	
	//更改COMM中的数据
	unsigned char typer0s1;
	UINT rid;	//RTT20080827
	unsigned char sid;
	unsigned char pos;
	rid=RowInfo[row-1].RID;
	sid=RowInfo[row-1].SID;
	typer0s1=RowInfo[row-1].TypeR0S1;
	pos=col-GRID_FIRST_DATA_COL;	//字节位置

	if(typer0s1==0){
		//如果指定的格位,是新增的,增加相应的字节数
		if(pos>=(Comm[rid].Receive.GetLength()/2)){
			//在Pos前增加??
			for(i=Comm[rid].Receive.GetLength()/2; i<pos; i++){
				//增加数据
				Comm[rid].Receive+="??";

				//刷成红色
				m_grid.SetItemBkColour(row,GRID_FIRST_DATA_COL+i,DATA_BG_RED_COLOR);
				m_grid.SetItemText(row,GRID_FIRST_DATA_COL+i,"??");
				m_grid.RedrawCell(row,GRID_FIRST_DATA_COL+i);
			}

			//增加字节
			Comm[rid].Receive+=hexstr;
		}

		//替换字节
		Comm[rid].Receive.Delete(pos*2,2);
		Comm[rid].Receive.Insert(pos*2,hexstr);

		//非蓝色时才刷成红色
		if(m_grid.GetItemBkColour(row,col)!=DATA_BG_BLUE_COLOR){
			m_grid.SetItemBkColour(row,col,DATA_BG_RED_COLOR);
		}
		m_grid.SetItemText(row,col,hexstr);
		m_grid.RedrawCell(row,col);


	}else{
		
		//如果指定的格位,是新增的,增加相应的字节数
		if(pos>=(Comm[rid].Send[sid].Data.GetLength()/2)){
			//在Pos前增加??
			for(i=Comm[rid].Send[sid].Data.GetLength()/2; i<pos; i++){
				//增加数据
				Comm[rid].Send[sid].Data+="??";

				//刷成红色
				m_grid.SetItemBkColour(row,GRID_FIRST_DATA_COL+i,DATA_BG_RED_COLOR);
				m_grid.SetItemText(row,GRID_FIRST_DATA_COL+i,"??");
				m_grid.RedrawCell(row,GRID_FIRST_DATA_COL+i);
			}

			//增加字节
			Comm[rid].Send[sid].Data+=hexstr;
		}

		//替换字节
		Comm[rid].Send[sid].Data.Delete(pos*2,2);
		Comm[rid].Send[sid].Data.Insert(pos*2,hexstr);

		//非蓝色时才刷成红色
		if(m_grid.GetItemBkColour(row,col)!=DATA_BG_BLUE_COLOR){
			m_grid.SetItemBkColour(row,col,DATA_BG_RED_COLOR);
		}
		m_grid.SetItemText(row,col,hexstr);
		m_grid.RedrawCell(row,col);

	}


	//清除Selection
	//m_grid.ResetSelectedRange();
	//m_grid.SetFocusCell(row,col);

}

void CModifyWnd::RefreshSendOrder(UINT rid, unsigned char sid)
{
	int i;
	CString str;
	char buffer[16];

	for(i=0; i<RowInfoCnt; i++){
		if(RowInfo[i].TypeR0S1==1){
			if(RowInfo[i].RID==rid && RowInfo[i].SID==sid){
				str=m_grid.GetItemText(i+1,2);
				str+=" ";
				str+=itoa(RcvIndex,buffer,10);
				
				if(str.GetLength()>1024){
					str=str.Right(16);
				}

				m_grid.SetItemText(i+1,2,str);

				m_grid.RedrawCell(i+1,2);
				break;
			}
		}
	}
}

void  CModifyWnd::ProcessModify(UINT rid, unsigned char sid)
{
	int modifyid=0xff1;
	int i;
	CString str;
	BYTE b;
	UINT l;

	for(i=0; i<ModifyCount; i++){
		if(Modify[i].RID==rid && Modify[i].SID==sid){
			if(Modify[i].Pos<Comm[rid].Send[sid].Data.GetLength()/2){

				//当前Comm[]中的值
				str=Comm[rid].Send[sid].Data.Mid(Modify[i].Pos*2,2);
				if(HexString2Buffer(str.GetBuffer(4),&b,1,&l)==0){
					b=0x00;
				}

				switch(Modify[i].Type){
				case MODIFY_TYPE_ADD_01H:
					b+=0x01;
					break;
				case MODIFY_TYPE_SUB_01H:
					b-=0x01;
					break;
				case MODIFY_TYPE_ADD_10H:
					b+=0x10;
					break;
				case MODIFY_TYPE_SUB_10H:
					b-=0x10;
					break;
				case MODIFY_TYPE_MOVE_LEFT:
					if(b==0x00){
						b=Modify[i].Var;
					}else{
						b<<=1;
					}
					break;
				case MODIFY_TYPE_MOVE_RIGHT:
					if(b==0x00){
						b=Modify[i].Var;
					}else{
						b>>=1;
					}
					break;
				case MODIFY_TYPE_ADD_ANY:
					b+=Modify[i].Var;
					break;
				case MODIFY_TYPE_SUB_ANY:
					b-=Modify[i].Var;
					break;
				}

				//将改变后的值放回Comm中
				str.Format("%2x",b);
				str.Replace(' ','0');
				
				Comm[rid].Send[sid].Data.SetAt(Modify[i].Pos*2,str.GetAt(0));
				Comm[rid].Send[sid].Data.SetAt(Modify[i].Pos*2+1,str.GetAt(1));

				m_grid.SetItemText(Modify[i].Row, Modify[i].Col,str.GetBuffer(4));
				m_grid.RedrawCell(Modify[i].Row, Modify[i].Col);

				
			}
		}
	}

}


void CModifyWnd::AddModifyAlways(int row, int col, unsigned char type, BYTE var)
{
	int i;
	CString str;
	UINT l;

	if(ModifyCount>=MAX_MODIFY){
		return;
	}

	if((row-1)>=RowInfoCnt){
		return;
	}

	if(row<1){
		return;
	}

	if(col<GRID_FIRST_DATA_COL){
		return;
	}

	if(RowInfo[row-1].TypeR0S1!=1){
		return;
	}

	if(col-GRID_FIRST_DATA_COL >= Comm[RowInfo[row-1].RID].Send[RowInfo[row-1].SID].Data.GetLength()/2){
		return;
	}

	//找到ModifyId
	UINT modifyid;	//RTT20080827
	modifyid=0xff;
	for(i=0; i<ModifyCount; i++){
		if(Modify[i].Row==row && Modify[i].Col==col){
			modifyid=i;
		}
	}

	//找到
	if(modifyid!=0xff){
		//将modifyid后面的向前移,以删除它
		for(i=modifyid+1; i<ModifyCount; i++){
			Modify[i-1].Type=Modify[i].Type;
			Modify[i-1].Col=Modify[i].Col;
			Modify[i-1].Row=Modify[i].Row;
			Modify[i-1].Var=Modify[i].Var;
		}
		ModifyCount--;
	}




	//增加Modify记录
	Modify[ModifyCount].Row=row;
	Modify[ModifyCount].Col=col;
	Modify[ModifyCount].Type=type;

	if(type==MODIFY_TYPE_ADD_ANY || type==MODIFY_TYPE_SUB_ANY ){
		Modify[ModifyCount].Var=var;
	}else{
		//找到原来的值存在Var中
		str=Comm[RowInfo[row-1].RID].Send[RowInfo[row-1].SID].Data.Mid( (col-GRID_FIRST_DATA_COL)*2,2 );
		if(HexString2Buffer(str.GetBuffer(4),&(Modify[ModifyCount].Var),1,&l)==0){
			return;
		}
	}

	Modify[ModifyCount].RID=RowInfo[row-1].RID;
	Modify[ModifyCount].SID=RowInfo[row-1].SID;
	Modify[ModifyCount].Pos=col-GRID_FIRST_DATA_COL;


	//刷成蓝色
	m_grid.SetItemBkColour(row,col,DATA_BG_BLUE_COLOR);
	m_grid.RedrawCell(row,col);

	ModifyCount++;

}

void CModifyWnd::FreeModify(int row, int col)
{

	int i;
	//char tag[32];
	char buf[4096];
	CString str;
	unsigned long outlen;

	if((row-1)>=RowInfoCnt){
		return;
	}

	if(row<1){
		return;
	}

	if(col<GRID_FIRST_DATA_COL){
		return;
	}
	

	//if(RowInfo[row-1].TypeR0S1==0){
	//	return;
	//}


	
	//找到ModifyId
	unsigned char modifyid;
	modifyid=0xff;
	for(i=0; i<ModifyCount; i++){
		if(Modify[i].Row==row && Modify[i].Col==col){
			modifyid=i;
		}
	}

	//找到
	if(modifyid!=0xff){
		//将modifyid后面的向前移,以删除它
		for(i=modifyid+1; i<ModifyCount; i++){
			Modify[i-1].Type=Modify[i].Type;
			Modify[i-1].Col=Modify[i].Col;
			Modify[i-1].Row=Modify[i].Row;
			Modify[i-1].Var=Modify[i].Var;
		}
		ModifyCount--;
	}





	//从文件中Load该条数据到COMM
	unsigned char typer0s1;
	UINT rid;	//RTT20080827
	unsigned char sid;
	unsigned char pos;
	CDTXML xml;
	int len;

	rid=RowInfo[row-1].RID;
	sid=RowInfo[row-1].SID;
	typer0s1=RowInfo[row-1].TypeR0S1;
	pos=col-GRID_FIRST_DATA_COL;	//字节位置

	//从文件中还原pos字节
	if(typer0s1==0){
		
		if(xml.Open(GConfigFile.GetBuffer(MAX_PATH))){
			if(xml.Enter(STR_CONTENT)){
				if(xml.Enter(STR_COMMS)){
					if(xml.Enter(STR_COMM,rid)){
						if(xml.GetString(STR_RECEIVE,4096,buf,outlen)){

							//整理.
							str=buf;
							str.Remove(' ');
							str.Remove(',');
							str.Replace("*","* ");
							strcpy(buf,str.GetBuffer(4096));

							len=strlen(buf);
							if(pos*2+1<len){ //在文件中的字节以内
								Comm[rid].Receive.Delete(pos*2, 2);
								Comm[rid].Receive.Insert(pos*2, buf[pos*2]);
								Comm[rid].Receive.Insert(pos*2+1, buf[pos*2+1]);

								//刷新Modify格子
								m_grid.SetItemBkColour(row, col,DATA_BG_R_COLOR);
								m_grid.SetItemText(row,col,Comm[rid].Receive.Mid(pos*2,2));
								m_grid.RedrawCell(row,col);

							}else{	//在文件中的字节外
								Comm[rid].Receive=Comm[rid].Receive.Left(pos*2);

								//刷新Modify格子
								for(i=col; i<MAX_COL; i++){
									m_grid.SetItemBkColour(row, i,DATA_BG_R_COLOR);
									m_grid.SetItemText(row,i,"");
									m_grid.RedrawCell(row,i);
								}

							}
						}
					}
				}
			}
		}

	
	}else{

		if(xml.Open(GConfigFile.GetBuffer(MAX_PATH))){
			if(xml.Enter(STR_CONTENT)){
				if(xml.Enter(STR_COMMS)){
					if(xml.Enter(STR_COMM,rid)){
						if(xml.Enter(STR_SEND,sid)){
							if(xml.GetString(STR_DATA,4096,buf,outlen)){
								TrimString(buf, 2, ',' , ' ');
								len=strlen(buf);
								if(pos*2+1<len){ //在文件中的字节以内
									Comm[rid].Send[sid].Data.Delete(pos*2, 2);
									Comm[rid].Send[sid].Data.Insert(pos*2, buf[pos*2]);
									Comm[rid].Send[sid].Data.Insert(pos*2+1, buf[pos*2+1]);

									//刷新Modify格子
									m_grid.SetItemBkColour(row, col,DATA_BG_S_COLOR);
									m_grid.SetItemText(row,col,Comm[rid].Send[sid].Data.Mid(pos*2,2));
									m_grid.RedrawCell(row,col);

								}else{	//在文件中的字节外
									Comm[rid].Send[sid].Data=Comm[rid].Send[sid].Data.Left(pos*2);

									//刷新Modify格子
									for(i=col; i<MAX_COL; i++){
										m_grid.SetItemBkColour(row, i,DATA_BG_S_COLOR);
										m_grid.SetItemText(row,i,"");
										m_grid.RedrawCell(row,i);
									}
								}
							}
						}
					}
				}
			}
		}

	
	}


}

void CModifyWnd::OnButton00()
{
	CCellRange range;
	int i,j;

	if(m_grid.GetSelectedCount()>0){
		range=m_grid.GetSelectedCellRange();
		for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
			for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
				if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
					AddModifyReplace(j,i,"00");
			}
		}

	}

	//AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,"00");

	m_grid.SetFocus();

}

void CModifyWnd::OnButton7f() 
{
	CCellRange range;
	int i,j;

	if(m_grid.GetSelectedCount()>0){
		range=m_grid.GetSelectedCellRange();
		for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
			for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
				if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
					AddModifyReplace(j,i,"7F");
			}
		}

	}

	//AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,"7F");
	m_grid.SetFocus();
}

void CModifyWnd::OnButton80() 
{
	CCellRange range;
	int i,j;

	if(m_grid.GetSelectedCount()>0){
		range=m_grid.GetSelectedCellRange();
		for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
			for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
				if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
					AddModifyReplace(j,i,"80");
			}
		}

	}

	//AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,"80");
	m_grid.SetFocus();
	
}

void CModifyWnd::OnButtonFf() 
{
	CCellRange range;
	int i,j;

	if(m_grid.GetSelectedCount()>0){
		range=m_grid.GetSelectedCellRange();
		for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
			for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
				if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
					AddModifyReplace(j,i,"FF");
			}
		}

	}

	//AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,"FF");
	m_grid.SetFocus();
	
}

void CModifyWnd::OnButton01() 
{
	CCellRange range;
	int i,j;

	if(m_grid.GetSelectedCount()>0){
		range=m_grid.GetSelectedCellRange();
		for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
			for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
				if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
					AddModifyReplace(j,i,"01");
			}
		}

	}

	//AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,"01");
	m_grid.SetFocus();
	
}

void CModifyWnd::OnButtonFree() 
{
	CCellRange range;
	int i,j;

	if(m_grid.GetSelectedCount()>0){
		range=m_grid.GetSelectedCellRange();
		for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
			for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
				FreeModify(j,i);
			}
		}

	}

	//FreeModify(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col);
	
	m_grid.SetFocus();
}

void CModifyWnd::OnButtonDoubleRandom() 
{
	CCellRange range;
	int i,j;

	if(m_grid.GetSelectedCount()>0){
		range=m_grid.GetSelectedCellRange();
		for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
			for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
				if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
					AddModifyReplace(j,i,"??");
			}
		}

	}

	//AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,"??");
	m_grid.SetFocus();
}

void CModifyWnd::OnMenuitemModifyReplace00() 
{
	// TODO: Add your command handler code here
	OnButton00();	
}


void CModifyWnd::OnMenuitemModifyReplace01() 
{
	// TODO: Add your command handler code here
	OnButton01();	
	
}

void CModifyWnd::OnMenuitemModifyReplace7f() 
{
	// TODO: Add your command handler code here
	OnButton7f();	
}

void CModifyWnd::OnMenuitemFree() 
{
	OnButtonFree();
	
}

void CModifyWnd::OnMenuitemModifyReplace80() 
{
	// TODO: Add your command handler code here
	OnButton80();
}

void CModifyWnd::OnMenuitemModifyReplaceDran() 
{
	// TODO: Add your command handler code here
	OnButtonDoubleRandom();	
}

void CModifyWnd::OnMenuitemModifyReplaceFf() 
{
	// TODO: Add your command handler code here
	OnButtonFf();	
}

void CModifyWnd::OnMenuitemSaveRow() 
{
	SaveRow(m_grid.GetFocusCell().row,TRUE);
}


void CModifyWnd::OnCheckAllways() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

//	if(m_CheckModifyAlways){
//		GetDlgItem(IDC_CHECK_ALLWAYS)->SetWindowText("每次");
//	}else{
//		GetDlgItem(IDC_CHECK_ALLWAYS)->SetWindowText("一次");
//	}
//
}

void CModifyWnd::OnButtonSaveall() 
{
	int i; int j;
	char modified;

	for(i=0; i<RowInfoCnt; i++){

		//找到该行是否有修改
		modified=0;
		for(j=GRID_FIRST_DATA_COL; j<MAX_COL; j++){
			if(m_grid.GetItemBkColour(i+1,j)==DATA_BG_RED_COLOR){
				modified=1;
				break;
			}
		}

		//保存该行
		if(modified){
			SaveRow(i+1,FALSE);
		}
	}

	m_grid.RedrawWindow();

}

void CModifyWnd::OnButtonModifyAdd1() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	CCellRange range;
	int i,j;

	if(m_CheckModifyAlways){
		
		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						AddModifyAlways(j,i, MODIFY_TYPE_ADD_01H,0);
				}
			}
		}

	}else{

		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						if(HexString2Buffer(m_grid.GetItemText(j,i).GetBuffer(4),&b,1,&l)){
							if(l==1){
								b+=0x01;
								str.Format("%2x",b);
								str.Replace(' ','0');
								AddModifyReplace(j,i,str.GetBuffer(8));
							}
						}
				}
			}
		}

	}
	m_grid.SetFocus();
}

void CModifyWnd::OnButtonModifySub01() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	CCellRange range;
	int i,j;


	if(m_CheckModifyAlways){
		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						AddModifyAlways(j,i, MODIFY_TYPE_SUB_01H,0);
				}
			}
		}
	}else{

		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						if(HexString2Buffer(m_grid.GetItemText(j,i).GetBuffer(4),&b,1,&l)){
							if(l==1){
								b-=0x01;
								str.Format("%2x",b);
								str.Replace(' ','0');
								AddModifyReplace(j,i,str.GetBuffer(8));
							}
						}
				}
			}
		}


	}
	m_grid.SetFocus();

}

void CModifyWnd::OnButtonModifyAdd10() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	CCellRange range;
	int i,j;

	if(m_CheckModifyAlways){
		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						AddModifyAlways(j,i, MODIFY_TYPE_ADD_10H,0);
				}
			}
		}
	}else{

		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						if(HexString2Buffer(m_grid.GetItemText(j,i).GetBuffer(4),&b,1,&l)){
							if(l==1){
								b+=0x10;
								str.Format("%2x",b);
								str.Replace(' ','0');
								AddModifyReplace(j,i,str.GetBuffer(8));
							}
						}
				}
			}
		}

	}
	m_grid.SetFocus();
	
}


void CModifyWnd::OnButtonModifyMoveleft() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	CCellRange range;
	int i,j;
				
	if(m_CheckModifyAlways){
		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						AddModifyAlways(j,i, MODIFY_TYPE_MOVE_LEFT,0);
				}
			}
		}
	}else{
		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						if(HexString2Buffer(m_grid.GetItemText(j,i).GetBuffer(4),&b,1,&l)){
							if(l==1){
								b<<=0x01;
								str.Format("%2x",b);
								str.Replace(' ','0');
								AddModifyReplace(j,i,str.GetBuffer(8));
							}
						}
				}
			}
		}
	}
	m_grid.SetFocus();
}

void CModifyWnd::OnButtonModifyMoveright() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	CCellRange range;
	int i,j;

	if(m_CheckModifyAlways){
		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						AddModifyAlways(j,i, MODIFY_TYPE_MOVE_RIGHT,0);
				}
			}
		}
	}else{
		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						if(HexString2Buffer(m_grid.GetItemText(j,i).GetBuffer(4),&b,1,&l)){
							if(l==1){
								b>>=0x01;
								str.Format("%2x",b);
								str.Replace(' ','0');
								AddModifyReplace(j,i,str.GetBuffer(8));
							}
						}
				}
			}
		}
	}
	m_grid.SetFocus();
	
}

void CModifyWnd::OnButtonModifySub10() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	CCellRange range;
	int i,j;

	if(m_CheckModifyAlways){
		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						AddModifyAlways(j,i, MODIFY_TYPE_SUB_10H,0);
				}
			}
		}
	}else{

		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						if(HexString2Buffer(m_grid.GetItemText(j,i).GetBuffer(4),&b,1,&l)){
							if(l==1){
								b-=0x10;
								str.Format("%2x",b);
								str.Replace(' ','0');
								AddModifyReplace(j,i,str.GetBuffer(8));
							}
						}
				}
			}
		}


	}

	m_grid.SetFocus();
	
}

BOOL CModifyWnd::DestroyWindow() 
{
	GModifyWndMutex.Lock();
	PModifyWnd=0;

	//SuspendThread(PVirtualCarWnd->HThread);

	//Save window width/height to config file
	CDTXML cfg;
	if(cfg.Open(STR_APP_CONFIG_FILE)){
		RECT rc;
		GetWindowRect(&rc);

		cfg.Enter("TOP");
		cfg.Enter("TAOWND");

		WINDOWPLACEMENT p;
		GetWindowPlacement(&p);
		if(p.showCmd==SW_SHOWMAXIMIZED){
			cfg.SetInteger("MAXIMIZED",1);
		}else{
			cfg.SetInteger("MAXIMIZED",0);
			cfg.SetInteger("WIDTH",rc.right-rc.left);
			cfg.SetInteger("HEIGHT",rc.bottom-rc.top);
			cfg.SetInteger("X",rc.left);
			cfg.SetInteger("Y",rc.top);
		}
		cfg.Back();
		
		cfg.Save();
		cfg.Close();
	}


	return CDialog::DestroyWindow();
}

void CModifyWnd::OnMenuitemAdd01() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	if(HexString2Buffer(m_grid.GetItemText(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col).GetBuffer(4),&b,1,&l)){
		if(l==1){
			b+=0x01;
			str.Format("%2x",b);
			str.Replace(' ','0');
			AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,str.GetBuffer(4));
		}
	}

}

void CModifyWnd::OnMenuitemSub01() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	if(HexString2Buffer(m_grid.GetItemText(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col).GetBuffer(4),&b,1,&l)){
		if(l==1){
			b-=0x01;
			str.Format("%2x",b);
			str.Replace(' ','0');
			AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,str.GetBuffer(4));
		}
	}
	
}

void CModifyWnd::OnMenuitemAdd10() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	if(HexString2Buffer(m_grid.GetItemText(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col).GetBuffer(4),&b,1,&l)){
		if(l==1){
			b+=0x10;
			str.Format("%2x",b);
			str.Replace(' ','0');
			AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,str.GetBuffer(4));
		}
	}
	
	
}

void CModifyWnd::OnMenuitemSub10() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	if(HexString2Buffer(m_grid.GetItemText(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col).GetBuffer(4),&b,1,&l)){
		if(l==1){
			b-=0x10;
			str.Format("%2x",b);
			str.Replace(' ','0');
			AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,str.GetBuffer(4));
		}
	}
	
	
}

void CModifyWnd::OnMenuitemMoveleft() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	if(HexString2Buffer(m_grid.GetItemText(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col).GetBuffer(4),&b,1,&l)){
		if(l==1){
			b<<=1;
			str.Format("%2x",b);
			str.Replace(' ','0');
			AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,str.GetBuffer(4));
		}
	}
	
	
}

void CModifyWnd::OnMenuitemMoveright() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	if(HexString2Buffer(m_grid.GetItemText(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col).GetBuffer(4),&b,1,&l)){
		if(l==1){
			b>>=1;
			str.Format("%2x",b);
			str.Replace(' ','0');
			AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,str.GetBuffer(4));
		}
	}
	
	
}

void CModifyWnd::OnMenuitemAlwaysAdd01() 
{
	AddModifyAlways(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col, MODIFY_TYPE_ADD_01H,0);

}

void CModifyWnd::OnMenuitemAlwaysSub01() 
{
	AddModifyAlways(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col, MODIFY_TYPE_SUB_01H,0);
	
}

void CModifyWnd::OnMenuitemAlwaysAdd10() 
{
	AddModifyAlways(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col, MODIFY_TYPE_ADD_10H,0);
	
}

void CModifyWnd::OnMenuitemAlwaysSub10() 
{
	AddModifyAlways(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col, MODIFY_TYPE_SUB_10H,0);
	
}

void CModifyWnd::OnMenuitemAlwaysMoveleft() 
{
	AddModifyAlways(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col, MODIFY_TYPE_MOVE_LEFT,0);
	
}

void CModifyWnd::OnMenuitemAlwaysMoveright() 
{
	AddModifyAlways(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col, MODIFY_TYPE_MOVE_RIGHT,0);
	
}

void CModifyWnd::OnButtonModifyAddAny() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	CCellRange range;
	int i,j;

	CInputByteDlg dlg;
	BYTE custom;

	dlg.DoModal();
	HexString2Buffer(dlg.m_Edit.GetBuffer(4),&custom,1,&l);

	
	if(m_CheckModifyAlways){
		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						AddModifyAlways(j,i, MODIFY_TYPE_ADD_ANY,custom);
				}
			}
		}
	}else{


		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						if(HexString2Buffer(m_grid.GetItemText(j,i).GetBuffer(4),&b,1,&l)){
							if(l==1){
								b+=custom;
								str.Format("%2x",b);
								str.Replace(' ','0');
								AddModifyReplace(j,i,str.GetBuffer(8));
							}
						}
				}
			}
		}


	}
	
	m_grid.SetFocus();


}

void CModifyWnd::OnButtonModifySubAny() 
{
	unsigned char b;
	unsigned int l=0;
	CString str;

	CCellRange range;
	int i,j;

	CInputByteDlg dlg;
	BYTE custom;

	dlg.DoModal();
	HexString2Buffer(dlg.m_Edit.GetBuffer(4),&custom,1,&l);

	
	if(m_CheckModifyAlways){
		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						AddModifyAlways(j,i, MODIFY_TYPE_SUB_ANY,custom);
				}
			}
		}
	}else{


		if(m_grid.GetSelectedCount()>0){
			range=m_grid.GetSelectedCellRange();
			for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
				for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
					if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
						if(HexString2Buffer(m_grid.GetItemText(j,i).GetBuffer(4),&b,1,&l)){
							if(l==1){
								b-=custom;
								str.Format("%2x",b);
								str.Replace(' ','0');
								AddModifyReplace(j,i,str.GetBuffer(8));
							}
						}
				}
			}
		}


	}
	
	m_grid.SetFocus();


	
}

void CModifyWnd::OnMenuitemAlwaysAddany() 
{
	AddModifyAlways(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col, MODIFY_TYPE_ADD_ANY,0);
}

void CModifyWnd::OnMenuitemAlwaysSubany() 
{
	AddModifyAlways(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col, MODIFY_TYPE_SUB_ANY,0);
}

void CModifyWnd::OnButtonSaveRow() 
{
	SaveRow(m_grid.GetFocusCell().row,TRUE);	
}

void CModifyWnd::OnMenuitemModifyReplaceStar() 
{
	OnButtonStar();
}

void CModifyWnd::OnButtonStar() 
{
	CCellRange range;
	int i,j;

	if(m_grid.GetSelectedCount()>0){
		range=m_grid.GetSelectedCellRange();
		for(i=range.GetMinCol(); i<=range.GetMaxCol(); i++){
			for(j=range.GetMinRow(); j<=range.GetMaxRow(); j++){
				if(m_grid.IsCellSelected(j, i))	//判断表格是否选中
					if(RowInfo[j-1].TypeR0S1==0){ 
						AddModifyReplace(j,i,"* ");
					}
			}
		}
	}

	//AddModifyReplace(m_grid.GetFocusCell().row,m_grid.GetFocusCell().col,"01");
	m_grid.SetFocus();
	
}

void CModifyWnd::UpdateGridXNumber()
{
	int i;
	char buf[32];

	//设置字节位置标尺
	for(i=GRID_FIRST_DATA_COL; i<MAX_COL; i++){
		if(m_GridXHex0Dec1==0){
			sprintf(buf,"%02x",  (unsigned char)(m_GridXOffset+ (i-GRID_FIRST_DATA_COL)) );
		}else{
			sprintf(buf,"%d",  (unsigned char)(m_GridXOffset+ (i-GRID_FIRST_DATA_COL)) );
		}
		m_grid.SetItemText(0,i,buf);
	}

}

void CModifyWnd::OnCheckHex0dec1() 
{
	UpdateData(TRUE);
	
	if(m_GridXHex0Dec1==0){
		GetDlgItem(IDC_CHECK_HEX0DEC1)->SetWindowText("十六进");
	}else{
		GetDlgItem(IDC_CHECK_HEX0DEC1)->SetWindowText("十进制");
	}

	UpdateGridXNumber();
	m_grid.RedrawRow(0);
}

void CModifyWnd::OnKillfocusEditGridxOffset() 
{
	UpdateData(TRUE);
	UpdateGridXNumber();
	m_grid.RedrawRow(0);
	
}

void CModifyWnd::OnDestroy() 
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
// 	delete this ;
// 	this->GetParent()->SendMessage(WM_CLOSE_TAO) ;
//	GTaoDlgOpen= FALSE ;
}

