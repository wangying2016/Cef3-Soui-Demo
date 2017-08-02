// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// This file is shared by cefclient and cef_unittests so don't include using
// a qualified path.

#include "stdafx.h"
#include "CefClientApp.h"  // NOLINT(build/include)
#include "include/cef_cookie.h"
#include "CefUtil.h"  // NOLINT(build/include)

CefClientApp::CefClientApp()
{
}

void CefClientApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line)
{
	command_line->AppendSwitch("--disable-gpu");
	command_line->AppendSwitch("--ignore-certificate-errors");
	command_line->AppendSwitch("--no-proxy-server");
	command_line->AppendSwitchWithValue("--renderer-process-limit", "1");
	command_line->AppendSwitchWithValue("--browser-subprocess-path", "WebProcess.exe");
}

int CefClientApp::m_nBrowserOpened;

int  CefClientApp::GetOpenedBrowserCount()
{
	return m_nBrowserOpened;
}

int CefClientApp::AddOpenedBrowserCount()
{
	return ++m_nBrowserOpened;
}

int CefClientApp::DelOpenedBrowserCount()
{
	return --m_nBrowserOpened;
}

BOOL CefClientApp::Initialize()
{
	// get arguments
	CefMainArgs mainArgs(GetModuleHandle(NULL));

	CefRefPtr<CefClientApp> app(new CefClientApp);

	int exit_code = CefExecuteProcess(mainArgs, app.get(), NULL);
	if (exit_code >= 0)
		return exit_code;

	// setup settings
	CefSettings settings;
	settings.persist_session_cookies = TRUE;
	settings.remote_debugging_port = 5050;
	settings.uncaught_exception_stack_size = 100;
	settings.windowless_rendering_enabled = true;
	//settings.single_process = true;
	//CefString(&settings.cache_path) = _T("cache\0");

	// 
	// CEF Initiaized
	// 
	bool bRet = CefInitialize(mainArgs, settings, app.get(), NULL);

	return bRet;
}

void CefClientApp::UnInitialize()
{
	STRACE(_T("shutting down cef..."));
	CefShutdown();
}

void CefClientApp::RunMessageLoop()
{
	CefRunMessageLoop();
}

void CefClientApp::DoMessageLoopWork()
{
	CefDoMessageLoopWork();
}