#pragma once

#include "BrowserHandler.h"

namespace SOUI
{

	class SCefWebView : public SWindow
		, public BrowserHandler::OffScreenRender
		, public BrowserHandler::MessageHandler
	{
		SOUI_CLASS_NAME(SCefWebView, L"cef")

	public:
		SCefWebView(void);
		~SCefWebView(void);

		// ------------------------------------------------------------------------------
		//
		// methods for user
		//
		// ------------------------------------------------------------------------------

		BOOL LoadURL(const SStringW& url);
		SStringW GetTitle();
		void ExecJavaScript(const SStringW& js);
		BOOL CanGoBack();
		void GoBack();
		BOOL CanGoForward();
		void GoForward();
		BOOL IsLoading();
		void Reload();
		void StopLoad();
		BOOL Open();
		void Close();


		// ------------------------------------------------------------------------------
		//
		// browser callbacks
		//
		// ------------------------------------------------------------------------------
	protected:

		//
		// callback helper methods
		//

		void SendMouseMoveEvent(UINT uMsg, WPARAM wp, LPARAM lp);
		void SendMouseClickEvent(UINT uMsg, WPARAM wp, LPARAM lp);
		void SendFocusEvent(bool focus);
		void SendMouseWheelEvent(UINT uMsg, WPARAM wp, LPARAM lp);
		void SendKeyEvent(UINT uMsg, WPARAM wp, LPARAM lp);
		void SendCaptureLostEvent(UINT uMsg, WPARAM wp, LPARAM lp);
		void  Draw(IBitmap * pbuff);
		BOOL  IsCaptured();

		//
		// ClientHandler::RenderHandler methods
		//
		virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

		//
		// ClientHandler::MessageHandler methods
		//
		virtual bool OnBrowserMessage(CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) OVERRIDE;

		//
		// CefRenderHandler methods
		//

		virtual bool GetViewRect(CefRefPtr<CefBrowser> browser,
			CefRect& rect) OVERRIDE;

		virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
			int viewX,
			int viewY,
			int& screenX,
			int& screenY) OVERRIDE;

		virtual void OnPaint(CefRefPtr<CefBrowser> browser,
			PaintElementType type,
			const RectList& dirtyRects,
			const void* buffer,
			int width,
			int height) OVERRIDE;

		virtual void OnCursorChange(CefRefPtr<CefBrowser> browser,
			CefCursorHandle cursor,
			CursorType type,
			const CefCursorInfo& custom_cursor_info) OVERRIDE;

		// ------------------------------------------------------------------------------
		//
		// internal event handlers
		//
		// ------------------------------------------------------------------------------

	protected:

		void AdjustPixmap(int width, int height);
		void UpdateBkgndRenderTarget();
		BOOL OnAttrUrl(SStringW strValue, BOOL bLoading);

		SOUI_ATTRS_BEGIN()
			ATTR_CUSTOM(L"url", OnAttrUrl)
			SOUI_ATTRS_END()

			virtual BOOL OnSetCursor(const CPoint &pt);

		int  OnCreate(LPVOID);
		void OnDestroy();
		void OnSize(UINT nType, CSize size);
		void OnPaint(IRenderTarget *pRT);
		void OnSetFocus(SWND wndOld);
		void OnKillFocus(SWND wndFocus);
		void OnShowWindow(BOOL bShow, UINT nStatus);

		LRESULT OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_CREATE(OnCreate)
			MSG_WM_DESTROY(OnDestroy)
			MSG_WM_SIZE(OnSize)
			MSG_WM_PAINT_EX(OnPaint)
			MSG_WM_SETFOCUS_EX(OnSetFocus)
			MSG_WM_KILLFOCUS_EX(OnKillFocus)
			MSG_WM_SHOWWINDOW(OnShowWindow)
			MESSAGE_HANDLER_EX(WM_MOUSEMOVE, OnMouseMove)
			MESSAGE_HANDLER_EX(WM_MOUSEWHEEL, OnMouseWheel)
			MESSAGE_HANDLER_EX(WM_CAPTURECHANGED, OnCaptureChanged)
			MESSAGE_HANDLER_EX(WM_CANCELMODE, OnCaptureChanged)
			MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, 0x209, OnMouseEvent)
			MESSAGE_RANGE_HANDLER_EX(WM_KEYFIRST, WM_KEYLAST, OnKeyEvent)
			//MESSAGE_RANGE_HANDLER_EX(WM_IME_STARTCOMPOSITION, WM_IME_KEYLAST, OnKeyEvent)
			//MESSAGE_RANGE_HANDLER_EX(WM_IME_SETCONTEXT, WM_IME_KEYUP, OnKeyEvent)
			SOUI_MSG_MAP_END()

			// ------------------------------------------------------------------------------
			//
			// members
			//
			// ------------------------------------------------------------------------------
	protected:

		BOOL                            m_bInternalPaint;
		BrowserHandler*                 m_pBrowserHandler;
		SStringW                        m_strUrl;
		SStringW                        m_strTitle;
		BOOL                            m_bSkipCursor;
		BOOL                            m_bBkgndDirty;
		CAutoRefPtr<IRenderTarget>      m_prtBackground;  /**< »º´æ´°¿Ú»æÖÆµÄRT */
		CAutoRefPtr<IBitmap>            m_pBitmapBuff;
	};

}
