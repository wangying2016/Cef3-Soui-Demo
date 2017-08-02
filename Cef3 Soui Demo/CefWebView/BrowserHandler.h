#pragma once

#include "include/cef_client.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_runnable.h"
#include "CefClientApp.h"
#include "include/cef_drag_handler.h"
#include <list>
#include "souistd.h"

class BrowserHandler : public CefClient,
					   public CefRenderHandler,
					   public CefContextMenuHandler,
					   public CefDisplayHandler,
					   public CefDownloadHandler,
					   public CefDragHandler,
					   public CefKeyboardHandler,
					   public CefLifeSpanHandler,
					   public CefLoadHandler
{
public:

	// Interface implemented to handle off-screen rendering.
	class OffScreenRender : public CefRenderHandler
	{
	public:
		virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) = 0;
		IMPLEMENT_REFCOUNTING(OffScreenRender);
	};

	class MessageHandler
	{
	public:
		virtual bool OnBrowserMessage(CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) = 0;
	};

	BrowserHandler();
	~BrowserHandler();

	// ------------------------------------------------------------------------------
	// 
	// browser methods for user
	//
	// ------------------------------------------------------------------------------

	IMPLEMENT_REFCOUNTING(BrowserHandler);

public:

	BOOL Open(HWND hParent, SOUI::CRect rcView);
	void Close();
	void CloseAllBrowsers(bool force_close);
	void OpenDevTools();
	void SetRender(OffScreenRender * pHost);
	CefRefPtr<CefBrowser> GetBrowser();
	void RegisterMessageHandler(MessageHandler * handler);
	void UnRegisterMessgeHandler(MessageHandler * handler);
	static BOOL IsKeyDown(WPARAM wparam);

	// ------------------------------------------------------------------------------
	//
	// impl cef handler callbacks
	//
	// ------------------------------------------------------------------------------

protected:

	//
	// CefClient methods. Important to return |this| for the handler callbacks.
	// 

	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefRenderHandler> GetRenderHandler()           OVERRIDE { return this; }
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler()         OVERRIDE { return this; }
	virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler()       OVERRIDE { return this; }
	virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler()       OVERRIDE { return this; }
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler()       OVERRIDE { return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler()               OVERRIDE { return this; }
	virtual CefRefPtr<CefDragHandler> GetDragHandler()               OVERRIDE { return this; }

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message)  OVERRIDE
	{
		if (m_pMsgHandler)
		{
			return m_pMsgHandler->OnBrowserMessage(browser, source_process, message);
		}

		return true;
	}

	//
	// CefRenderHandler interfaces
	// 

	virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser,
		CefRect& rect) OVERRIDE;

	virtual void OnPopupShow(CefRefPtr<CefBrowser> browser,
		bool show) OVERRIDE;

	virtual void OnPopupSize(CefRefPtr<CefBrowser> browser,
		const CefRect& rect)  OVERRIDE;

	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser,
		CefRect& rect) OVERRIDE;

	virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
		CefScreenInfo& screen_info)  OVERRIDE;

	virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
		int viewX,
		int viewY,
		int& screenX,
		int& screenY) OVERRIDE;

	virtual void OnPaint(
		CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		const void* buffer,
		int width,
		int height) OVERRIDE;

	virtual void OnCursorChange(CefRefPtr<CefBrowser> browser,
		CefCursorHandle cursor,
		CursorType type,
		const CefCursorInfo& custom_cursor_info) OVERRIDE;

	virtual bool StartDragging(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDragData> drag_data,
		CefRenderHandler::DragOperationsMask allowed_ops,
		int x, int y) OVERRIDE;

	virtual void UpdateDragCursor(
		CefRefPtr<CefBrowser> browser,
		CefRenderHandler::DragOperation operation)
		OVERRIDE;

	//
	// CefContextMenuHandler methods
	//

	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) OVERRIDE
	{
		model->Clear();
	}

	//
	// CefDisplayHandler methods
	//

	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
		const CefString& title) OVERRIDE
	{
		//if (m_pWebView)
		//{
		//    m_pWebView->SetTitle(title.c_str());
		//}
	}

	//
	// CefDownloadHandler methods
	//

	virtual void OnBeforeDownload(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		const CefString& suggested_name,
		CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;

	virtual void OnDownloadUpdated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE;

	//
	// CefKeyboardHandler methods
	//

	virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event,
		bool* is_keyboard_shortcut) OVERRIDE;

	//
	// CefLifeSpanHandler methods
	//

	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser)OVERRIDE;

	//
	// CefLoadHandler methods
	//

	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame) OVERRIDE
	{
	}

	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode) OVERRIDE
	{
		m_bPageLoaded = TRUE;
	}

	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl)
	{
	}

	// List of any popup browser windows. Only accessed on the CEF UI thread.
	typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
	BrowserList m_PopupBrowsers;

private:
	CefRefPtr<CefBrowser>   m_refBrowser;
	BOOL                    m_bPageLoaded;
	OffScreenRender       * m_posr;
	MessageHandler        * m_pMsgHandler;
};

