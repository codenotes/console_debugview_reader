// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <windows.h>
#include <stdio.h>
#include <vector>
#include "rtmidi/RtMidi.h"
#include "GregDebug/WinDebugMonitor.h"
#include "everything/Everything.h"
#include "Boost/filesystem.hpp"

HANDLE hStdin;
DWORD fdwSaveOldMode;

VOID ErrorExit(LPSTR);
VOID KeyEventProc(KEY_EVENT_RECORD);
VOID MouseEventProc(MOUSE_EVENT_RECORD);
VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD);


class MyMonitor :public CWinDebugMonitor
{

	MyMonitor()
	{
		GatherProcessInformation();
		addInterestedProcs("something.exe");
	}

	virtual void OutputWinDebugString(const char *str, DWORD dwProcessId)
	{
	
		if(isInterested(dwProcessId))
		{
			printf("%s\n", str);
		}
	};
};

void mycallback(double deltatime, std::vector< unsigned char > *message, void *userData)
{
	unsigned int nBytes = message->size();
	int val;
	//byte 7 and 9
//	val = (int)message->at(i);
	if ((*message)[0] == 254) return;

	//std::cout <<  (int)(*message)[8]<<":"<<(int)(*message)[8] << ":" << (int)(*message)[10] << std::endl;
	
	for (unsigned int i = 0; i < nBytes; i++)
	{
	
		val = (*message)[i];
		std::cout << i << ":" << val << " ";
	}
	std::cout << std::endl;
//	if (nBytes > 0)
//		std::cout << "stamp = " << deltatime << std::endl;
}


int testMidiIn()
{
	RtMidiIn *midiin = new RtMidiIn();
	// Check available ports.
	unsigned int nPorts = midiin->getPortCount();
	if (nPorts == 0) {
		std::cout << "No ports available!\n";
		goto cleanup;
	}
	midiin->openPort(0);
	// Set our callback function.  This should be done immediately after
	// opening the port to avoid having incoming messages written to the
	// queue.
	midiin->setCallback(&mycallback);
	// Don't ignore sysex, timing, or active sensing messages.
	midiin->ignoreTypes(false, false, false);
	std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
	char input;
	std::cin.get(input);
	// Clean up
cleanup:
	delete midiin;
	return 0;
}

using namespace std;


//GREG2:rtMidi working example
#pragma comment(lib,"everything64.lib")

std::vector<string> ssr(std::string srch, int max)
{
	Everything_SetSearchA(srch.c_str());
	Everything_QueryA(TRUE);
	std::vector<string> l;

	std::string s;
	char temp[1024];

	auto num = Everything_GetNumResults();

	{
		DWORD i;

		for (i = 0; i < max; i++)
		{

			//s = Everything_GetResultPathA(i);
			Everything_GetResultFullPathNameA(i, temp, 1024);
			boost::filesystem::path p(temp);
			//	s = temp;
			//s += Everything_GetResultFileNameW(i);
			//l.push_back(p.filename().generic_string());
			l.push_back(p.generic_string());
			//l.append(temp);
			//printf("%S\n", Everything_GetResultPathA(i));
			//printf("%S\n", Everything_GetResultFileNameW(i));
		}
	}
	//printf("\nnum:%d\n", num);

	return l;
}

INIT_GREG_RETURN_HANDLER
ANSI_Util con1;

void retHand(const char* str, int loc, DWORD modifier)
{ 
	//ANSI_Util::colors cl = GREEN_ANSI;
	ANSI_Util::PrintAtLoc(22,1,GREEN_DEF,"%s,loc:%d\n", str, loc);

	con1.clearScollingRegion("s1");

	auto l = ssr(str,50);

	int i = 1;

	for (auto &x : l)
	{
		x=GREEN_DEF + std::to_string(i++) + ":"+RESET_DEF+x;
		con1.AppendScrollingRegion("s1", x);
	}



}

int main()
{
//	ANSI_Util au2;

	con1.AddLoc("input", 15, 1);
	con1.StoreScrollingRegionLocation("s1",1,10);
	printf("\x1b\[7l");
	ADD_GREG_RETURN_HANDLER(retHand);
//	MyMonitor m;
	//testMidiIn();

	//au2.PrintAtLoc("input", "test");
	con1.getInputAtLocation("input");

	while (1)
	{


		con1.GetPressedKey(true);

	}
}


int main2(VOID)
{
	DWORD cNumRead, fdwMode, i;
	INPUT_RECORD irInBuf[128];
	int counter = 0;

	// Get the standard input handle. 

	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE)
		ErrorExit("GetStdHandle");

	// Save the current input mode, to be restored on exit. 

	if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
		ErrorExit("GetConsoleMode");

	// Enable the window and mouse input events. 

	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	if (!SetConsoleMode(hStdin, fdwMode))
		ErrorExit("SetConsoleMode");

	// Loop to read and handle the next 100 input events. 

	while (counter++ <= 100)
	{
		// Wait for the events. 

		if (!ReadConsoleInput(
			hStdin,      // input buffer handle 
			irInBuf,     // buffer to read into 
			128,         // size of read buffer 
			&cNumRead)) // number of records read 
			ErrorExit("ReadConsoleInput");

		// Dispatch the events to the appropriate handler. 

		for (i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT: // keyboard input 
				KeyEventProc(irInBuf[i].Event.KeyEvent);
				break;

			case MOUSE_EVENT: // mouse input 
				MouseEventProc(irInBuf[i].Event.MouseEvent);
				break;

			case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
				ResizeEventProc(irInBuf[i].Event.WindowBufferSizeEvent);
				break;

			case FOCUS_EVENT:  // disregard focus events 

			case MENU_EVENT:   // disregard menu events 
				break;

			default:
				ErrorExit("Unknown event type");
				break;
			}
		}
	}

	// Restore input mode on exit.

	SetConsoleMode(hStdin, fdwSaveOldMode);

	return 0;
}

VOID ErrorExit(LPSTR lpszMessage)
{
	fprintf(stderr, "%s\n", lpszMessage);

	// Restore input mode on exit.

	SetConsoleMode(hStdin, fdwSaveOldMode);

	ExitProcess(0);
}

VOID KeyEventProc(KEY_EVENT_RECORD ker)
{
	printf("Key event: ");

	if (ker.bKeyDown)
		printf("key pressed:%c vk:%d vs:%d mask:%d\n",ker.uChar, ker.wVirtualKeyCode, ker.wVirtualScanCode, ker.dwControlKeyState);
	else printf("key released\n");
}

VOID MouseEventProc(MOUSE_EVENT_RECORD mer)
{
#ifndef MOUSE_HWHEELED
#define MOUSE_HWHEELED 0x0008
#endif
	printf("Mouse event: ");

	switch (mer.dwEventFlags)
	{
	case 0:

		if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			printf("left button press \n");
		}
		else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
		{
			printf("right button press \n");
		}
		else
		{
			printf("button press\n");
		}
		break;
	case DOUBLE_CLICK:
		printf("double click\n");
		break;
	case MOUSE_HWHEELED:
		printf("horizontal mouse wheel\n");
		break;
	case MOUSE_MOVED:
		printf("mouse moved\n");
		break;
	case MOUSE_WHEELED:
		printf("vertical mouse wheel\n");
		break;
	default:
		printf("unknown\n");
		break;
	}
}

VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
	printf("Resize event\n");
	printf("Console screen buffer is %d columns by %d rows.\n", wbsr.dwSize.X, wbsr.dwSize.Y);
}

#ifdef _DEBUG
#pragma comment(lib, "C:/Users/gbrill/Dropbox/usr/lib/rtmidid.lib")
#endif

#pragma comment(lib,"winmm.lib")

int midiProbe()
{

		RtMidiIn  *midiin = 0;
		RtMidiOut *midiout = 0;
		// RtMidiIn constructor
		try {
			midiin = new RtMidiIn();
		}
		catch (RtMidiError &error) {
			error.printMessage();
			exit(EXIT_FAILURE);
		}
		// Check inputs.
		unsigned int nPorts = midiin->getPortCount();
		std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
		std::string portName;
		for (unsigned int i = 0; i < nPorts; i++) {
			try {
				portName = midiin->getPortName(i);
			}
			catch (RtMidiError &error) {
				error.printMessage();
				goto cleanup;
			}
			std::cout << "  Input Port #" << i + 1 << ": " << portName << '\n';
		}
		// RtMidiOut constructor
		try {
			midiout = new RtMidiOut();
		}
		catch (RtMidiError &error) {
			error.printMessage();
			exit(EXIT_FAILURE);
		}
		// Check outputs.
		nPorts = midiout->getPortCount();
		std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";
		for (unsigned int i = 0; i < nPorts; i++) {
			try {
				portName = midiout->getPortName(i);
			}
			catch (RtMidiError &error) {
				error.printMessage();
				goto cleanup;
			}
			std::cout << "  Output Port #" << i + 1 << ": " << portName << '\n';
		}
		std::cout << '\n';
		// Clean up
	cleanup:
		delete midiin;
		delete midiout;
		return 0;
	
}

