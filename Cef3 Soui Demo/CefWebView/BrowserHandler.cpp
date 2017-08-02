#include "stdafx.h"
#include "BrowserHandler.h"
#include "include/cef_runnable.h"
#include <shlobj.h>
#include "CefUtil.h"

// ------------------------------------------------------------------------------
// 
// helpers
//
// ------------------------------------------------------------------------------

std::string GetDownloadPath(const std::string& file_name)
{
	TCHAR szFolderPath[MAX_PATH];
	std::string path;

	// Save the file in the user's "My Documents" folder.
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
		NULL, 0, szFolderPath)))
	{
		path = CefString(szFolderPath);
		path += "\\" + file_name;
	}

	return path;
}

BrowserHandler::BrowserHandler() : m_posr(NULL)
, m_bPageLoaded(FALSE)
, m_pMsgHandler(NULL)
{

}

BrowserHandler::~BrowserHandler()
{
}

// ------------------------------------------------------------------------------
// 
// cef handler callbacks
//
// ------------------------------------------------------------------------------

bool BrowserHandler::GetRootScreenRect(CefRefPtr<CefBrowser> browser,
	CefRect& rect)
{
	if (m_posr)
	{
		return m_posr->GetRootScreenRect(browser, rect);
	}

	return false;
}

bool BrowserHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
	if (m_posr)
	{
		return m_posr->GetViewRect(browser, rect);
	}

	return false;
}

bool BrowserHandler::GetScreenPoint(CefRefPtr<CefBrowser> browser,
	int viewX,
	int viewY,
	int& screenX,
	int& screenY)
{
	if (m_posr)
	{
		return m_posr->GetScreenPoint(browser, viewX, viewY, screenX, screenY);
	}

	return false;
}

bool BrowserHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser,
	CefScreenInfo& screen_info)
{
	if (m_posr)
	{
		return m_posr->GetScreenInfo(browser, screen_info);
	}

	return false;
}

void BrowserHandler::OnPopupShow(CefRefPtr<CefBrowser> browser,
	bool show)
{
	if (m_posr)
	{
		return m_posr->OnPopupShow(browser, show);
	}
}

void BrowserHandler::OnPopupSize(CefRefPtr<CefBrowser> browser,
	const CefRect& rect)
{
	if (m_posr)
	{
		return m_posr->OnPopupSize(browser, rect);
	}
}

void BrowserHandler::OnPaint(CefRefPtr<CefBrowser> browser,
	PaintElementType type,
	const RectList & dirtyRects,
	const void* buffer, int width, int height)
{
	if (m_posr)
	{
		m_posr->OnPaint(browser, type, dirtyRects, buffer, width, height);
	}
}

void BrowserHandler::OnCursorChange(CefRefPtr<CefBrowser> browser,
	CefCursorHandle cursor,
	CursorType type,
	const CefCursorInfo& custom_cursor_info)
{
	if (m_posr)
	{
		m_posr->OnCursorChange(browser, cursor, type, custom_cursor_info);
	}
}

bool BrowserHandler::StartDragging(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefDragData> drag_data,
	CefRenderHandler::DragOperationsMask allowed_ops,
	int x, int y)
{
	if (m_posr)
	{
		return m_posr->StartDragging(browser, drag_data, allowed_ops, x, y);
	}

	return false;
}

void BrowserHandler::UpdateDragCursor(
	CefRefPtr<CefBrowser> browser,
	CefRenderHandler::DragOperation operation)
{
	if (m_posr)
	{
		m_posr->UpdateDragCursor(browser, operation);
	}
}

void BrowserHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefDownloadItem> download_item,
	const CefString& suggested_name,
	CefRefPtr<CefBeforeDownloadCallback> callback)
{
	REQUIRE_UI_THREAD();
	// Continue the download and show the "Save As" dialog.
	callback->Continue(GetDownloadPath(suggested_name), true);
}

void BrowserHandler::OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefDownloadItem> download_item,
	CefRefPtr<CefDownloadItemCallback> callback)
{
	REQUIRE_UI_THREAD();
	if ((download_item->IsComplete() || download_item->IsCanceled()) &&
		browser->IsPopup())
	{
		// we should close the pop window
		browser->GetHost()->CloseBrowser(true);
	}
}

BOOL BrowserHandler::IsKeyDown(WPARAM wparam)
{
	return (GetKeyState(wparam) & 0x8000) != 0;
}

bool BrowserHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
	const CefKeyEvent& event,
	CefEventHandle os_event,
	bool* is_keyboard_shortcut)
{
	if (!m_bPageLoaded)
	{
		return true;
	}

	CefWindowHandle hWnd = browser->GetHost()->GetWindowHandle();

	//
	// ÍË³ö
	if (event.character == VK_ESCAPE)
	{
		::PostMessage(::GetParent(hWnd), WM_KEYDOWN, event.character, 0);
		return true;
	}

	//
	// µ÷ÊÔ¿ì½Ý¼ü
	if (IsKeyDown(VK_SHIFT))
	{
		STRACE(L"char:%d", event.character);
		if (event.character == VK_F12)
		{
			OpenDevTools();
		} else if (event.character == VK_F11)
		{
			browser->GetMainFrame()->ViewSource();
		}
	}

	return false;
}

bool BrowserHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	//STRACE(L"browser do close");
	return false;
}

void BrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	REQUIRE_UI_THREAD();

	if (browser->IsPopup())
	{
		m_PopupBrowsers.push_back(browser);
		browser->GetHost()->SetFocus(true);
	}

	CefClientApp::AddOpenedBrowserCount();
	//STRACE(L"on browser after created");
}

void  BrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	//STRACE(L"on browser before close");

	if (m_refBrowser.get() &&
		m_refBrowser->GetIdentifier() == browser->GetIdentifier())
	{
		// !!! IMPORTANT !!!
		m_refBrowser = NULL;
		STRACE(L"main browser closed");
	}

	if (browser->IsPopup())
	{
		BrowserList::iterator bit = m_PopupBrowsers.begin();
		for (; bit != m_PopupBrowsers.end(); ++bit)
		{
			if ((*bit)->IsSame(browser))
			{
				//STRACE(L"popup browser closed");
				m_PopupBrowsers.erase(bit);
				break;
			}
		}
	}

	int n = CefClientApp::DelOpenedBrowserCount();
	//STRACE(L"browser left:%d", n);
	if (n == 0)
	{
		STRACE(L"quit message loop");
		//CefQuitMessageLoop();
	}
}


// ------------------------------------------------------------------------------
// 
// browser methods for user & internal methods
//
// ------------------------------------------------------------------------------

void BrowserHandler::OpenDevTools()
{
	if (!m_refBrowser)
	{
		return;
	}

	CefWindowInfo windowInfo;
	CefBrowserSettings settings;

	windowInfo.SetAsPopup(m_refBrowser->GetHost()->GetWindowHandle(), "DevTools");
	const CefPoint point(0, 0);
	m_refBrowser->GetHost()->ShowDevTools(windowInfo, this, settings, point);
}

BOOL BrowserHandler::Open(HWND hParent, SOUI::CRect rcView)
{
	CefWindowInfo info;
	info.SetAsWindowless(hParent, true);

	info.x = 0;
	info.y = 0;
	info.width = rcView.Width();
	info.height = rcView.Height();

	m_refBrowser = CefBrowserHost::CreateBrowserSync(info,
		this,
		"",
		CefBrowserSettings(),
		NULL);

	return !!m_refBrowser;
}

void BrowserHandler::Close()
{
	if (m_refBrowser)
		m_refBrowser->GetHost()->CloseBrowser(true);
}

void BrowserHandler::CloseAllBrowsers(bool force_close)
{
	if (!CefCurrentlyOn(TID_UI))
	{
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			NewCefRunnableMethod(this, &BrowserHandler::CloseAllBrowsers,
				force_close));
		return;
	}

	if (!m_PopupBrowsers.empty())
	{
		// Request that any popup browsers close.
		BrowserList::const_iterator it = m_PopupBrowsers.begin();
		for (; it != m_PopupBrowsers.end(); ++it)
			(*it)->GetHost()->CloseBrowser(force_close);
	}

	if (m_refBrowser.get())
	{
		// Request that the main browser close.
		m_refBrowser->GetHost()->CloseBrowser(force_close);
	}
}

void BrowserHandler::SetRender(OffScreenRender * pHost)
{
	m_posr = pHost;
}

CefRefPtr<CefBrowser> BrowserHandler::GetBrowser()
{
	return m_refBrowser;
}

void BrowserHandler::RegisterMessageHandler(MessageHandler * handler)
{
	m_pMsgHandler = handler;
}

void BrowserHandler::UnRegisterMessgeHandler(MessageHandler * handler)
{
	m_pMsgHandler = NULL;
}
