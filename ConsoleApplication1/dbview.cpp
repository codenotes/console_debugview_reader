
#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdio.h>



#define IfFalseRet(c) do{if(!(c)){return dwLastError = ::GetLastError();}}while(false)

class CHandle
{
public:
	CHandle(HANDLE h = NULL) : m_h(h)
	{
	}
	~CHandle()
	{
		Release();
	}
	void Release()
	{
		if (*this)
		{
			::CloseHandle(m_h);
		}
		m_h = NULL;
	}
	operator bool() const
	{
		return m_h != INVALID_HANDLE_VALUE && m_h != NULL;
	}
	operator HANDLE() const
	{
		return m_h;
	}
	CHandle& operator= (const HANDLE& h)
	{
		Release();
		m_h = h;
		return *this;
	}
	CHandle& operator= (CHandle& h)
	{
		if (this != &h)
		{
			HANDLE hSwap = m_h;
			m_h = h.m_h;
			h.m_h = hSwap;
			h.Release();
		}
		return *this;
	}
private:
	HANDLE m_h;
};

LPCTSTR DBWIN_BUFFER = TEXT("DBWIN_BUFFER");
LPCTSTR DBWIN_BUFFER_READY = TEXT("DBWIN_BUFFER_READY");
LPCTSTR DBWIN_DATA_READY = TEXT("DBWIN_DATA_READY");
LPCTSTR DBWIN_MUTEX = TEXT("DBWinMutex");

#pragma pack(push, 1)
struct CDBWinBuffer
{
	DWORD dwProcessId;
	BYTE  abData[4096 - sizeof(DWORD)];
};
#pragma pack(pop)

bool g_fContinue = true;

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		g_fContinue = false;
		return TRUE;
	}
	return FALSE;
}


void testANSI();

#ifdef _WIN32

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77


#endif

ANSI_Util au2;
#define INPUT_LINE 10
#define INPUT_COL 10

#define LOG(...) ANSI_Util::OutputDebugStringV(__VA_ARGS__)



int __cdecl main()
{
	//testANSI();
	au2.EnableVTMode();
	au2.AddLoc("one", 1, 1);
	au2.StoreScrollingRegionLocation("scroll1", 2, 15);
	au2.SetScrollingRegion("scroll1");
	
	au2.AppendScrollingRegion("scroll1", ANSI_Util::colors::RED, "%s", "one");
	//au2.AppendScrollingRegion("scroll1", ANSI_Util::colors::RED, "two");
	//au2.FillScrollingRegion("scroll1");
	//au2.AppendScrollingRegion("scroll1", ANSI_Util::colors::RED, "%s", "three");
	//au2.FillScrollingRegion("scroll1");
	//au2.ScrollDown("scroll1");
	//return 0;

	DWORD dwLastError = ERROR_SUCCESS;

	IfFalseRet(SetConsoleCtrlHandler((PHANDLER_ROUTINE)(CtrlHandler), TRUE) == TRUE);

	CHandle hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, DBWIN_MUTEX);
	if (!hMutex)
	{
		IfFalseRet(GetLastError() == ERROR_FILE_NOT_FOUND);
		IfFalseRet(hMutex = CreateMutex(NULL, FALSE, DBWIN_MUTEX));
	}

	CHandle hEventBufferReady = OpenEvent(EVENT_MODIFY_STATE, FALSE, DBWIN_BUFFER_READY);
	if (!hEventBufferReady)
	{
		IfFalseRet(GetLastError() == ERROR_FILE_NOT_FOUND);
		IfFalseRet(hEventBufferReady = CreateEvent(NULL, FALSE, TRUE, DBWIN_BUFFER_READY));
	}

	CHandle hEventDataReady = OpenEvent(EVENT_MODIFY_STATE, FALSE, DBWIN_DATA_READY);
	if (!hEventDataReady)
	{
		IfFalseRet(GetLastError() == ERROR_FILE_NOT_FOUND);
		IfFalseRet(hEventDataReady = CreateEvent(NULL, FALSE, FALSE, DBWIN_DATA_READY));
	}

	CHandle hFileMappingBuffer = OpenFileMapping(FILE_MAP_READ, FALSE, DBWIN_BUFFER);
	if (!hFileMappingBuffer)
	{
		IfFalseRet(GetLastError() == ERROR_FILE_NOT_FOUND);
		IfFalseRet(hFileMappingBuffer = CreateFileMapping(
			INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,
			0, sizeof(CDBWinBuffer), DBWIN_BUFFER));
	}

	CDBWinBuffer* pDbgBuffer = (CDBWinBuffer*)(MapViewOfFile(
		hFileMappingBuffer, SECTION_MAP_READ, 0, 0, 0));
	IfFalseRet(pDbgBuffer);
	
	while (g_fContinue)
	{
		if (WaitForSingleObject(hEventDataReady, 100) == WAIT_OBJECT_0)
		{
			au2.AppendScrollingRegion("scroll1", ANSI_Util::colors::BOLDCYAN, "%s", (const char*)pDbgBuffer->abData);
//			au2.PrintAtLoc("one", ANSI_Util::BOLDCYAN, "%s", (const char*)pDbgBuffer->abData);
	//		au2.CurPos(20, 1);
		//	printf("%s", pDbgBuffer->abData);
			
			SetEvent(hEventBufferReady);
			au2.FillScrollingRegion("scroll1");
			au2.ScrollDown("scroll1");
	//		au2.FillScrollingRegion("scroll1");
		//	au2.ScrollDown("scroll1");
		}
	}

	UnmapViewOfFile(pDbgBuffer);

	return dwLastError;
}