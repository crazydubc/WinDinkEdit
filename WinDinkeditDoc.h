// WinDinkeditDoc.h : interface of the CWinDinkeditDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef WINDINKEDITDOC_H
#define WINDINKEDITDOC_H

class CWinDinkeditDoc : public CDocument
{
protected: // create from serialization only
	CWinDinkeditDoc();
	DECLARE_DYNCREATE(CWinDinkeditDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinDinkeditDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWinDinkeditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWinDinkeditDoc)
	afx_msg void OnDmodProperties();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
