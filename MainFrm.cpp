// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "WinDinkedit.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "WinDinkeditView.h"
#include "Engine.h"
#include "interface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Globals.h"
#include "Engine.h"
#include "Map.h"

CMainFrame* mainWnd;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_DMOD_PROPERTIES, OnUpdateDmodProperties)
	ON_UPDATE_COMMAND_UI(ID_SAVE_FIX, OnUpdateFileSave)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MOUSE, OnUpdateMousePos)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_VISION, OnUpdateVision)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCR_MATCH, OnUpdateScreenMatch)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SPR_HARD, OnUpdateSpriteHard)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
	ON_COMMAND(ID_FILE_PLAYGAME, OnFilePlaygame)
	ON_COMMAND(ID_SAVE_FIX, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_PLAYGAME, OnUpdateFilePlaygame)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_MOUSE,
	ID_INDICATOR_VISION,
	ID_INDICATOR_SCR_MATCH,
	ID_INDICATOR_SPR_HARD,
//	ID_SEPARATOR,

//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	mainWnd = this;
//	Game_Init();	// load up directdraw
}

CMainFrame::~CMainFrame()
{
//	Game_Shutdown();
}

int CMainFrame::setStatusText(char *text)
{
	m_wndStatusBar.OnProgress(0);
	m_wndStatusBar.SetPaneText(0, text);

	return true;
}

int CMainFrame::updateMousePos(int screen, int x, int y)
{
	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_MOUSE);

	char buffer[50];

	wsprintf(buffer, "screen: %.3d, x: %.3d, y: %.3d", screen, x, y);

//	m_wndStatusBar.SetPaneStyle(nIndex, SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneText(nIndex, buffer);

	return true;
}

int CMainFrame::updateVision(int vision)
{
	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_VISION);

	char buffer[20];

	wsprintf(buffer, "vision: %.3d", vision);

	m_wndStatusBar.SetPaneText(nIndex, buffer);

	return true;
}

int CMainFrame::updateScreenMatch(bool enabled)
{
	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_SCR_MATCH);

	if (enabled)
	{
		m_wndStatusBar.SetPaneText(nIndex, "SCR_MATCH");
	//	m_wndStatusBar.SetPaneStyle(nIndex, SBPS_NORMAL);
	}
	else
	{
		m_wndStatusBar.SetPaneText(nIndex, "");
	//	m_wndStatusBar.SetPaneStyle(nIndex, SBPS_DISABLED);
	}

	return true;
}

int CMainFrame::updateSpriteHard(bool enabled)
{
	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_SPR_HARD);

	if (enabled)
	{
		m_wndStatusBar.SetPaneText(nIndex, "SPR_HARD");
	//	m_wndStatusBar.SetPaneStyle(nIndex, SBPS_NORMAL);
	}
	else
	{
		m_wndStatusBar.SetPaneText(nIndex, "");
	//	m_wndStatusBar.SetPaneStyle(nIndex, SBPS_DISABLED);
	}

	return true;
}

void CMainFrame::OnUpdateMousePos(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateVision(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateScreenMatch(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CMainFrame::OnUpdateSpriteHard(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	//kill timer, we will reset it later if needed
	KillTimer(nIDEvent);

	//what timer got called?
	switch (nIDEvent)
	{
	case TIMER_AUTO_SAVE:
		{
			if (auto_save_time)
			{
				//autosave!
				current_map->save_dmod();
				mainWnd->setStatusText("Dmod Auto-saved..");

				SetTimer(TIMER_AUTO_SAVE, auto_save_time * 60000, NULL);
			}
		} break;
	case TIMER_MOUSE_CHECK:
		{
			//is the mouse still down?
			if (KEY_DOWN(VK_LBUTTON) || KEY_DOWN(VK_MBUTTON))
			{
				SetTimer(TIMER_MOUSE_CHECK, 100, NULL);
			} else {
				mouseLeftReleased(current_map->x_hover, current_map->y_hover);
			}
		} break;
	default:break; //what timer is that?! Oh screw it, we killed it.
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
/*	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
*/

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	updateScreenMatch(false);
	updateSpriteHard(false);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(100, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CWinDinkeditView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

CWinDinkeditView* CMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CWinDinkeditView* pView = DYNAMIC_DOWNCAST(CWinDinkeditView, pWnd);
	return pView;
}

CLeftView* CMainFrame::GetLeftPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
	CLeftView* pView = DYNAMIC_DOWNCAST(CLeftView, pWnd);
	return pView;
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.

	CWinDinkeditView* pView = GetRightPane(); 

	// if the right-hand pane hasn't been created or isn't a view,
	// disable commands in our range

	if (pView == NULL)
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.
	CWinDinkeditView* pView = GetRightPane();
}

void CMainFrame::OnMove(int x, int y) 
{
	CFrameWnd::OnMove(x, y);
	
	// TODO: Add your message handler code here
	if (current_map)
		GetRightPane()->windowMoved(x, y);
}


void CMainFrame::OnUpdateDmodProperties(CCmdUI* pCmdUI) 
{
	if (current_map)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	if (current_map)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnFileSave()
{
	if (current_map)
	{
		BeginWaitCursor();
		current_map->save_dmod();
		EndWaitCursor();

		mainWnd->GetActiveDocument()->SetModifiedFlag(FALSE);
	}
}


#include "map.h"

void CMainFrame::OnFilePlaygame()
{
	int result = MessageBox("Warning, this will save you d-mod, and run dink, continue?", "WARNING: Save Confirmation", MB_YESNO);

	if (result == IDNO)
		return;

	current_map->save_dmod();
	mainWnd->GetActiveDocument()->SetModifiedFlag(FALSE);

	char openline[256];
	char param[10];
	wsprintf(openline, "%sdink.exe", main_dink_path.GetBuffer(0));
	wsprintf(param, "-game %s", Dmod_Name.GetBuffer(0));

	HINSTANCE NEW = ShellExecute(NULL, "open", openline, param, NULL, SW_SHOWDEFAULT);

	if (int(NEW) < 32)
	{
		char error[256];
		wsprintf(error, "Error executing %s Error Code: %i", openline, int(NEW));
		MessageBox(error, "ERROR: Running D-mod", MB_OK);
	}
}

void CMainFrame::OnUpdateFilePlaygame(CCmdUI *pCmdUI)
{
	if (current_map)
		pCmdUI->Enable();
	else
		pCmdUI->Enable(FALSE);
}
