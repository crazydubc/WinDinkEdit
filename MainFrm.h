// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef MAINFRM_H
#define MAINFRM_H

#include "StatusBar.h"

class CWinDinkeditView;
class CLeftView;

class CMainFrame : public CFrameWnd
{

public:
	int updateMousePos(int screen, int x, int y);
	int updateVision(int vision);
	int updateScreenMatch(bool enabled);
	int updateSpriteHard(bool enabled);
	int setStatusText(char *text);
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
public:

// Operations
public:
	afx_msg void OnUpdateMousePos(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVision(CCmdUI* pCmdUI);
	afx_msg void OnUpdateScreenMatch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpriteHard(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIdEvent);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
	CWinDinkeditView* GetRightPane();
	CLeftView* GetLeftPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CProgStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	CProgressCtrl m_wndProgress;
	CDialogBar      m_wndDlgBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnUpdateDmodProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileSave();
	afx_msg void OnFilePlaygame();
	afx_msg void OnUpdateFilePlaygame(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

extern CMainFrame* mainWnd;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
