// WinDinkeditDoc.cpp : implementation of the CWinDinkeditDoc class
//

#include "stdafx.h"
#include "WinDinkedit.h"

#include "WinDinkeditDoc.h"
#include "DmodPropertiesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Globals.h"
#include "Map.h"

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditDoc

IMPLEMENT_DYNCREATE(CWinDinkeditDoc, CDocument)

BEGIN_MESSAGE_MAP(CWinDinkeditDoc, CDocument)
	//{{AFX_MSG_MAP(CWinDinkeditDoc)
	ON_COMMAND(ID_DMOD_PROPERTIES, OnDmodProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditDoc construction/destruction

CWinDinkeditDoc::CWinDinkeditDoc()
{
	// TODO: add one-time construction code here
	SetModifiedFlag(false);
}

CWinDinkeditDoc::~CWinDinkeditDoc()
{
}

BOOL CWinDinkeditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditDoc diagnostics

#ifdef _DEBUG
void CWinDinkeditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWinDinkeditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditDoc commands

BOOL CWinDinkeditDoc::OnOpenDocument(LPCTSTR dmod_name) 
{	
	// check if a map exists already and kill it if one does
	if (current_map)
	{
		delete current_map;
		current_map = NULL;
	}

	// dmod being opened has not been modified yet
	SetModifiedFlag(FALSE);

	CString dmod_path = main_dink_path + Dmod_Name;

	// pass filename to dink game
	current_map = new Map(dmod_path.GetBuffer(0));
	current_map->finishLoading();
	
	return TRUE;
}

/*BOOL CWinDinkeditDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if (current_map)
	{
		BeginWaitCursor();
		current_map->save_dmod();
		EndWaitCursor();

		SetModifiedFlag(FALSE);
	}
	
	return TRUE;
}*/

void CWinDinkeditDoc::OnCloseDocument() 
{
	if (current_map)
	{
		delete current_map;
		current_map = NULL;
	}
	
	CDocument::OnCloseDocument();
}

BOOL CWinDinkeditDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	// now close the dmod
	if (current_map && IsModified())
	{
		int result = MessageBox(NULL, "Save changes to dmod?", "Save Dmod?", MB_YESNOCANCEL | MB_ICONWARNING);

		if (result == IDYES)
		{
			current_map->save_dmod();
		}
		else if (result == IDNO)
		{
		}
		else if (result == IDCANCEL)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CWinDinkeditDoc::OnDmodProperties() 
{
	DmodPropertiesDialog newdlg(NULL);

	newdlg.DoModal();
}

BOOL CWinDinkeditDoc::SaveModified() 
{
	return CDocument::SaveModified();
}
