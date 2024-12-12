#pragma once
#include "framework.h"
#include <string>
#include <vector>
#include <Wbemidl.h>
#include "MonitorCfg.h"
#include "UDPControl.h"
#pragma comment(lib, "wbemuuid.lib")


class CSideControlWnd : public CWnd
{
public:
    CSideControlWnd() : _expandedWidth(250), _collapsedWidth(20), _isExpanded(true) {}
    int getWidth() const;
private:
    CButton ExpandBtn;

    CNetAddressCtrl UrlField;
    CMFCButton WebViewBtn;
    CMFCButton UDPConectBtn;
    CMFCButton UDPCloseBtn;
    COLORREF UDPConectColor;

    CListCtrl MonitorsList;


    bool GetMonitorsInfo();
    std::vector<MonitorCfg> _monitorsCfg;
    
    bool GetUrl();

    UDPControl _updControl;

    void expand();
    void collapse();
    CStringW _url;
    int _expandedWidth;
    int _collapsedWidth;
    bool _isExpanded;
public:
    DECLARE_MESSAGE_MAP()
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void toggleExpand();
    afx_msg void webViewConect();
    afx_msg void UDPConect();
    afx_msg void UDPClose();
    afx_msg void createCfgDialog(NMHDR* pNotifyStruct, LRESULT* result);
    afx_msg LRESULT OnSetButtonColor(WPARAM wParam, LPARAM lParam);
};

