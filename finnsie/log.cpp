#include "log.h"

namespace finnsie {

	void Log::Init()
	{
		// TODO(CK): Init file stream?
		return;
	}

	void Log::Msg(const char* msg, Severity severity)
	{
		// TODO(CK): Don't print to console unless #define VERBOSE 
		// just write to a file
		switch (severity)
		{
		case Severity::TRACE:
#if PRINT_LOG
			printf("TRACE: %s\n", msg);
#endif
			break;
		case Severity::WARN:
			printf("WARNING: %s\n", msg);
			break;
		case Severity::ERR:
			printf("ERROR: %s\n", msg);
			EXIT_FAILURE;
			break;
		}
	}

	// TODO(CK): Log
}