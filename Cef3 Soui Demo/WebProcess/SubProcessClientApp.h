#pragma once

#include "include/cef_app.h"

class CefClient;

class SubProcessClientApp : public CefApp
	, public CefRenderProcessHandler
{
public:

	SubProcessClientApp() {}
	~SubProcessClientApp() {}

	static CefRefPtr<SubProcessClientApp>& Instance();
	static int  CefAppInitialize();
	static void CefAppDeInitialize();

	virtual void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line) OVERRIDE;

	// CefApp methods:
	//virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE { return this; }

	//
	// CefRenderProcessHandler methods:
	//

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) OVERRIDE;

	virtual void OnUncaughtException(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
		CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;

	virtual bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:
	IMPLEMENT_REFCOUNTING(SubProcessClientApp);
};
