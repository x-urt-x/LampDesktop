#include "framework.h"
#include "CSideControlWnd.h"
#include "Resource.h"
#include <vector>
#include "CConfigD.h"
#include "CConfigD.h"

BEGIN_MESSAGE_MAP(CSideControlWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDD_SIDECONTROL_EXPAND_BTN, &CSideControlWnd::toggleExpand)
	ON_BN_CLICKED(IDD_SIDECONTROL_WEBVIEW_BTN, &CSideControlWnd::webViewConect)
	ON_NOTIFY(NM_DBLCLK, IDD_MONITORSLIST, &CSideControlWnd::createCfgDialog)
END_MESSAGE_MAP()


int CSideControlWnd::getWidth() const
{
	return _isExpanded ? _expandedWidth : _collapsedWidth;
}

bool CSideControlWnd::GetMonitorsInfo()
{
	UINT32 num_paths;
	UINT32 num_modes;
	std::vector<DISPLAYCONFIG_PATH_INFO> paths;
	std::vector<DISPLAYCONFIG_MODE_INFO> modes;
	LONG res;

	// The display configuration could change between the call to
	// GetDisplayConfigBufferSizes and the call to QueryDisplayConfig, so call
	// them in a loop until the correct buffer size is chosen
	do {
		res = GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &num_paths, &num_modes);
		if (res == ERROR_SUCCESS) {
			paths.resize(num_paths);
			modes.resize(num_modes);
			res = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &num_paths, paths.data(), &num_modes, modes.data(), nullptr);
		}
	} while (res == ERROR_INSUFFICIENT_BUFFER);

	if (res != ERROR_SUCCESS) {
		return false;
	}

	// num_paths and num_modes could decrease in a loop
	paths.resize(num_paths);
	modes.resize(num_modes);


	for (const auto& path : paths) {
		// Send a GET_SOURCE_NAME request
		DISPLAYCONFIG_SOURCE_DEVICE_NAME source = {
			{DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME, sizeof(source), path.sourceInfo.adapterId, path.sourceInfo.id}, {},
		};
		if (DisplayConfigGetDeviceInfo(&source.header) == ERROR_SUCCESS) {
			MonitorCfg cfg;
			if (path.sourceInfo.modeInfoIdx != DISPLAYCONFIG_PATH_MODE_IDX_INVALID) {
				const auto& mode = modes[path.sourceInfo.modeInfoIdx];
				if (mode.infoType == DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE)
					cfg.res.Format(_T("%ux%u"), mode.sourceMode.width, mode.sourceMode.height);
			}

			DISPLAYCONFIG_TARGET_DEVICE_NAME name = {
				{DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME, sizeof(name), path.sourceInfo.adapterId, path.targetInfo.id}, {},
			};
			res = DisplayConfigGetDeviceInfo(&name.header);
			if (ERROR_SUCCESS == res)
				cfg.name = CString(name.monitorFriendlyDeviceName);
			_monitorsCfg.emplace_back(cfg);
		}
	}

	return _monitorsCfg.size() > 0;
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
	if (FAILED(UrlField.GetAddress(&adr_str)))
	{
		UrlField.DisplayErrorTip();
		return;
	}
	UrlField.GetWindowTextW(_url);
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
	UrlField.Create(WS_VISIBLE,
		CRect(PS_PP(25, 5, 200, 20)), this, IDD_URLFIELD);
	MonitorsList.Create(WS_VISIBLE | LVS_REPORT | LVS_NOCOLUMNHEADER,
		CRect(PS_PP(25, 90, 200, 10)), this, IDD_MONITORSLIST);

	if (!GetMonitorsInfo())
		return -1;
	MonitorsList.InsertColumn(0, _T(""), LVCFMT_LEFT, 175);
	for (auto cfg : _monitorsCfg)
		MonitorsList.InsertItem(99, (cfg.name + ' ' + cfg.res));
	CRect rect;
	MonitorsList.GetItemRect(0, &rect, LVIR_BOUNDS);
	CRect windowRect;
	MonitorsList.GetWindowRect(&windowRect);
	MonitorsList.GetParent()->ScreenToClient(&windowRect);
	MonitorsList.SetWindowPos(NULL, windowRect.left, windowRect.top, windowRect.Width(), rect.Height() * MonitorsList.GetItemCount() + 5, SWP_NOZORDER | SWP_NOMOVE);


	// TODO:  Добавьте специализированный код создания
	expand();
	return 0;
}
void CSideControlWnd::createCfgDialog(NMHDR* pNotifyStruct, LRESULT* result)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNotifyStruct);

	if (pNMItemActivate->iItem != -1) {
		int itemIndex = pNMItemActivate->iItem;
		CConfigD cfgD(&_monitorsCfg[itemIndex]);
		cfgD.DoModal();
	}
	return;
}

BOOL CSideControlWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);  // Получаем размеры клиентской области
	pDC->FillSolidRect(&rect, RGB(180, 180, 180));
	return CWnd::OnEraseBkgnd(pDC);
}