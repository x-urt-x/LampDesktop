#pragma once
#include "framework.h"
class CSideControlWnd : public CWnd
{
public:
    CSideControlWnd() : _expandedWidth(250), _collapsedWidth(20), _isExpanded(true) {}
    int getWidth() const;
private:
    CButton ExpandBtn;

    CNetAddressCtrl urlField;
    CButton WebViewBtn;
    CButton UDPConectBtn;



    void expand();
    void collapse();
    int _expandedWidth;
    int _collapsedWidth;
    bool _isExpanded;
public:
    DECLARE_MESSAGE_MAP()
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void toggleExpand();
    afx_msg void webViewConect();
};

