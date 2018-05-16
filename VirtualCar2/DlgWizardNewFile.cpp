// DlgWizardNewFile.cpp : implementation file
//

#include "stdafx.h"
#include "VirtualCar.h"
#include "DlgWizardNewFile.h"
#include "VirtualCarWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardNewFile dialog


const char * STR_DEFAULT_NEWCFGFILE_PATH="c:\\EcuEmu.xml";

CDlgWizardNewFile::CDlgWizardNewFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWizardNewFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWizardNewFile)
	m_FilePath = _T("");
	//}}AFX_DATA_INIT

	GWizardDlgResult =ID_WIZARD_CANCEL;
}


void CDlgWizardNewFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWizardNewFile)
	DDX_Text(pDX, IDC_EDIT_FILE, m_FilePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWizardNewFile, CDialog)
	//{{AFX_MSG_MAP(CDlgWizardNewFile)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_EN_CHANGE(IDC_EDIT_FILE, OnChangeEditFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWizardNewFile message handlers

BOOL CDlgWizardNewFile::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CDTXML xml;
	
	if(IsFileExist(GConfigFile)){
		m_FilePath=GConfigFile;
	}else{
		m_FilePath=STR_DEFAULT_NEWCFGFILE_PATH;
	}
	
	while(IsFileExist(m_FilePath)){
		m_FilePath=m_FilePath.Left(m_FilePath.GetLength()-4);
		m_FilePath+="X";
		m_FilePath+=".xml";
	}
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWizardNewFile::OnButtonBrowse() 
{
	CString strPath;

	CFileDialog dlg(1,NULL,NULL,NULL,NULL,NULL);


	if(IsFileExist(GConfigFile)){
		strPath=GConfigFile;
	}else{
		::GetModuleFileName(NULL,strPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
		strPath.ReleaseBuffer();
	}
	int nPos=strPath.ReverseFind('\\');
	strPath=strPath.Left(nPos);
	//CString strFileName = strPath.Right(strPath.GetLenth() - nPos -1);



	dlg.m_ofn.lpstrInitialDir=strPath;
	dlg.m_ofn.lpstrFilter="�������������ļ�(*.xml)\0*.xml\0";
	dlg.DoModal();
	

	if(strlen(dlg.m_ofn.lpstrFile)<2){
		return;
	}

	m_FilePath=dlg.m_ofn.lpstrFile;


	UpdateData(FALSE);
}

void CDlgWizardNewFile::OnButtonNext() 
{
	CString msg;
	CString str;
	DWORD written;

	if(m_FilePath.Find(STR_APP_CONFIG_FILE)>0){
		msg=STR_APP_CONFIG_FILE;
		msg+="��Ӧ�ó���������ļ������ܸ�������ļ���";
		MessageBox(msg);
		return;
	}

	if(IsFileExist(m_FilePath)){
		msg="�ļ�";
		msg+=m_FilePath;
		msg+="�Ѿ����ڣ����븲����";
		if(MessageBox(msg,NULL,MB_YESNO)==IDNO){
			return;
		}

	}

	//�½����ļ�
	HANDLE h=CreateFile(m_FilePath, GENERIC_READ|GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(h==INVALID_HANDLE_VALUE){
		return;
	}
	
	//����XML�ļ�ͷ
	str=STR_XML_FILE_INFO;
	WriteFile(h,(LPCVOID)str.GetBuffer(255),str.GetLength(),&written,NULL);
	CloseHandle(h);

	GWizardXMLFileName=m_FilePath;
	GWizardDlgResult=ID_WIZARD_NEXT;

	OnOK();
}

void CDlgWizardNewFile::OnChangeEditFile() 
{
	UpdateData(TRUE);

}

BOOL CDlgWizardNewFile::PreTranslateMessage(MSG* pMsg) 
{
    if(pMsg->message == WM_KEYDOWN){
        if(pMsg->wParam == VK_ESCAPE){
			return TRUE;
        }

		if(pMsg->wParam == VK_RETURN){
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
