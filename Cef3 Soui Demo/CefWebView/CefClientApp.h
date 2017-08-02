// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
#define CEF_TESTS_CEFCLIENT_CLIENT_APP_H_

#pragma once
#include "include/cef_app.h"

class CefClientApp : public CefApp
{
public:
	CefClientApp();

	//
	// CefApp methods
	// 
	virtual void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line) OVERRIDE;

	// ------------------------------------------------------------------------------
	//
	// methods for user
	//
	// ------------------------------------------------------------------------------

public:
	static BOOL Initialize();
	static void UnInitialize();
	static void RunMessageLoop();
	static void DoMessageLoopWork();
	static int  GetOpenedBrowserCount();
	static int  AddOpenedBrowserCount();
	static int  DelOpenedBrowserCount();

	IMPLEMENT_REFCOUNTING(CefClientApp);

private:
	static int m_nBrowserOpened;
};

#endif  // CEF_TESTS_CEFCLIENT_CLIENT_APP_H_
