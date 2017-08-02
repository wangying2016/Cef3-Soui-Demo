#include "StdAfx.h"
#include "souistd.h"
#include "Cef3WebView.h"
#include "CefClientApp.h"
#include "CefUtil.h"
#include "ExtendEvents.h"

namespace SOUI
{

#define CHECK_CEF_BROWSER_HOST									\
    if ( !m_pBrowserHandler ) return;							\
    CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser(); \
    if ( !pb.get() ) return;									\
    CefRefPtr<CefBrowserHost> host = pb->GetHost();				\
    if (!host) return 

	SCefWebView::SCefWebView(void) : m_bSkipCursor(FALSE)
		, m_bInternalPaint(FALSE)
		, m_bBkgndDirty(TRUE)
	{
		m_evtSet.addEvent(EVENTID(EventWebViewNotify));

		m_pBrowserHandler = new BrowserHandler;
		m_pBrowserHandler->AddRef();

		m_bFocusable = TRUE;
	}

	SCefWebView::~SCefWebView(void)
	{
		//m_pBrowser=NULL;
	}

	// ------------------------------------------------------------------------------
	//
	// methods for user
	//
	// ------------------------------------------------------------------------------

	BOOL SCefWebView::LoadURL(const SStringW& url)
	{
		if (!m_pBrowserHandler)
		{
			return FALSE;
		}

		CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser();
		if (pb.get())
		{
			CefRefPtr<CefFrame> frame = pb->GetMainFrame();
			if (!frame)
				return FALSE;

			m_strUrl = url;
			frame->LoadURL((LPCWSTR)url);
		}
		return TRUE;
	}

	SStringW SCefWebView::GetTitle()
	{
		return m_strTitle;
	}

	void SCefWebView::ExecJavaScript(const SStringW& js)
	{
		if (!m_pBrowserHandler)
		{
			return;
		}

		CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser();
		if (pb.get())
		{
			CefRefPtr<CefFrame> frame = pb->GetMainFrame();
			if (frame)
			{
				frame->ExecuteJavaScript((LPCWSTR)js, L"", 0);
			}
		}
	}

	BOOL SCefWebView::CanGoBack()
	{
		if (!m_pBrowserHandler)
		{
			return FALSE;
		}

		CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser();
		if (pb.get())
		{
			return pb->CanGoBack();
		}

		return FALSE;
	}

	void SCefWebView::GoBack()
	{
		if (!m_pBrowserHandler)
		{
			return;
		}

		CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser();
		if (pb.get())
		{
			return pb->GoBack();
		}
	}

	BOOL SCefWebView::CanGoForward()
	{
		if (!m_pBrowserHandler)
		{
			return FALSE;
		}

		CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser();
		if (pb.get())
		{
			return pb->CanGoForward();
		}

		return FALSE;
	}

	void SCefWebView::GoForward()
	{
		if (!m_pBrowserHandler)
		{
			return;
		}

		CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser();
		if (pb.get())
		{
			return pb->GoForward();
		}
	}

	BOOL SCefWebView::IsLoading()
	{
		if (!m_pBrowserHandler)
		{
			return FALSE;
		}

		CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser();
		if (pb.get())
		{
			return pb->IsLoading();
		}

		return FALSE;
	}

	void SCefWebView::Reload()
	{
		if (!m_pBrowserHandler)
		{
			return;
		}

		CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser();
		if (pb.get())
		{
			return pb->Reload();
		}
	}

	void SCefWebView::StopLoad()
	{
		if (!m_pBrowserHandler)
		{
			return;
		}

		CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser();
		if (pb.get())
		{
			return pb->StopLoad();
		}
	}

	BOOL SCefWebView::Open()
	{
		CefRefPtr<CefBrowser> pb = m_pBrowserHandler->GetBrowser();
		if (pb.get())
		{
			return TRUE;
		}

		if (!m_pBrowserHandler->Open(GetContainer()->GetHostHwnd(), GetClientRect()))
		{
			return FALSE;
		}

		m_pBrowserHandler->SetRender(this);
		m_pBrowserHandler->RegisterMessageHandler(this);
		return TRUE;
	}

	void SCefWebView::Close()
	{
		m_pBrowserHandler->CloseAllBrowsers(true);
		m_pBrowserHandler->SetRender(NULL);
		m_pBrowserHandler->UnRegisterMessgeHandler(this);
		m_bBkgndDirty = TRUE;
		m_pBitmapBuff = NULL;
	}

	void SCefWebView::UpdateBkgndRenderTarget()
	{
		// 更新背景RenderTarget
		CRect rcWnd = GetClientRect();
		if (!m_prtBackground)
		{
			GETRENDERFACTORY->CreateRenderTarget(&m_prtBackground, rcWnd.Width(), rcWnd.Height());
		} else
		{
			m_prtBackground->Resize(rcWnd.Size());
		}
		m_prtBackground->SetViewportOrg(-rcWnd.TopLeft());
		m_bBkgndDirty = TRUE;
	}

	int SCefWebView::OnCreate(LPVOID)
	{
		/* m_pDropTarget = DropTargetWin::Create(this, GetContainer()->GetHostHwnd());
		GetContainer()->RegisterDragDrop(GetSwnd(), m_pDropTarget);*/

		if (!m_strUrl.IsEmpty())
		{
			Open();
			LoadURL(m_strUrl);
		}

		return 0;
	}

	void SCefWebView::OnSize(UINT nType, CSize size)
	{
		STRACE(L"webview resized");

		SWindow::OnSize(nType, size);
		UpdateBkgndRenderTarget();


		if (!m_pBitmapBuff || (m_pBitmapBuff->Width() != size.cx || m_pBitmapBuff->Height() != size.cy))
		{
			CHECK_CEF_BROWSER_HOST;
			host->WasResized();
		}
	}

	BOOL SCefWebView::OnAttrUrl(SStringW strValue, BOOL bLoading)
	{
		m_strUrl = strValue;

		if (!bLoading)
		{
			LoadURL(strValue);
		}

		return !bLoading;
	}

	void SCefWebView::OnDestroy()
	{
		STRACE(L"webview destroy");

		if (m_pBrowserHandler)
		{
			Close();
		}

		SWindow::OnDestroy();
	}

	void SCefWebView::OnPaint(IRenderTarget *pRT)
	{
		//STRACE(L"web view onpaint");

		CRect rcWnd = GetClientRect();

		if (m_bBkgndDirty && m_prtBackground)
		{
			m_bBkgndDirty = FALSE;
			m_prtBackground->BitBlt(rcWnd, pRT, rcWnd.left, rcWnd.top);
		}

		SWindow::OnPaint(pRT);

		if (m_pBrowserHandler)
		{
			CRect rcClipBox;
			pRT->GetClipBox(rcClipBox);
			rcClipBox.IntersectRect(rcClipBox, rcWnd);

			CHECK_CEF_BROWSER_HOST;

			// request cef to update buff bitmap
			m_bInternalPaint = TRUE;

			host->Invalidate(PET_VIEW);

			m_bInternalPaint = FALSE;
		}

		if (m_pBitmapBuff)
		{
			rcWnd.right = rcWnd.left + m_pBitmapBuff->Width();
			rcWnd.bottom = rcWnd.top + m_pBitmapBuff->Height();
			pRT->DrawBitmap(rcWnd, m_pBitmapBuff, 0, 0, 0xFF);
		}
	}

	LRESULT SCefWebView::OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SendCaptureLostEvent(uMsg, wParam, lParam);
		return 0;
	}

	LRESULT SCefWebView::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SendMouseMoveEvent(uMsg, wParam, lParam);
		return 0;
	}

	LRESULT SCefWebView::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SendMouseWheelEvent(uMsg, wParam, lParam);
		return 0;
	}

	LRESULT SCefWebView::OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_LBUTTONDOWN)
		{
			SetFocus();
			SetCapture();
		} else if (uMsg == WM_LBUTTONUP)
		{
			ReleaseCapture();
		}
		SendMouseClickEvent(uMsg, wParam, lParam);

		CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		SWND hHover = GetRoot()->SwndFromPoint(pt, FALSE);
		SWindow * pHover = SWindowMgr::GetWindow(hHover);

		if (GetSwnd() != hHover && pHover)
		{
			pHover->SetFocus();
			OnKillFocus(NULL);
			SendCaptureLostEvent(uMsg, wParam, lParam);
		}

		return 0;
	}

	LRESULT SCefWebView::OnKeyEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SendKeyEvent(uMsg, wParam, lParam);
		return 0;
	}

	void SCefWebView::OnSetFocus(SWND wndOld)
	{
		CHECK_CEF_BROWSER_HOST;

		STRACE(L"set browser focus");
		host->SendFocusEvent(true);
	}

	void SCefWebView::OnKillFocus(SWND wndFocus)
	{
		CHECK_CEF_BROWSER_HOST;

		STRACE(L"kill browser focus");

		host->SendFocusEvent(false);
	}

	void SCefWebView::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		SWindow::OnShowWindow(bShow, nStatus);

		CHECK_CEF_BROWSER_HOST;
		host->WasHidden(!bShow);
	}

	BOOL SCefWebView::OnSetCursor(const CPoint &pt)
	{
		return TRUE;
	}

	// ------------------------------------------------------------------------------
	//
	// callback helper methods
	//
	// ------------------------------------------------------------------------------
	//

	void SCefWebView::SendMouseMoveEvent(UINT uMsg, WPARAM wp, LPARAM lp)
	{
		//cef cursor是异步的 可能在我们鼠标已经离开webview后 webview还会发送鼠标样式
		//需要忽略掉

		CHECK_CEF_BROWSER_HOST;

		CPoint pt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
		CRect  rcView = GetClientRect();

		bool leave = !rcView.PtInRect(pt) && !IsCaptured();

		if (leave)
			m_bSkipCursor = TRUE;
		else
			m_bSkipCursor = FALSE;

		CefMouseEvent evt;
		evt.x = pt.x - rcView.left;
		evt.y = pt.y - rcView.top;
		evt.modifiers = GetCefMouseModifiers(wp);

		//STRACE(L"browser mouse move, x:%d,y:%d,mod:%d", 
		//    evt.x, evt.y, evt.modifiers);
		host->SendMouseMoveEvent(evt, leave);
	}

	void SCefWebView::SendMouseClickEvent(UINT uMsg, WPARAM wp, LPARAM lp)
	{
		CHECK_CEF_BROWSER_HOST;

		CefMouseEvent evt;
		CefBrowserHost::MouseButtonType mt = MBT_LEFT;
		bool bMouseUp = false;
		int  nClickCount = 1;

		switch (uMsg)
		{
		case WM_LBUTTONUP:
			bMouseUp = true;
		case WM_LBUTTONDOWN:
			mt = MBT_LEFT;
			break;
		case WM_LBUTTONDBLCLK:
			nClickCount = 2;
			break;
		case WM_MBUTTONUP:
			bMouseUp = true;
		case WM_MBUTTONDOWN:
			mt = MBT_MIDDLE;
			break;
		case WM_RBUTTONUP:
			bMouseUp = true;
		case WM_RBUTTONDOWN:
			mt = MBT_RIGHT;
			break;
		}
		CRect rcView = GetClientRect();
		CPoint pt(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));

		evt.x = pt.x - rcView.left;
		evt.y = pt.y - rcView.top;

		evt.modifiers = GetCefMouseModifiers(wp);

		STRACE(L"browser click, msg:%d, x:%d,y:%d,mod:%d, mt:%d, up:%d",
			uMsg, evt.x, evt.y, evt.modifiers, mt, bMouseUp);

		host->SendMouseClickEvent(evt, mt, bMouseUp, nClickCount);
	}

	void SCefWebView::SendFocusEvent(bool focus)
	{
		CHECK_CEF_BROWSER_HOST;

		STRACE(L"sent browser focus:%d", focus);
		host->SendFocusEvent(focus);
	}

	void SCefWebView::SendMouseWheelEvent(UINT uMsg, WPARAM wp, LPARAM lp)
	{
		CHECK_CEF_BROWSER_HOST;

		POINT pt;
		pt.x = GET_X_LPARAM(lp);
		pt.y = GET_Y_LPARAM(lp);

		int delta = GET_WHEEL_DELTA_WPARAM(wp);

		CefMouseEvent evt;
		CRect  rcView = GetClientRect();

		pt.x -= rcView.left;
		pt.y -= rcView.top;
		evt.x = pt.x;
		evt.y = pt.y;
		evt.modifiers = 0;

		STRACE(L"browser mouse wheel, x:%d,y:%d,mod:%d, deltaX:%d, deltaY:%d",
			evt.x, evt.y, evt.modifiers,
			IsKeyDown(VK_SHIFT) ? delta : 0, IsKeyDown(VK_SHIFT) ? 0 : delta);

		host->SendMouseWheelEvent(evt, IsKeyDown(VK_SHIFT) ? delta : 0, IsKeyDown(VK_SHIFT) ? 0 : delta);
	}

	void SCefWebView::SendKeyEvent(UINT uMsg, WPARAM wp, LPARAM lp)
	{
		CHECK_CEF_BROWSER_HOST;

		CefKeyEvent evt = ToCefKeyEvent(uMsg, wp, lp);

		STRACE(L"browser key event. wkcode:%d, nkcode:%x, issys:%d, type:%d, modfiers:%d",
			evt.windows_key_code,
			evt.native_key_code,
			evt.is_system_key,
			evt.type,
			evt.modifiers);

		host->SendKeyEvent(evt);
	}

	void SCefWebView::SendCaptureLostEvent(UINT uMsg, WPARAM wp, LPARAM lp)
	{
		CHECK_CEF_BROWSER_HOST;

		STRACE(L"browser send capture losted");
		host->SendCaptureLostEvent();
	}

	void  SCefWebView::Draw(IBitmap * pbuff)
	{
		if (m_bInternalPaint || !m_prtBackground || m_bBkgndDirty)
		{
			STRACE(L"on ready...");
			return;
		}

		CRect rcWnd = GetClientRect();
		if (rcWnd.Width() != pbuff->Width() || rcWnd.Height() != pbuff->Height())
		{
			rcWnd.right = rcWnd.left + pbuff->Width();
			rcWnd.bottom = rcWnd.top + pbuff->Height();
		}

		CAutoRefPtr<IRegion> rgn;
		GETRENDERFACTORY->CreateRegion(&rgn);
		rgn->CombineRect(rcWnd, RGN_OR);

		CAutoRefPtr<IRenderTarget> pRT = GetContainer()->OnGetRenderTarget(rcWnd, 0);
		pRT->BitBlt(rcWnd, m_prtBackground, rcWnd.left, rcWnd.top);
		pRT->DrawBitmap(rcWnd, pbuff, 0, 0, 0xFF);
		GetContainer()->OnReleaseRenderTarget(pRT, rcWnd, 0);
	}

	BOOL SCefWebView::IsCaptured()
	{
		return GetCapture() == GetSwnd();
	}

	//void SCefWebView::SetTitle(const wchar_t * title)
	//{
	//    m_strTitle = title;
	//}

	// ------------------------------------------------------------------------------
	//
	// browser callbacks
	//
	// ------------------------------------------------------------------------------

	void SCefWebView::OnBeforeClose(CefRefPtr<CefBrowser> browser)
	{

	}

	// 
	//  BrowserHandler::MessageHandlermethods
	//

	bool SCefWebView::OnBrowserMessage(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message)
	{
		EventWebViewNotify evt(this);

		evt.MessageName = message->GetName().ToWString().c_str();
		CefRefPtr<CefListValue> arg = message->GetArgumentList();

		for (int i = 0; i < arg->GetSize(); ++i)
		{
			SStringW str = arg->GetString(i).ToWString().c_str();
			evt.Arguments.Add(str);
		}

		return !!FireEvent(evt);
	}

	//
	// CefRenderHandler methods
	//

	bool SCefWebView::GetViewRect(CefRefPtr<CefBrowser> browser,
		CefRect& rect)
	{
		//STRACE(L"browser get view rect");
		CRect rc = GetClientRect();

		rect.x = rect.y = 0;
		rect.width = rc.Width();
		rect.height = rc.Height();
		return true;
	}

	bool SCefWebView::GetScreenPoint(CefRefPtr<CefBrowser> browser,
		int viewX,
		int viewY,
		int& screenX,
		int& screenY)
	{
		// Convert the point from view coordinates to actual screen coordinates.
		POINT screen_pt = { viewX, viewY };
		ClientToScreen(GetContainer()->GetHostHwnd(), &screen_pt);
		screenX = screen_pt.x;
		screenY = screen_pt.y;
		return true;
	}

	void SCefWebView::AdjustPixmap(int width, int height)
	{
		if (!m_pBitmapBuff)
		{
			GETRENDERFACTORY->CreateBitmap(&m_pBitmapBuff);
		}

		if (m_pBitmapBuff->Width() != width || m_pBitmapBuff->Height() != height)
		{
			m_pBitmapBuff->Init(width, height);
		}
	}


	void SCefWebView::OnPaint(CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		const void* buffer,
		int width,
		int height)
	{
		STRACE(L"cef paint ...");
		if (type != PET_VIEW)
		{
			STRACE(L"not pet view");
			return;
		}

		AdjustPixmap(width, height);
		LPBYTE pDst = (LPBYTE)m_pBitmapBuff->LockPixelBits();
		if (pDst)
		{
			memcpy(pDst, buffer, width * height * 4);
			m_pBitmapBuff->UnlockPixelBits(pDst);
		}

		Draw(m_pBitmapBuff);
	}

	void SCefWebView::OnCursorChange(CefRefPtr<CefBrowser> browser,
		CefCursorHandle cursor,
		CursorType type,
		const CefCursorInfo& custom_cursor_info)
	{
		if (!m_bSkipCursor)
		{
			//SetClassLongPtr(m_hwnd, GCLP_HCURSOR, static_cast<LONG>(reinterpret_cast<LONG_PTR>(cursor)));
			::SetCursor(cursor);
		}
	}

}; // namespace SOUI