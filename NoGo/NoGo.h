
// NoGo.h: NoGo 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"       // 主符号

#define CT_NOGO 0
#define CT_GOBANG 1

// CNoGoApp:
// 有关此类的实现，请参阅 NoGo.cpp
//

class CNoGoApp : public CWinAppEx
{
public:
	CNoGoApp() noexcept;


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CNoGoApp theApp;
