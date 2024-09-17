#pragma once

#include <afxpriv.h>

//////////////////
// Status bar with progress control.
//
class CProgStatusBar : public CStatusBar {
public:
	CProgStatusBar();
	virtual ~CProgStatusBar();
	CProgressCtrl& GetProgressCtrl() {
		return m_wndProgBar;
	}
	void OnProgress(UINT pct);
	void SetRange(int min, int max);

protected:
	CProgressCtrl m_wndProgBar;			 // the progress bar
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CProgStatusBar)
};

