
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "framework.h"
#include "Resource.h"
#include "EdgeBrowserAppDlg.h"
#include "CSideControlWnd.h"
#include "CMainFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrameWnd, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrameWnd, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_MESSAGE(WM_MESSAGE_EXPAND, &CMainFrameWnd::OnUptadeInnerSize)
	ON_MESSAGE(WM_MESSAGE_CONNECT, &CMainFrameWnd::SetWebView)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrameWnd::CMainFrameWnd() noexcept
{
	// TODO: add member initialization code here
}

CMainFrameWnd::~CMainFrameWnd()
{
}

int CMainFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//create a view to occupy the client area of the frame
	if (!webViewWnd.Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, CRect(0, 0, 500, 1000), this, IDD_WEBVIEWWND, nullptr))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	//webViewWnd.ModifyStyle(;
	//webViewWnd.Create(IDD_EDGEBROWSERAPP_DIALOG, this);
	//webViewWnd.InitializeWebView();
	if (!controlWnd.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(500, 0, 1000, 1000), this, IDD_SIDECONTROLWND, nullptr))
	{
		TRACE0("Failed to create control window\n");
		return -1;
	}

	//webView.Start(_T("bing.com"));
	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	return 0;
}

BOOL CMainFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrameWnd::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrameWnd::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrameWnd::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	//webViewWnd.SetFocus();
}

BOOL CMainFrameWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (webViewWnd.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	this->SendMessage(WM_APP + 1);
	// TODO: добавьте свой код обработчика сообщений
}

LRESULT CMainFrameWnd::OnUptadeInnerSize(WPARAM wParam, LPARAM lParam)
{
	if (webViewWnd.GetSafeHwnd() && controlWnd.GetSafeHwnd())
	{
		// Получаем текущую ширину второго окна
		int controlWidth = controlWnd.getWidth();
		CRect rcClient;
		GetClientRect(&rcClient);
		// Перемещаем и изменяем размеры окон
		webViewWnd.MoveWindow(0, 0, rcClient.Width() - controlWidth, rcClient.Height());
		controlWnd.MoveWindow(rcClient.Width() - controlWidth, 0, controlWidth, rcClient.Height());
	}
	return LRESULT();
}

LRESULT CMainFrameWnd::SetWebView(WPARAM wParam, LPARAM lParam)
{
	webViewWnd.InitializeWebView(reinterpret_cast<LPCWSTR>(lParam));
	return LRESULT();
}

BOOL CMainFrameWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);  // Получаем размеры клиентской области
	pDC->FillSolidRect(&rect, RGB(250, 250, 250));
	return CWnd::OnEraseBkgnd(pDC);
}
