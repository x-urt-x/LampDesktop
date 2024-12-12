// CConfigD.cpp: файл реализации
//

#include "framework.h"
#include "EdgeBrowserApp.h"
#include "CConfigD.h"


// Диалоговое окно CConfigD

IMPLEMENT_DYNAMIC(CConfigD, CDialogEx)

CConfigD::CConfigD(MonitorCfg* cfg, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ConfigD, pParent), _cfg(cfg)
{

}

CConfigD::~CConfigD()
{}

void CConfigD::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDD_ENABLE, _isActive);
	DDX_Control(pDX, IDD_BR, _br);
	DDX_Control(pDX, IDD_RATE, _rate);
}


BEGIN_MESSAGE_MAP(CConfigD, CDialogEx)
	ON_BN_CLICKED(IDOK, &CConfigD::OnBnClickedOk)
END_MESSAGE_MAP()


// Обработчики сообщений CConfigD


BOOL CConfigD::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_isActive.SetCheck(_cfg->getIsActive());

	CString rate;
	rate.Format(L"%u", _cfg->getRate());
	_rate.ModifyStyle(0, ES_NUMBER);
	_rate.SetWindowTextW(rate);

	CString br;
	br.Format(L"%u", _cfg->getBr());
	_br.ModifyStyle(0, ES_NUMBER);
	_br.SetWindowTextW(br);

	return TRUE;
}


void CConfigD::OnBnClickedOk()
{
	_cfg->setBr(GetDlgItemInt(IDD_BR));
	_cfg->setRate(GetDlgItemInt(IDD_RATE));
	_cfg->setIsActive(IsDlgButtonChecked(IDD_ENABLE));

	CDialogEx::OnOK();
}
