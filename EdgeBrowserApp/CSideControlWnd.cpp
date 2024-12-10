#include "framework.h"
#include "CSideControlWnd.h"
#include "Resource.h"

BEGIN_MESSAGE_MAP(CSideControlWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDD_SIDECONTROL_EXPAND_BTN, &CSideControlWnd::toggleExpand)
	ON_BN_CLICKED(IDD_SIDECONTROL_WEBVIEW_BTN, &CSideControlWnd::webViewConect)
END_MESSAGE_MAP()


int CSideControlWnd::getWidth() const
{
	return _isExpanded ? _expandedWidth : _collapsedWidth;
}

void CSideControlWnd::expand()
{
	ExpandBtn.SetWindowText(_T("→"));
	_isExpanded = true;
}

void CSideControlWnd::collapse()
{
	ExpandBtn.SetWindowText(_T("←"));
	_isExpanded = false;
}

void CSideControlWnd::toggleExpand()
{
	if (_isExpanded)
	{
		collapse();
	}
	else
	{
		expand();
	}
	GetParent()->SendMessage(WM_MESSAGE_EXPAND);
}

void CSideControlWnd::webViewConect()
{
	NC_ADDRESS adr_str;
	NET_ADDRESS_INFO_ ard_info;
	adr_str.pAddrInfo = &ard_info;
	if (FAILED(urlField.GetAddress(&adr_str)))
	{
		urlField.DisplayErrorTip();
		return;
	}
	urlField.GetWindowTextW(_url);
	_url = L"http://" + _url;
	GetParent()->SendMessage(WM_MESSAGE_CONNECT, 0, reinterpret_cast<LPARAM>(_url.GetString()));
}

int CSideControlWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	WebViewBtn.Create(L"Start Web", WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_TEXT,
		CRect(PS_PP(25, 30, 90, 50)), this, IDD_SIDECONTROL_WEBVIEW_BTN);	
	UDPConectBtn.Create(L"Start UDP", WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_TEXT,
		CRect(PS_PP(135, 30, 90, 50)), this, IDD_SIDECONTROL_UDP_CONECT_BTN);
	ExpandBtn.Create(L"", WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_TEXT,
		CRect(PS_PP(0, 5, 20, 20)), this, IDD_SIDECONTROL_EXPAND_BTN);
	urlField.Create(WS_VISIBLE, 
		CRect(PS_PP(25, 5, 200, 20)), this, IDD_URLFIELD);
	// TODO:  Добавьте специализированный код создания
	expand();
	return 0;
}

BOOL CSideControlWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);  // Получаем размеры клиентской области
	pDC->FillSolidRect(&rect, RGB(180, 180, 180));
	return CWnd::OnEraseBkgnd(pDC);
}