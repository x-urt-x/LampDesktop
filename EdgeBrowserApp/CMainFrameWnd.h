#pragma once
#include <afxwin.h>
#include "EdgeBrowserAppDlg.h"
#include "CSideControlWnd.h"
class CMainFrameWnd :
    public CFrameWnd
{

public:
	CMainFrameWnd() noexcept;

protected:
	DECLARE_DYNAMIC(CMainFrameWnd)

		// Attributes
public:
	// Operations
public:

	// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	// Implementation
public:
	virtual ~CMainFrameWnd();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CEdgeBrowserAppDlg webViewWnd;
	CSideControlWnd controlWnd;

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnUptadeInnerSize(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT SetWebView(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};

