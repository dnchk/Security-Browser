#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

class CMFCApplication1App : public CWinApp
{
public:
	CMFCApplication1App();
	//OnSize(UINT nType, int cx, int cy);
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CMFCApplication1App theApp;
