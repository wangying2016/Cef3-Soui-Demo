#include "HtmlEventHandler.h"

bool HtmlEventHandler::Execute(const CefString& name,
	CefRefPtr<CefV8Value> object,
	const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval,
	CefString& exception)
{
	if (name != "HandleEvent" || arguments.size() == 0)
	{
		return true;
	}

	CefRefPtr<CefBrowser> browser =
		CefV8Context::GetCurrentContext()->GetBrowser();

	CefRefPtr<CefProcessMessage> message =
		CefProcessMessage::Create(arguments[0]->GetStringValue());

	message->GetArgumentList()->SetSize(arguments.size() - 1);
	for (size_t i = 1; i < arguments.size(); ++i)
	{
		message->GetArgumentList()->SetString(i - 1, arguments[i]->GetStringValue());
	}

	browser->SendProcessMessage(PID_BROWSER, message);

	return false;
}