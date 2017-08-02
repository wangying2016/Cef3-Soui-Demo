#pragma  once

#include "include/cef_app.h"
#include "include/cef_v8.h"

class HtmlEventHandler : public CefV8Handler {
public:
	HtmlEventHandler() {}

	virtual bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception) OVERRIDE;

	// Provide the reference counting implementation for this class.
	IMPLEMENT_REFCOUNTING(HtmlEventHandler);
};
