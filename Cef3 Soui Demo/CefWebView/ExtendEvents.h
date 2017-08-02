#pragma once


namespace SOUI
{

	//----------------------------------------------------------------------------------
	//
	// CefWebView 窗口相关通知，起始偏移900
	//
	// 只实现了自定义的js通知, js调用HandleEvent时,会通过以下通知, 通知订阅者。
	//
	// CEF本身还有很多别的通知, 例如加载完成，加载出错等等,这些就需要另外实现
	//
	//
	//----------------------------------------------------------------------------------

#define EVT_CEFWEBVIEW_BEGIN        (EVT_EXTERNAL_BEGIN + 900)
#define EVT_WEBVIEW_NOTIFY          (EVT_CEFWEBVIEW_BEGIN+0)

	class EventWebViewNotify : public TplEventArgs<EventWebViewNotify>
	{
		SOUI_CLASS_NAME(EventWebViewNotify, L"on_webview_notify")
	public:
		EventWebViewNotify(SObject *pSender) :TplEventArgs<EventWebViewNotify>(pSender) {}
		enum { EventID = EVT_WEBVIEW_NOTIFY };

		SStringW         MessageName;
		SArray<SStringW> Arguments;
	};


};// namespace SOUI