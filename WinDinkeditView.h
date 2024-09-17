// WinDinkeditView.h : interface of the CWinDinkeditView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef WINDINKEDITVIEW_H
#define WINDINKEDITVIEW_H

class Sprite;
class CWinDinkeditDoc;

class CWinDinkeditView : public CListView
{
public:
	void displaySpriteMenu(Sprite* sprite, int screen_num, int sprite_num, int x, int y);
	void displayScreenMenu(int screen, int x, int y);
	void gainFocus();

protected: // create from serialization only
	CWinDinkeditView();
	DECLARE_DYNCREATE(CWinDinkeditView)

// Attributes
public:
	CWinDinkeditDoc* GetDocument();
	int windowMoved(int x, int y);

// Operations
public:
	afx_msg void OnAutoUpdateMinimap(CCmdUI* pCmdUI);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinDinkeditView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWinDinkeditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
public:
	//{{AFX_MSG(CWinDinkeditView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnScreenProperties();
	afx_msg void OnDeleteScreen();
	afx_msg void OnSpriteProperties();
	afx_msg void OnVision();
	afx_msg void OnNewScreen();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewMinimap();
	afx_msg void OnViewSpriteMode();
	afx_msg void OnViewTileMode();
	afx_msg void OnViewTileset1();
	afx_msg void OnViewTileset10();
	afx_msg void OnViewTileset11();
	afx_msg void OnViewTileset3();
	afx_msg void OnViewTileset4();
	afx_msg void OnViewTileset5();
	afx_msg void OnViewTileset6();
	afx_msg void OnViewTileset7();
	afx_msg void OnViewTileset8();
	afx_msg void OnViewTileset9();
	afx_msg void OnViewTileset2();
	afx_msg void OnScreenMatch();
	afx_msg void OnPaint();
	afx_msg void OnSpriteHardness();
	afx_msg void OnViewHardboxMode();
	afx_msg void OnSetWarpBegin();
	afx_msg void OnSetWarpEnd();
	afx_msg void OnImportScreen();
	afx_msg void OnAutoUpdateMinimap();
	afx_msg void OnScreenShot();
	afx_msg void OnStoreSprite();
	afx_msg void OnFfcreate();
	afx_msg void OnCopyScreen();
	afx_msg void OnPasteScreen();
	afx_msg void OnCompressScripts();
	afx_msg void OnOptions();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnRevertHardTile();
	afx_msg void OnTransformHardnessNormal();
	afx_msg void OnTransformHardnessLow();
	afx_msg void OnTransformHardnessUnknown();
	afx_msg void OnSetDefaultHardTile();
	afx_msg void OnEditHardTile();
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WinDinkeditView.cpp
inline CWinDinkeditDoc* CWinDinkeditView::GetDocument()
   { return (CWinDinkeditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
