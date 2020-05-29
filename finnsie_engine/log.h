#ifndef LOG_HG_
#define LOG_HG_

#include <ostream>
#include <sstream>

namespace finnsie {

	enum class Severity
	{
		ERR,
		WARN,
		TRACE
	};

	class Log
	{
	public:
		static void Init();
		static void Msg(const char* msg, Severity severity);

	private:

	};
}

#define PRINT_LOG 0
#define LOG(msg) finnsie::Log::Msg(msg, Severity::TRACE)
#define LOG_WARN(msg) finnsie::Log::Msg(msg, Severity::WARN)
#define LOG_ERR(msg) finnsie::Log::Msg(msg, Severity::TRACE)

#endif