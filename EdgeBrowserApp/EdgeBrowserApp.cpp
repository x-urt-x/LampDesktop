//Author: Ayush Chaudhary
//Email: ayush.shyam@gmail.com
//License: Free license 
//Description: In this application I show how to use webview2 edge browser in yoor dialog based applications.

#include "stdafx.h"
#include "framework.h"
#include "EdgeBrowserApp.h"
#include "CMainFrameWnd.h"
//#include "Browser.h"
#include <string.h>
#include <vector>
#include <map>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef __windows__
#undef __windows__
#endif

// CEdgeBrowserAppApp

BEGIN_MESSAGE_MAP(CEdgeBrowserAppApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

HINSTANCE g_hInstance;
int g_nCmdShow;
bool g_autoTabHandle = true;
static std::map<DWORD, HANDLE> s_threads;
static int RunMessagePump();
static DWORD WINAPI ThreadProc(void* pvParam);
static void WaitForOtherThreads();
static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// CEdgeBrowserAppApp construction

CEdgeBrowserAppApp::CEdgeBrowserAppApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CEdgeBrowserAppApp object

CEdgeBrowserAppApp theApp;


// CEdgeBrowserAppApp initialization

BOOL CEdgeBrowserAppApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	//AfxEnableControlContainer();

	//CShellManager *pShellManager = new CShellManager;
	//CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	//CEdgeBrowserAppDlg dlg;
	CFrameWnd* pFrame = new CMainFrameWnd;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	bool ans = pFrame->Create(NULL, L"Lamp Control", WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX , CRect(0, 0, 1000, 1000), NULL, nullptr);


	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	//MessageBoxW(nullptr, L"test", L"Test box", 0);

	//INT_PTR nResponse = dlg.DoModal();
	//if (nResponse == IDOK)
	//{
	//	// TODO: Place code here to handle when the dialog is
	//	//  dismissed with OK
	//}

	//// Delete the shell manager created above.
	//if (pShellManager != nullptr)
	//{
	//	delete pShellManager;
	//}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}


LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

static DWORD WINAPI ThreadProc(void* pvParam)
{
	//new Browser();
	return RunMessagePump();
}


static void WaitForOtherThreads()
{
	while (!s_threads.empty())
	{
		std::vector<HANDLE> threadHandles;
		for (auto it = s_threads.begin(); it != s_threads.end(); ++it)
		{
			threadHandles.push_back(it->second);
		}

		HANDLE* handleArray = threadHandles.data();
		DWORD dwIndex = MsgWaitForMultipleObjects(
			static_cast<DWORD>(threadHandles.size()), threadHandles.data(), FALSE,
			INFINITE, QS_ALLEVENTS);

		if (dwIndex == WAIT_OBJECT_0 + threadHandles.size())
		{
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}


static int RunMessagePump()
{
	HACCEL hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDC_EDGE));

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			if (!g_autoTabHandle || !IsDialogMessage(GetAncestor(msg.hwnd, GA_ROOT), &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	DWORD threadId = GetCurrentThreadId();
	auto it = s_threads.find(threadId);
	if (it != s_threads.end())
	{
		CloseHandle(it->second);
		s_threads.erase(threadId);
	}

	return (int)msg.wParam;
}
