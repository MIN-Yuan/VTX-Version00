// DlgWizardComm.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "DlgWizardComm.h"
#include "VirtualCarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardComm dialog


CDlgWizardComm::CDlgWizardComm(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizardComm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWizardComm)
	m_Data = _T("");
	//}}AFX_DATA_INIT
}


void CDlgWizardComm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWizardComm)
	DDX_Text(pDX, IDC_EDIT_DATA, m_Data);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWizardComm, CDialog)
	//{{AFX_MSG_MAP(CDlgWizardComm)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardComm message handlers



void InsertR(CDTXML &xml, CString str, INT commid)
{
	if(!xml.Enter(STR_COMM,commid)){
		xml.AddChild(STR_COMM,commid);
		xml.Enter(STR_COMM,commid);
	}

	TrimToPureHexString(str);

	if(commid==0){
		xml.SetInteger(STR_ENABLE,1);
		xml.SetInteger(STR_SHOW,1);
	}

	xml.SetString(STR_RECEIVE,str.GetBuffer(1024));

	if(commid==0){
		xml.SetString(STR_REMARK,"接收注释文字");
		xml.SetInteger(STR_TYPE,0);
		xml.SetInteger(STR_COUNT,1);
		xml.SetInteger(STR_POS,0);
		xml.SetString(STR_ACT,"");
		xml.SetString("Y0","");
		xml.SetString("Y1","");
		xml.SetString("Y2","");
		xml.SetString("Y3","");
	}
	
	xml.Back();
}

void InsertS(CDTXML &xml, CString str)
{
	int COMMID;
	int fSID;

	COMMID=xml.GetChildCount(STR_COMM)-1;
	
	if(COMMID<0){
		return;
	}

	if(!xml.Enter(STR_COMM,COMMID)){
		return;
	}

	TrimToPureHexString(str);

	fSID=xml.GetChildCount(STR_SEND);
	if(!xml.AddChild(STR_SEND,fSID)){
		return;
	}
	if(!xml.Enter(STR_SEND,fSID)){
		return;
	}

	xml.SetString(STR_DATA,str.GetBuffer(1024));

	if(COMMID==0){
		xml.SetString(STR_REMARK, "发送注释文字");
		xml.SetString(STR_ACT,"");
		xml.SetInteger(STR_SYMBOL,0);
	}

	xml.Back(2);

	xml.Save();
}


void CDlgWizardComm::OnButtonNext() 
{
	CDTXML xml;
	int i,len;
	int b,e;
	CString strtemp;
	CString strline;
	BOOL	R0S1=1;

	UpdateData(TRUE);

	if(!xml.Open(GWizardXMLFileName)){
		MessageBox("打开文件失败");
	}

	if(!xml.Enter(STR_CONTENT)){
		xml.AddChild(STR_CONTENT);
		xml.Enter(STR_CONTENT);
	}

	if(!xml.Enter(STR_COMMS)){
		xml.AddChild(STR_COMMS);
		xml.Enter(STR_COMMS);
	}

	INT fcommid;
	fcommid=xml.GetChildCount(STR_COMM);

	strtemp=m_Data;


	strtemp.Replace("RECE","R");
	strtemp.Replace("SEND","S");


	//统一格式化成\n
	strtemp.Replace("\n\r","\n");
	strtemp.Replace("\r\n","\n");
	strtemp.Replace("\r","\n");
	strtemp+="\n";	//最后加一个\n结束


	//去除INIT行
	b=strtemp.Find("INIT");
	if(b>=0){
		e=strtemp.Find('\n',b);
		if(e>=0){
			if(e>b){
				strtemp.Delete(b,e-b);
			}
		}
	}


	//去除SENDTIMES行
	b=strtemp.Find("SENDTIMES");
	if(b>=0){
		e=strtemp.Find('\n',b);
		if(e>=0){
			if(e>b){
				strtemp.Delete(b,e-b);
			}
		}
	}

	//去除STOP行
	b=strtemp.Find("STOP");
	if(b>=0){
		e=strtemp.Find('\n',b);
		if(e>=0){
			if(e>b){
				strtemp.Delete(b,e-b);
			}
		}
	}


	//去除KEEP行
	b=strtemp.Find("KEEP");
	if(b>=0){
		e=strtemp.Find('\n',b);
		if(e>=0){
			if(e>b){
				strtemp.Delete(b,e-b);
			}
		}
	}

	//去除MODE行
	b=strtemp.Find("MODE");
	if(b>=0){
		e=strtemp.Find('\n',b);
		if(e>=0){
			if(e>b){
				strtemp.Delete(b,e-b);
			}
		}
	}

	b=0; e=0;
	len=strtemp.GetLength();
	for(i=0; i<len; i++){

		//找出一行
		if(strtemp.GetAt(i)=='\n'){
			b=e;
			e=i+1;

			if(e-b>=2){
				strline=strtemp.Mid(b,e-b);
				
				if(strline.Find("R")>=0 || strline.Find("r")>=0){
					R0S1=0;
				}else if(strline.Find("S")>=0 || strline.Find("s")>=0 ){
					R0S1=1;
				}else{
					R0S1=!R0S1;
				}

				if(R0S1==0){
					InsertR(xml,strline,fcommid);
					fcommid++;
				}else{
					InsertS(xml,strline);
				}


			}
		}
	}

	xml.Save();
	GWizardDlgResult=ID_WIZARD_NEXT;
	
	OnCancel();
}

void CDlgWizardComm::OnOK()
{
	//OnOK();
}


BOOL CDlgWizardComm::PreTranslateMessage(MSG* pMsg) 
{
    if(pMsg->message == WM_KEYDOWN){
        if(pMsg->wParam == VK_ESCAPE){
			return TRUE;
        }

		//if(pMsg->wParam == VK_RETURN){
		//	return TRUE;
		//}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
