
// EdgeBrowserAppDlg.h : header file
//

#pragma once
#include "ComponentBase.h"
//#include "EdgeBrowserApp.h"
#include <dcomp.h>
#include <functional>
#include <memory>
#include <ole2.h>
#include <string>
#include <vector>
#include <winnt.h>


// CEdgeBrowserAppDlg dialog
class CEdgeBrowserAppDlg : public CWnd
{
	// Construction
public:
	CEdgeBrowserAppDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDGEBROWSERAPP_DIALOG };
#endif
	void InitializeWebView(LPCWSTR url);
	void CloseWebView(bool cleanupUserDataFolder = false);
	HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment);
	HRESULT OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);
	void RunAsync(std::function<void(void)> callback);
	void ResizeEverything();
	HRESULT DCompositionCreateDevice2(IUnknown* renderingDevice, REFIID riid, void** ppv);

	void OnSize(UINT a, int b, int c);

	ICoreWebView2Controller* GetWebViewController()
	{
		return m_controller.Get();
	}
	ICoreWebView2* GetWebView()
	{
		return m_webView.Get();
	}
	ICoreWebView2Environment* GetWebViewEnvironment()
	{
		return m_webViewEnvironment.Get();
	}
	HWND GetMainWindow()
	{
		return this->GetSafeHwnd();
	}

	// Implementation
protected:
	HICON m_hIcon;
	DWORD m_creationModeId = 0;
	Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_webViewEnvironment;
	Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
	Microsoft::WRL::ComPtr<ICoreWebView2> m_webView;
	Microsoft::WRL::ComPtr<IDCompositionDevice> m_dcompDevice;
	std::vector<std::unique_ptr<ComponentBase>> m_components;
	HWND m_mainWindow = nullptr;
	HINSTANCE g_hInstance;
	static constexpr size_t s_maxLoadString = 100;
	template <class ComponentType, class... Args> void NewComponent(Args&&... args);

	template <class ComponentType> ComponentType* GetComponent();

	// Generated message map functions

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	LPCWSTR _url;
};

template <class ComponentType, class... Args> void CEdgeBrowserAppDlg::NewComponent(Args&&... args)
{
	m_components.emplace_back(new ComponentType(std::forward<Args>(args)...));
}

template <class ComponentType> ComponentType* CEdgeBrowserAppDlg::GetComponent()
{
	for (auto& component : m_components)
	{
		if (auto wanted = dynamic_cast<ComponentType*>(component.get()))
		{
			return wanted;
		}
	}
	return nullptr;
}

