// Copyright (c) 2011 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFCLIENT_UTIL_H_
#define CEF_TESTS_CEFCLIENT_UTIL_H_

#pragma once
#include "include/cef_task.h"
#include <windows.h>  // NOLINT(build/include_order)

#ifndef ASSERT
#ifndef NDEBUG
#define ASSERT(condition) if (!(condition)) { DebugBreak(); }
#else
#define ASSERT(condition) ((void)0)
#endif
#endif

#define REQUIRE_UI_THREAD()   ASSERT(CefCurrentlyOn(TID_UI));
#define REQUIRE_IO_THREAD()   ASSERT(CefCurrentlyOn(TID_IO));
#define REQUIRE_FILE_THREAD() ASSERT(CefCurrentlyOn(TID_FILE));

int GetCefMouseModifiers(WPARAM wparam);
int GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam);
BOOL IsKeyDown(WPARAM wparam);
CefKeyEvent ToCefKeyEvent(UINT message, WPARAM wParam, LPARAM lParam);

#endif  // CEF_TESTS_CEFCLIENT_UTIL_H_
#pragma once
