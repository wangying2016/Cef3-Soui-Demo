#include "include/cef_client.h"
#include "SubProcessClientApp.h"
#include "HtmlEventHandler.h"

int SubProcessClientApp::CefAppInitialize()
{
	CefRefPtr<SubProcessClientApp>  pApp = Instance();

	// get arguments
	CefMainArgs mainArgs(GetModuleHandle(NULL));

	// Execute the secondary process, if any.
	int exit_code = CefExecuteProcess(mainArgs, pApp.get(), NULL);
	if (exit_code >= 0)
		return exit_code;

	CefSettings settings;
	//settings.no_sandbox = TRUE;
	settings.multi_threaded_message_loop = TRUE;
	settings.remote_debugging_port = 5050;

	CefString(&settings.cache_path) = "cache\0\0";

	//CEF Initiaized
	CefInitialize(mainArgs, settings, pApp.get(), NULL);

	return true;
}

void SubProcessClientApp::CefAppDeInitialize()
{
	CefShutdown();
}

void SubProcessClientApp::OnBeforeCommandLineProcessing(
	const CefString& process_type,
	CefRefPtr<CefCommandLine> command_line)
{
	command_line->AppendSwitch("--disable-gpu");
	command_line->AppendSwitchWithValue("--renderer-process-limit", "1");
}

CefRefPtr<SubProcessClientApp>& SubProcessClientApp::Instance()
{
	static CefRefPtr<SubProcessClientApp> pApp;
	if (pApp == NULL)
	{
		pApp = new SubProcessClientApp();
	}

	return pApp;
}

void SubProcessClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context)
{
	MessageBox(NULL, L"OnContextCreated", L"Inform", MB_OK);
	CefRefPtr<CefV8Value> object = context->GetGlobal();
	CefRefPtr<CefV8Handler> handler = new HtmlEventHandler();
	CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("HandleEvent", handler);
	object->SetValue("HandleEvent", func, V8_PROPERTY_ATTRIBUTE_NONE);
}

void SubProcessClientApp::OnUncaughtException(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context,
	CefRefPtr<CefV8Exception> exception,
	CefRefPtr<CefV8StackTrace> stackTrace)
{
}

bool SubProcessClientApp::OnProcessMessageReceived(
	CefRefPtr<CefBrowser> browser,
	CefProcessId source_process,
	CefRefPtr<CefProcessMessage> message)
{
	return true;
}
