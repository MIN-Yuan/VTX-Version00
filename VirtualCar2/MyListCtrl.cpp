// MyListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "MyListCtrl.h"
#include "DTHsb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl

CMyListCtrl::CMyListCtrl()
{
}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrl)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl message handlers
void CMyListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = CDRF_DODEFAULT;
	NMLVCUSTOMDRAW * lplvdr=(NMLVCUSTOMDRAW*)pNMHDR;
	NMCUSTOMDRAW &nmcd = lplvdr->nmcd;
	
	double h,s,b;
	
	switch(lplvdr->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:		
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
		{
// #define ID_IMG_NOTE		0
// #define ID_IMG_CAR		1
// #define ID_IMG_DIAGNOTOR 2
// #define ID_IMG_ERR		3
// #define ID_IMG_EMPTY		4

			switch(nmcd.lItemlParam){
			case 0:
				h=180;
				break;
			case 1:
				h=217;
				break;
			case 2:
				h=60;
				break;
			case 3:
				h=0;
				break;
			case 4:
				h=120;
				break;
			}

			if(nmcd.dwItemSpec%20==0){
				s=25;
				b=75;				
			}else{
				s=5+nmcd.dwItemSpec%10;
				b=95-nmcd.dwItemSpec%10;
			}
			


			lplvdr->clrTextBk=HSB2RGB(h,s,b);
			*pResult = CDRF_DODEFAULT;
		}
		break;
	}
	
}
