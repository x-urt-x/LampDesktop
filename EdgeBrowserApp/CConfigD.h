#pragma once
#include "framework.h"
#include "MonitorCfg.h"


// Диалоговое окно CConfigD

class CConfigD : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigD)

public:
	CConfigD(MonitorCfg* cfg, CWnd* pParent = nullptr);
	virtual ~CConfigD();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ConfigD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	MonitorCfg* _cfg;
	CButton _isActive;
	CEdit _br;
	CEdit _rate;
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
};
