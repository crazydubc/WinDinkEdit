// WinDinkedit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WinDinkedit.h"

#include "MainFrm.h"
#include "WinDinkeditDoc.h"
#include "LeftView.h"
#include "Globals.h"
#include "Map.h"
#include "OpenDmod.h"
#include "Common.h"
#include "NewDmodDialog.h"
#include "GetFolderDlg.h"

CWinDinkeditApp* dinkedit_app;

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditApp

BEGIN_MESSAGE_MAP(CWinDinkeditApp, CWinApp)
	//{{AFX_MSG_MAP(CWinDinkeditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_NEW_DMOD, OnNewDmod)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinDinkeditApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinDinkeditApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditApp construction

CWinDinkeditApp::CWinDinkeditApp()
{
	
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWinDinkeditApp object

CWinDinkeditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditApp initialization

BOOL CWinDinkeditApp::InitInstance()
{
	AfxEnableControlContainer();

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	char app_path[256];

	// get the path to the WDE directory
	if (GetCurrentDirectory(MAX_PATH, app_path) == NULL)
	{
		return false;
	}

	WDE_path = app_path;
	WDE_path += "\\";
	
	// get the dink directory path

	// first check in the dinksmallwood.ini file
	if (readDinkIni(app_path) == false)
	{
		for (int i = strlen(app_path) - 2; i > 0; i--)
		{
			if (app_path[i] == '\\')
			{
				break;
			}
			app_path[i] = 0;
		}
	}

	dink_path = main_dink_path = app_path;
	dink_path += "dink\\";


	// check to make sure we have the right directory
	FILE* stream;

	char buffer[MAX_PATH];

	wsprintf(buffer, "%sdink.exe", app_path);

	while ((stream = fopen(buffer, "rb")) == NULL)
	{
		MessageBox(NULL, "The Dink.exe program was unable to be located. Please locate the file.", NULL,NULL);
		// failed to open dink.exe, path is wrong
		CGetFolderDlg newdlg;
		int retcode = newdlg.DoModal();

		if (retcode == IDCANCEL)
		{
			MessageBox(NULL, "Please run the dink game to restore the path.", "Error", MB_OK);
			exit(1);
			break;
		} else {
			strcpy(buffer, newdlg.m_strFolderPath);
			
			writeDinkIni(buffer);

			if (buffer[strlen(buffer) - 1] != '\\')
				sprintf(buffer, "%s\\", buffer);

			strcpy(app_path, buffer);

			dink_path = main_dink_path = buffer;
			dink_path += "dink\\";

			wsprintf(buffer, "%sdink.exe", app_path);
		}
		
	}

	fclose(stream);


	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("WinDinkedit"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU (ie most recently used files))

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWinDinkeditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLeftView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	bool open_file = false;
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		open_file = true;
		strcpy(buffer, cmdInfo.m_strFileName); 
	}

	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	
	if (open_file)
	{
		OpenDocumentFile(buffer);
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileRund();
	afx_msg void OnFileRundinkedit();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// create the about box
void CWinDinkeditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditApp message handlers

void CWinDinkeditApp::OnFileOpen()
{
	char dmod_name[256];

	// create the open dmod dialog box
	OpenDmod newdlg(NULL);

	// give the dialog a pointer to the dmod_name buffer
	newdlg.setDmodNamePtr(dmod_name);

	int retcode = newdlg.DoModal();

	if (retcode == IDOK)
	{
		OpenDocumentFile(dmod_name);
	}
}

CDocument* CWinDinkeditApp::OpenDocumentFile(LPCTSTR file_name) 
{
	char buffer[MAX_PATH];
	strcpy(buffer, file_name);
	
	Dmod_Name = getDmodName(buffer);

	return CWinApp::OpenDocumentFile(Dmod_Name.GetBuffer(0));
}


void CWinDinkeditApp::OnNewDmod() 
{
	char dmod_name[256];
	
	NewDmodDialog newdlg(NULL);

	// give the dialog a pointer to the dmod_name buffer
	newdlg.setDmodNamePtr(dmod_name);

	int retcode = newdlg.DoModal();

	if (retcode == IDOK)
	{
		OpenDocumentFile(dmod_name);
	}
}
