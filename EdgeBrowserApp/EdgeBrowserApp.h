#pragma once

#include "resource.h"


// CEdgeBrowserAppApp:
// See EdgeBrowserApp.cpp for the implementation of this class
//

class CEdgeBrowserAppApp : public CWinApp
{
public:
	CEdgeBrowserAppApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CEdgeBrowserAppApp theApp;
