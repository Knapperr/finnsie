#include "log.h"

#include <iostream>

namespace finnsie {

	void Log::Init()
	{
		// TODO(CK): Init a ostream 
		return;
	}

	void Log::Msg(const char* msg, Severity severity)
	{
		// TODO(CK): Don't print to console unless #define VERBOSE
		// just write to a file
		switch (severity)
		{
		case Severity::TRACE:
			std::cout << "TRACE: " << msg << '\n';
			break;
		case Severity::WARN:
			std::cout << "WARNING: " << msg << '\n';
			break;
		case Severity::ERR:
			std::cout << "ERROR: " << msg << '\n';
			EXIT_FAILURE;
			break;
		}
	}

	// TODO(CK): Log
}