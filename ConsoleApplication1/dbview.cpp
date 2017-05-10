
#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdio.h>
#include "TlHelp32.h"
#include <comutil.h>


#define WINSIZE 6
#define STARTING_LINE 2
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

#include <iostream>
#include <thread>

int lpos = 1, numlines = 2;
//int start = lpos, finish = 0;

int crap;
using namespace std;

#define OFFSET 2

void stest(vector<string> &vec, int start)
{
	auto szVec = vec.size();
	int last_starting = start;
	//int cnt = 0;
	//for (auto s :vec )
	for(auto it=(vec.begin()+ (start-1));it!=vec.end();it++)
	{
		if (last_starting > numlines) //window filled, should scrill
		{
			Beep(450, 300);
		//	cin >> crap; //pause here
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			printf(CSI_DEF "2J"); // Clear screen						 
		//delete everything
			//
			au2.CurPos(CURSOR_SAVE);
			au2.CurPos(10, 1);
			printf("start:%d", start);
			au2.CurPos(CURSOR_RESTORE);
			stest(vec, start + 1 );
			break;
		}
		else
		{
			au2.CurPos(last_starting, 1);
			printf("%s%d", it->c_str(),last_starting);
			last_starting++;
		}
	}

}

void stest2(vector<string> &vec, int start)
{

}


bool printAndAvance(vector<string> &vec,int advancePos, int scrollStartLine, int winsize)
{
	//int winsize = 2;
	if (advancePos < 0)
	{
		Beep(450, 50);
		return false;
	}
	int topofwin =0;
	int bottomwin=0 ;
	int sz = vec.size();

	//printf(CSI_DEF "2J"); // Clear screen		

	//printf(CURSOR_SAVE);
	//au2.CurPos(scrollStartLine, 1);

	//for (int j = 0; j <= winsize; j++)
	//{
	//	printf("\n--");
	//}
	//printf(CURSOR_RESTORE);


	//au2.CurPos(scrollStartLine, 1);

	//old issue
	//it is advancing the string and scrolling the vector BEFORE reaching the end of the window.
	//that is the issue.  Change is, do NOT advance topwin UNLESS the buffer is filled.
	//check: do we have less to print than our window size is? If so, set topwin to 0

	if (sz <= winsize)
	{
		topofwin = 0;// scrollStartLine;
	}
	else
	{
		topofwin = advancePos;

	}

	bottomwin = topofwin + winsize;
	int i = 0;

	
	for (i = topofwin; (i < bottomwin) /*&& (i < sz)*/; i++)
	{
		au2.CurPos(scrollStartLine++, 1);
		printf(DL);
		if (i < sz) 
			//printf("%s-sl:%d", vec[i].c_str(),scrollStartLine);
			printf("%s", vec[i].c_str());
		else
			;//		printf("---");
	}



	return true;
}


bool pushBuffString(vector<string> & vec, string s, int scrollStartLine, int winsize, bool autoScroll=true)
{
	vec.push_back(s);
	int sz = vec.size();
	static int  lastpos = 0;
	
	auto cntWindows =(int)floor(( (float)sz / (float)winsize) );
	

	//what top and bottom positions the window on the lest N elements of array


	//printf(CURSOR_SAVE);
	//au2.CurPos(10, 10);
	//au2.PrintAndRestore(10,10,"cnt:%d", cntWindows);
	//printf("cnt:%d", cntWindows);
	//printf(CURSOR_RESTORE);

	//what is the position on the top of the Nth window?
	//auto pos = cntWindows*winsize;

	//if (autoScroll)
	//{
	//	//get size and pass that in as pos
	//	return printAndAvance(vec, lastpos++, scrollStartLine, winsize);
	//}

	return true;
}


void testansi2()
{
	au2.EnableVTMode();
	std::vector<std::string> vec;

	char temp[333];
	sprintf(temp, "\033[%d;%dr", STARTING_LINE, WINSIZE + 1); //seems to be inclusive, so added 1
	printf(temp);


	//pushBuffString(vec, temp, STARTING_LINE, WINSIZE, false);

	vec.push_back("one");
	vec.push_back("two");
	vec.push_back("three");
	vec.push_back("four");
	//	vec.push_back("five");
	//	vec.push_back("six");

	//vec.push_back("one");
	//vec.push_back("two");
	//vec.push_back("three");
	//vec.push_back("four");

	int x = 0;

	//while (x<4)
	//{
	//	printAndAvance(vec,x++,1,2);
	//	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	//}
	int pos = 0;
	printAndAvance(vec, 0, STARTING_LINE, WINSIZE);


	au2.AddLoc("input1", 10, 10);
	int adder = 0;
	while (1)
	{
		switch (au2.GetPressedKey(true))
		{
		case VK_LEFT:
			vec.push_back("five");
			printAndAvance(vec, 2, STARTING_LINE, WINSIZE);
			break;

		case VK_UP:
			printAndAvance(vec, --pos, STARTING_LINE, WINSIZE);
			break;

		case VK_DOWN:
			printAndAvance(vec, ++pos, STARTING_LINE, WINSIZE);
			break;

		case VK_RIGHT:
			//au2.GetInputAtLocation("input1", temp,10);
			//pushBuffString(vec, "greg", 1,2, true);
			sprintf(temp, "added:%d", adder++);
			pushBuffString(vec, temp, STARTING_LINE, WINSIZE, false);
			printAndAvance(vec, pos++, STARTING_LINE, WINSIZE);
			//	pos++;

			break;
		}
	}

	//	pushBuffString(vec, "five", 1, 2, true);
	printf("\n");
}


void getConsoleWidthAndHeight(int &columns, int&rows)
{

#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO csbi;


	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
//#include <sys/ioctl.h>
//#include <stdio.h>
//#include <unistd.h>
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
#endif
	
	return;
}


void midiTestStuffForMain()
{
//	midiProbe();
	//testMidiIn();
	
	ANSI_Util term;

	char temp[255];
	term.StoreScrollingRegionLocation("scroll1", 2, 4); //4 lines, adding 1 because seems to be necessary
	term.SetScrollingRegion("scroll1");

	term.drawHline(6);
	term.drawVline(12, 0, 7);

	term.pushBuffString("scroll1", "one");
	term.pushBuffString("scroll1", "two");
	term.pushBuffString("scroll1", "three");
	term.pushBuffString("scroll1", "four");
	term.pushBuffString("scroll1", "five");

	term.CurPos(10, 10);

	//const char*c = LOCIT(5, 10);

	//printf(LOCIT(20,20) " !!!!!!!!!!!!!!!!");
	//return 0;

	int xx = 0;
	int pos = 0;
	while (1)
	{
		switch (term.GetPressedKey(true))
		{

		case VK_LEFT:
			break;
		case VK_UP:
			term.ScrollUp("scroll1");// , ++pos);
			break;
		case VK_DOWN:
			term.ScrollDown("scroll1");// , --pos);
			break;
		case VK_RIGHT:
			sprintf(temp, "pushed:%d", xx++);
			term.pushBuffString("scroll1", temp);

			break;
		}
	}

}

//printf("\033[%d;%dH", L, N);


void ANSITestStuff()
{
	//testANSI();

	//printAndAvance(vec, 3, 1, 2);
	//pushBuffString(vec, "six", 1, 2, true);


	//stest(vec, lpos);
	//printf("\n");


	//au2.AddLoc("one", 1, 1);
	//au2.StoreScrollingRegionLocation("scroll1", 2, 3);
	//au2.SetScrollingRegion("scroll1");
	//
	//au2.AppendScrollingRegion("scroll1", ANSI_Util::colors::RED, "%s", "one");
	//au2.AppendScrollingRegion("scroll1", ANSI_Util::colors::RED, "two");
	//au2.AppendScrollingRegion("scroll1", ANSI_Util::colors::RED, "three");
	//au2.AppendScrollingRegion("scroll1", ANSI_Util::colors::RED, "four");
	//	au2.FillScrollingRegion("scroll1");
	//au2.AppendScrollingRegion("scroll1", ANSI_Util::colors::RED, "%s", "three");
	//au2.ScrollDown("scroll1");
	//au2.FillScrollingRegion("scroll1");
	//au2.ScrollDown("scroll1");
	//return 0;
}


int midiProbe();
int testMidiIn();

#include <set>

std::map<DWORD, bstr_t> procs;
std::set<bstr_t> interestedProcs;


#pragma comment(lib,"comsuppw.lib")


void GatherProcessInformation()
{

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot) {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe32)) {
			do
			{
				procs[pe32.th32ProcessID] = pe32.szExeFile;
				//printf("pid %d %s\n", pe32.th32ProcessID, pe32.szExeFile);
				printf("pid %d %s\n", pe32.th32ProcessID, (const char*) (procs[pe32.th32ProcessID] ));

			} while (Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
	}
}


USE_GREG_PRINT_HANDLER
USE_GREG_CONSOLE_HANDLER

int __cdecl main_original()
{
	interestedProcs.insert("appSrcTestConsole.exe"); //what I am interested in

	GatherProcessInformation();

	ANSI_Util au;
	printf(GREEN_DEF "started...\n" RESET_DEF);
	//au.AddLoc("one", 10, 1);

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
	int i = 0;
	bstr_t bt;

	while (g_fContinue) //GREG2: reading from debugOutputString, ones own output console
	{
		if (WaitForSingleObject(hEventDataReady, 100) == WAIT_OBJECT_0)
		{
			
			bt = procs[pDbgBuffer->dwProcessId];
			
			if (interestedProcs.count(bt))
			{

				printf("%s", pDbgBuffer->abData);

			}
			SetEvent(hEventBufferReady);
/*
			au2.AppendScrollingRegion("scroll1", ANSI_Util::colors::BOLDCYAN, "%d", i++);// , (const char*)pDbgBuffer->abData);
//			au2.PrintAtLoc("one", ANSI_Util::BOLDCYAN, "%s", (const char*)pDbgBuffer->abData);
	//		au2.CurPos(20, 1);
		//	printf("%s", pDbgBuffer->abData);
			
			SetEvent(hEventBufferReady);
//			au2.FillScrollingRegion("scroll1");
//			au2.ScrollDown("scroll1");
			au2.FillScrollingRegion("scroll1");
			au2.ScrollDown("scroll1");
			*/
		}
	}

	UnmapViewOfFile(pDbgBuffer);

	return dwLastError;
}