// VirtualCar.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "VirtualCarWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVirtualCarApp

BEGIN_MESSAGE_MAP(CVirtualCarApp, CWinApp)
	//{{AFX_MSG_MAP(CVirtualCarApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirtualCarApp construction

CVirtualCarApp::CVirtualCarApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVirtualCarApp object

CVirtualCarApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVirtualCarApp initialization

BOOL CVirtualCarApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	CVirtualCarWnd dlg;
	//m_hThread
//	return THREAD_PRIORITY_TIME_CRITICAL;
	SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL) ;

	m_pMainWnd = &dlg;
	
	PVirtualCarWnd=&dlg;

	int nResponse = dlg.DoModal();

	PVirtualCarWnd=0;

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
