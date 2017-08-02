
#include "include/cef_client.h"
#include <DbgHelp.h>
#include <shlobj.h>
#pragma comment(lib, "dbghelp.lib")
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include "SubProcessClientApp.h"

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	// 创建Dump文件   
	HANDLE hDumpFile = CreateFileW(lpstrDumpFilePathName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// Dump信息   
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	// 写入Dump文件内容   
	MiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(),
		hDumpFile,
		MiniDumpNormal,
		&dumpInfo,
		NULL,
		NULL);

	CloseHandle(hDumpFile);
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	TCHAR szCurrentDir[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szCurrentDir, MAX_PATH);
	(wcsrchr(szCurrentDir, TEXT('\\')))[1] = 0;

	TCHAR szDumpFileDir[MAX_PATH];
	wsprintf(szDumpFileDir, TEXT("%s\\Log\\DumpLog"), szCurrentDir);
	SHCreateDirectoryEx(NULL, szDumpFileDir, NULL);

	TCHAR szDumpFilePath[MAX_PATH];
	wsprintf(szDumpFilePath, TEXT("%s\\WebProcess.dmp"), szDumpFileDir);
	CreateDumpFile(szDumpFilePath, pException);

	MessageBox(NULL, szDumpFilePath, TEXT("呀，好像出错了！-_-!"), 0);
	TerminateProcess(GetCurrentProcess(), 0);

	return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char* argv[])
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

	CefMainArgs main_args(GetModuleHandle(NULL));

	CefRefPtr<SubProcessClientApp> app(new SubProcessClientApp());

	// 执行子进程逻辑，此时会堵塞直到子进程退出。
	return CefExecuteProcess(main_args, app.get(), NULL);
}
