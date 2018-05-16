// DlgSend.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "DlgSend.h"
#include "VirtualCarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSend dialog


CDlgSend::CDlgSend(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSend::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSend)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSend)
	DDX_Control(pDX, IDC_LIST_SEND, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSend, CDialog)
	//{{AFX_MSG_MAP(CDlgSend)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SEND, OnClickListSend)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SEND, OnDblclkListSend)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSend message handlers

void CDlgSend::OnClickListSend(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW * pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
		
		str_result=m_List.GetItemText(pNMListView->iItem, 1);
	}else{
		str_result.Empty();
	}
	
	*pResult = 0;
}

void MakeSpaceDividedData(CString & data, CString & out)
{
	CString data1;
	data1=data;

	data1.Replace("*", "* ");
	
	int len1=data1.GetLength();

	if(len1<1){
		return;
	}

	char * buf1=data1.GetBuffer(0);
	char * buf2=out.GetBufferSetLength(data.GetLength()*2);
	int pos2=0;

	int i;

	for(i=0; i<len1; i++){
		buf2[pos2]=buf1[i];
		pos2++;

		if(i%2){
			buf2[pos2]=' ';
			buf2++;
		}
	}
	
	buf2[pos2]=0;
	out.ReleaseBuffer();
}

BOOL CDlgSend::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(PVirtualCarWnd==0){
		return TRUE;
	}

	m_List.SetImageList(&PVirtualCarWnd->m_images,LVSIL_SMALL);

	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_List.InsertColumn(0,"ID",LVCFMT_LEFT,50,0);
	m_List.InsertColumn(1,"Data",LVCFMT_LEFT,350,0);
	m_List.InsertColumn(2,"Remark",LVCFMT_LEFT,280,0);


	int i;
	int j;
	CString str;
	int id;

	for(i=0; i<CommCount; i++)
	{
		str.Format("(%d)",i);
		id=m_List.GetItemCount();
		m_List.InsertItem(id,str,ID_IMG_DIAGNOTOR);
		MakeSpaceDividedData(Comm[i].Receive,str);
		m_List.SetItemText(id,1,str);
		m_List.SetItemText(id,2,Comm[i].Remark);
		m_List.SetItemData(id,2);

		for(j=0; j<Comm[i].SCount; j++){
			id=m_List.GetItemCount();
			m_List.InsertItem(id,"",ID_IMG_CAR);
			MakeSpaceDividedData(Comm[i].Send[j].Data,str);
			m_List.SetItemText(id,1,str);
			m_List.SetItemText(id,2,Comm[i].Send[j].Remark);
			m_List.SetItemData(id,1);
		}

	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSend::OnDblclkListSend(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW * pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
		
		str_result=m_List.GetItemText(pNMListView->iItem, 1);
		EndDialog(0);
	}else{
		str_result.Empty();
		EndDialog(0);
	}
	
	*pResult = 0;
}

void CDlgSend::OnButtonSend() 
{
	// TODO: Add your control notification handler code here
	EndDialog(0);	
}
