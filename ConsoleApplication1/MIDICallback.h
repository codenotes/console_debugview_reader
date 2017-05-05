#pragma once


#include "rtmidi/RtMidi.h"

class GregMidi
{
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

	static void mycallback(double deltatime, std::vector< unsigned char > *message, void *userData)
	{
		unsigned int nBytes = message->size();
		for (unsigned int i = 0; i < nBytes; i++)
			std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
		if (nBytes > 0)
			std::cout << "stamp = " << deltatime << std::endl;
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


};

